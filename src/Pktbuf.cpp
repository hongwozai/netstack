/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Pktbuf.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：Pktbuf
 ** 注  意：1.
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
    }
    ppool.detach(other);
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

    pkt->total_len = reserve + len;
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