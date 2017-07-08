/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_pktbuf.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：pkt测试程序
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Pktbuf.h"

int main(int argc, char *argv[])
{
    Pktbuf *p, *pp[10240];
    Pktbuf::hunk *h;

    RetType ret = Pktbuf::init(1024, 1024, 1514);
    if (ret != OK) exit(-1);

    // 1.
    p = Pktbuf::alloc(Pktbuf::ALLOCATOR, 100, Pktbuf::NONE);
    if (!p) exit(-1);
    h = p->hlist.locate(p->hlist.head);
    memset(h->data, 0, 100);
    Pktbuf::free(p);

    // 2.
    for (int i = 0; i < 1025; i++) {
        pp[i] = Pktbuf::alloc(Pktbuf::ALLOCATOR, 100, Pktbuf::NONE);
        if (!pp[i]) exit(-1);
        Pktbuf::hunk *h = pp[i]->hlist.locate(pp[i]->hlist.head);
        memset(h->data, 0, 100);
    }
    for (int i = 0; i < 1025; i++) {
        Pktbuf::free(pp[i]);
    }

    // 3.
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 100, Pktbuf::NONE);
    if (!p) exit(-1);
    h = p->hlist.locate(p->hlist.head);
    memset(h->data, 0, 100);
    Pktbuf::free(p);

    // 4.
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 5000, Pktbuf::NONE);
    if (!p) exit(-1);
    List_foreach(p->hlist.head, temp) {
        h = p->hlist.locate(temp);
        memset(h->data, 0, 1514);
    }
    if (p->hlist.count != 4)
        exit(-1);
    Pktbuf::free(p);

    // 5.
    for (int i = 0; i < 10000; i++) {
        pp[i] = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::NONE);
        if (!pp[i]) exit(-1);
        List_foreach(p->hlist.head, temp) {
            h = p->hlist.locate(temp);
            memset(h->data, 0, 1514);
        }
        Pktbuf::free(pp[i]);
    }

    // 6. cat
    pp[1] = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::NONE);
    pp[2] = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::NONE);
    if (pp[1]->hlist.count != 2) exit(-1);
    if (pp[2]->hlist.count != 2) exit(-1);
    pp[1]->cat(pp[2]);
    if (pp[1]->hlist.count != 4) exit(-1);
    Pktbuf::free(pp[1]);

    // 7. header 0
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::NONE);
    p->header(0);

    // 8. header > 0
    char *payload1, *payload2, *payload3;
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::NONE);
    if (p->hlist.count != 2) exit(-1);
    payload1 = p->hlist.locate(p->hlist.head)->payload;
    p->header(14);
    payload2 = p->hlist.locate(p->hlist.head)->payload;
    if (p->hlist.count != 2) exit(-1);
    if (payload2 - payload1 != 14) exit(-1);

    p->header(1500);
    if (p->hlist.count != 1) exit(-1);
    Pktbuf::free(p);

    // 9. header < 0
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::TCP);
    if (p->hlist.count != 3) exit(-1);

    payload1 = p->hlist.locate(p->hlist.head)->payload;
    p->header(-20);
    payload2 = p->hlist.locate(p->hlist.head)->payload;
    if (payload2 - payload1 != -20) exit(-1);

    p->header(20);
    payload3 = p->hlist.locate(p->hlist.head)->payload;
    if (payload1 != payload3) exit(-1);

    p->header(-100);
    if (p->hlist.count != 4) exit(-1);

    p->header(2500);
    if (p->hlist.count != 2) exit(-1);

    p->header(20);
    if (p->hlist.count != 2) exit(-1);
    Pktbuf::free(p);

    // 10, 测试total_len
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::TCP);
    if (p->total_len != (3000 + 20 + 20 + 14)) exit(-1);
    p->header(20);
    if (p->total_len != 3000 + 20 + 14) exit(-1);
    p->header(2000);
    if (p->total_len != 1034) exit(-1);
    Pktbuf::free(p);

    // 11. split
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 3000, Pktbuf::NONE);
    if (!p) exit(-1);
    Pktbuf *temp = p->split(1500);
    printf("len: %d\n", temp->total_len);
    if (temp->total_len != 1500) exit(-1);
    h = temp->hlist.locate(temp->hlist.head);
    printf("h->len: %d\n", h->len);
    if (h->len != 1500) exit(-1);

    printf("plen: %d\n", p->total_len);
    if (p->total_len != 1500) exit(-1);
    h = p->hlist.locate(p->hlist.head);
    if (h->len != 14) exit(-1);
    Pktbuf::free(p);

    // 11.2 split
    p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 5000, Pktbuf::NONE);
    if (!p) exit(-1);
    temp = p->split(2000);
    printf("1len: %d\n", temp->total_len);
    if (temp->total_len != 2000) exit(-1);
    h = temp->hlist.locate(temp->hlist.head);
    if (h->len != 1514) exit(-1);

    h = p->hlist.locate(p->hlist.head);
    printf("h->len: %d\n", h->len);
    if (h->len != 1028) exit(-1);

    Pktbuf::free(p);
    Pktbuf::destroy();
    return 0;
}
