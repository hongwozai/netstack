/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：ArpCache.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：arp缓存表
 ** 注  意：1.
 ********************************************************************/
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include "ArpCache.h"

Errno ArpCache::init(uint8_t mod, uint64_t timeout)
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
    if (0 != npool.init(128)) {
        free(buckets);
        return MEM_FAIL;
    }

    timechain.config(offsetof(Node, timelink));
    return OK;
}

Errno ArpCache::add(uint32_t ip, NetIf *device, NodeType type, uint8_t mac[6])
{
    Node *n = npool.attach();
    if (!n)
        return MEM_FAIL;
    n->type = type;
    n->ip   = ip;
    n->device = device;
    n->expires = time(0) + timeout;
    if (mac) {
        memcpy(n->mac, mac, 6);
    }
    n->plist.config(offsetof(Pktbuf, link));

    // 挂在时间链上，静态节点不入时间链
    if (n->type != STATIC) {
        // 尾查，头部最旧
        timechain.append(n);
    }
    buckets[ip & mask].push(&n->link);
    count++;
    return OK;
}

ArpCache::Node* ArpCache::find(uint32_t ip)
{
    HList_foreach(buckets[ip & mask].head, temp) {
        Node *n = (Node*)temp;
        if (n->ip == ip) {
            return n;
        }
    }
    return NULL;
}

void ArpCache::update()
{
    ListLink *temp, *next;
    List_safe_foreach(timechain.head, temp, next) {
        Node *n = timechain.locate(timechain.head);
        if (n->expires < (uint32_t)time(0)) {
            timechain.detach(&n->timelink);
            buckets[0].del(&n->link);
            npool.detach(n);
            count--;
        } else {
            break;
        }
    }
}

bool ArpCache::del(uint32_t ip)
{
    HashLink *temp, *next;
    HList_safe_foreach(buckets[ip & mask].head, temp, next) {
        Node *n = (Node*)temp;
        if (n->ip == ip) {
            timechain.detach(&n->timelink);
            buckets[ip & mask].del(&n->link);
            npool.detach(n);
            count--;
            return true;
        }
    }
    return false;
}

void ArpCache::destroy()
{
    free(buckets);
    npool.destroy();
}