/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：FragTable.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-06
 **
 ** 描  述：ip分片表
 ** 注  意：1. TODO: 到处使用的ntoh16，改为变量记录来加快速度
 ********************************************************************/
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include "Protocol/Proto.h"
#include "Netutils.h"
#include "FragTable.h"

using namespace Proto;

RetType FragTable::init(uint8_t mod, uint64_t timeout)
{
    memset(this, 0, sizeof(*this));
    this->timeout = timeout;
    mask = (1 << mod) - 1;

    // hash数组
    buckets = (HList*)malloc(sizeof(HList) * (mask + 1));
    if (!buckets)
        return MEM_FAIL;

    for (uint32_t i = 0; i < (mask + 1); ++i) {
        buckets[i].init();
    }

    // 内存池
    if (0 != npool.init(1024)) {
        free(buckets);
        return MEM_FAIL;
    }

    timechain.config(offsetof(Node, timelink));
    return OK;
}

void FragTable::destroy()
{
    free(buckets);
    npool.destroy();
    return;
}

void FragTable::print()
{
    char src_ip[16], dst_ip[16];
    for (uint32_t i = 0; i < (mask + 1); i++) {
        HList_foreach(buckets[i].head, temp) {
            Node *n = (Node*)temp;
            printf("%s -> %s id: %u pkt: %d\n",
                   Net::IP4Addr(n->src_ip, src_ip),
                   Net::IP4Addr(n->dst_ip, dst_ip),
                   n->ipid,
                   n->plist.count);
        }
    }
    return;
}

bool FragTable::del(Node *n)
{
    ListLink *temp, *next;
    List_safe_foreach(n->plist.head, temp, next) {
        Pktbuf *p = n->plist.locate(temp);
        n->plist.detach(temp);
        Pktbuf::free(p);
    }
    timechain.detach(&n->timelink);
    buckets[0].del(&n->link);
    npool.detach(n);
    count--;
    return false;
}

void FragTable::update()
{
    ListLink *temp, *next;
    List_safe_foreach(timechain.head, temp, next) {
        Node *n = timechain.locate(timechain.head);
        if (n->expires < (uint64_t)time(0)) {
            timechain.detach(&n->timelink);
            buckets[0].del(&n->link);
            del(n);
        } else {
            break;
        }
    }
}

/**
 * NOTE: 现在的分片丢失的处理策略就是等待超时时进行回收
 * NOTE: 对于偏移与长度等队不上的包直接丢弃，并等待超时回收
 */
RetType FragTable::add(uint32_t dst_ip, uint32_t src_ip, uint16_t ipid,
                       Pktbuf *&p)
{
    Node  *n;
    IPHdr *ip;
    uint32_t index;
    uint32_t offset;
    Pktbuf *tp;
    Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);

    // 调用者检查过，此处直接使用(可以确保长度合规)
    ip = (IPHdr*)h->payload;
    offset  = (ntoh16(ip->ip_off) & IP_OFFMASK) << 3;

    index = hash(dst_ip, src_ip, ipid);
    // 查找节点，存在则拼分片，不存在则新建节点
    HList_foreach(buckets[index].head, temp) {
        n = (Node*)temp;
        if (n->dst_ip == dst_ip && n->src_ip == src_ip &&
            n->ipid == ipid) {
            if ((ntoh16(ip->ip_off) & IP_MF) == 0) {
                n->havelast = true;
            }
            goto insert;
        }
    }

    // 新建节点流程
    n = npool.attach();
    if (!n) return MEM_FAIL;

    n->dst_ip = dst_ip;
    n->src_ip = src_ip;
    n->ipid   = ipid;
    n->plist.config(offsetof(Pktbuf, link));
    n->expires = time(0) + timeout;
    timechain.append(n);
    n->havelast = false;
    // 判断p的mf是否为空
    if ((ntoh16(ip->ip_off) & IP_MF) == 0) {
        n->havelast = true;
    }
    // 记录数据包
    n->plist.append(p);
    buckets[index].push(&n->link);
    return USE_PKT;

insert:
    // 1.按序插入链表
    // NOTE: 从后向前比较，因为多数的数据包都是按顺序到达，向后比较判断次数最少
    List_rforeach(n->plist.tail, temp) {
        uint32_t curoff;
        tp = n->plist.locate(temp);
        h = tp->hlist.locate(tp->hlist.head);
        ip = (IPHdr*)h->payload;
        curoff = ((ntoh16(ip->ip_off) & IP_OFFMASK) << 3)
            + ntoh16(ip->ip_len) - (ip->ip_hl << 2);
        // 找到合适的位置
        if (curoff <= offset) {
            n->plist.append(p);

            if (n->havelast) goto defrag;
            return USE_PKT;
        }
    }
    n->plist.push(p);
    if (!n->havelast)
        return USE_PKT;

defrag:
    // 2.判断分片是否到齐，到齐则重组
    if ((tp = defrag(n))) {
        p = tp;
        // 删除节点
        del(n);
        return OK_WITH_DEFRAG;
    }
    return USE_PKT;
}

Pktbuf *FragTable::defrag(Node *node)
{
    bool    isall = true;
    IPHdr  *ip;
    uint32_t len = 0, offset = 0;

    // 检查是否到齐, offset当前包偏移，len代表之前包的长度
    List_foreach(node->plist.head, temp) {
        Pktbuf *p = node->plist.locate(temp);
        Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);
        ip = (IPHdr*)h->payload;
        offset = (ntoh16(ip->ip_off) & IP_OFFMASK) << 3;
        if (len != offset) {
            isall = false;
            break;
        }
        len = offset + ntoh16(ip->ip_len) - (ip->ip_hl << 2);
    }

    // 重组
    if (isall == true) {
        Pktbuf *p, *first = NULL;
        IPHdr  *firstip;
        Pktbuf::hunk *h;
        while (node->plist.count) {
            p = node->plist.pop();
            h = p->hlist.locate(p->hlist.head);
            ip = (IPHdr*)h->payload;
            if (!first) {
                // 第一个分片
                first   = p;
                firstip = ip;
                len     = ntoh16(ip->ip_len);
                continue;
            }
            p->header(ip->ip_hl << 2);
            first->cat(p);
            len += ntoh16(ip->ip_len) - (ip->ip_hl << 2);
        }
        // 更改第一个包的ip字段值并从链表上摘下
        firstip->ip_off  = 0;
        firstip->ip_off &= ~(IP_MF | IP_OFFMASK);
        firstip->ip_off  = hton16(firstip->ip_off);
        firstip->ip_len  = hton16(len);
        return first;
    }
    return NULL;
}