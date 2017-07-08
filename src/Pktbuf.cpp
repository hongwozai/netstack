/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Pktbuf.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：Pktbuf
 ** 注  意：1. NOTE: 分配原则：下层分配 -> 下层释放，部分上层释放（重组数据包时）
 **                         上层分配 -> 下层释放
 ********************************************************************/
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "Pktbuf.h"
#include "Protocol/Proto.h"

using namespace Proto;

MPool<Pktbuf>       Pktbuf::ppool;
MPool<Pktbuf::hunk> Pktbuf::hpool;
uint32_t            Pktbuf::hsize;

RetType Pktbuf::header(int size)
{
    hunk *head, *h;

    if (size == 0) return OK;

    if (hlist.count == 0)
        return ARGS_ERROR;
    head = hlist.locate(hlist.head);
    if (size < 0) {
        // 取反
        size = -size;
        // 减去预留的空间，不足则分配
        if (head->payload - size >= head->data) {
            // 可以安全的移动
            head->payload -= size;
            total_len += size;
            return OK;
        } else {
            // 内存不够
            size -= head->payload - head->data;
            head->payload = head->data;
            total_len += size;
            // 选择使用两种分配方式
            if ((unsigned)size <= hsize) {
                h = hpool.attach();
                if (!h)
                    return MEM_FAIL;
                h->type = FIXEDPOOL;
                h->payload = h->data + hsize - size;
                h->len = size;
            } else {
                h = (hunk*)malloc(size);
                if (!h)
                    return MEM_FAIL;
                h->type = ALLOCATOR;
                h->payload = h->data;
                h->len = size;
            }
            hlist.push(h);
        }
        return OK;
    }
    if (size > 0) {
        // 向后挪移指针，直至全部挪走
        ListLink *temp, *next;
        List_safe_foreach(hlist.head, temp, next) {
            h = hlist.locate(temp);
            if ((unsigned)size < h->len) {
                head->payload += size;
                head->len     -= size;
                total_len     -= size;
                break;
            }
            size -= h->len;
            total_len -= h->len;
            hlist.detach(&h->link);
            hpool.detach(h);
        }
        return OK;
    }
    return OK;
}

void Pktbuf::cat(Pktbuf *other)
{
    ListLink *temp, *next;
    List_safe_foreach(other->hlist.head, temp, next) {
        hunk *h = other->hlist.locate(temp);
        other->hlist.detach(temp);
        hlist.append(h);
        total_len += h->len;
    }
    ppool.detach(other);
}

Pktbuf *Pktbuf::split(unsigned size)
{
    Pktbuf *p = NULL;
    ListLink *temp, *next;

    if (size >= total_len) {
        return NULL;
    }

    // 申请创建出来的pktbuf
    p = ppool.attach();
    if (!p)
        return NULL;
    p->total_len = 0;
    p->isvlan  = false;
    p->hlist.config(offsetof(hunk, link));

    List_safe_foreach(hlist.head, temp, next) {
        Pktbuf::hunk *h = hlist.locate(temp);
        if (size >= h->len) {
            // 去掉
            hlist.detach(temp);
            total_len    -= h->len;
            // 添加到新的pktbuf
            p->hlist.append(h);
            p->total_len += h->len;
            size -= h->len;
        } else {
            Pktbuf::hunk *newh;
            if (size >= hsize) {
                newh = (hunk*)malloc(sizeof(hunk) + size);
            } else {
                newh = hpool.attach();
            }
            if (!newh)
                goto fail;
            newh->len  = size;
            newh->type = (size >= hsize) ? ALLOCATOR : FIXEDPOOL;
            // 拷贝数据至新的节点
            memcpy(newh->data, h->payload, size);
            newh->payload = newh->data;
            p->hlist.append(newh);
            p->total_len += size;
            // 删除当前数据
            h->payload += size;
            h->len     -= size;
            total_len  -= size;
            break;
        }
    }
    return p;
fail:
    ppool.detach(p);
    return NULL;
}

RetType Pktbuf::init(int pnum, int hnum, uint32_t hsize)
{
    uint32_t real = hsize + sizeof(hunk);

    if (ppool.init(pnum)) {
        return MEM_FAIL;
    }

#define HUNK_LEN (sizeof(hunk)+sizeof(EtherHdr)+sizeof(IPHdr)+sizeof(TcpHdr))
    real = (real < HUNK_LEN) ? HUNK_LEN : real;
    if (hpool.init(hnum, real)) {
        hpool.destroy();
        return MEM_FAIL;
    }
    Pktbuf::hsize = real - sizeof(hunk);
    return OK;
}

void Pktbuf::destroy()
{
    ppool.destroy();
    hpool.destroy();
}

Pktbuf *Pktbuf::alloc(AllocType type, uint32_t len, ReserveType layer)
{
    uint32_t reserve;
    Pktbuf *pkt = NULL;
    hunk *h = NULL;

    // 确定保留的字节数
    switch (layer) {
    case TCP:
        reserve = sizeof(TcpHdr) + sizeof(IPHdr) + sizeof(EtherHdr);
        break;
    case UDP:
        reserve = sizeof(UdpHdr) + sizeof(IPHdr) + sizeof(EtherHdr);
        break;
    case IP:
        reserve = sizeof(IPHdr) + sizeof(EtherHdr);
        break;
    case ETHER:
        reserve = sizeof(EtherHdr);
        break;
    case NONE:
        reserve = 0;
        break;
    }
    // TODO: 暂时不支持保留长度小于内存池长度的
    if (type == FIXEDPOOL && reserve >= hsize) {
        return NULL;
    }

    // 申请并填充控制结构
    pkt = ppool.attach();
    if (!pkt)
        return NULL;

    pkt->total_len = len;
    pkt->isvlan    = false;
    pkt->hlist.config(offsetof(hunk, link));

    // 根据类型决定如何申请内存
    switch (type) {
    case ALLOCATOR:
        h = (hunk*)malloc(sizeof(hunk) + reserve + len);
        if (!h) goto fail;

        h->type = ALLOCATOR;
        h->payload = h->data + reserve;
        h->len = len;
        pkt->hlist.append(h);
        break;

    case FIXEDPOOL:
        for (int temp = reserve + len; temp > 0; temp -= hsize) {
            h = hpool.attach();
            if (!h) goto fail;

            h->type = FIXEDPOOL;
            if ((uint32_t)temp == reserve + len) {
                if ((uint32_t)temp > hsize) {
                    h->payload = h->data + reserve;
                    h->len = hsize - reserve;
                } else {
                    // 此时reserve <= hsize - len
                    h->payload = h->data + hsize - len;
                    h->len = len;
                }
            } else {
                h->payload = h->data;
                h->len = ((uint32_t)temp > hsize) ? hsize : temp;
            }
            pkt->hlist.append(h);
        }
        break;

    default:
        goto fail;
    }
    return pkt;

fail:
    free(pkt);
    return NULL;
}

void Pktbuf::free(Pktbuf *pkt)
{
    ListLink *temp, *next;
    List_safe_foreach(pkt->hlist.head, temp, next) {
        hunk *h = pkt->hlist.locate(temp);

        pkt->hlist.detach(temp);
        switch (h->type) {
        case ALLOCATOR:
            ::free(h);
            break;

        case FIXEDPOOL:
            hpool.detach(h);
            break;

        default:
            break;
        }
    }
    ppool.detach(pkt);
}