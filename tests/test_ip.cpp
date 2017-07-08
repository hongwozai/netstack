/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_ip.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-07
 **
 ** 描  述：ip层测试
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "IP.h"
#include "Netutils.h"
#include "EtherIf.h"
#include "ArpCache.h"
#include "PcapDriver.h"

using namespace Net;

int main(int argc, char *argv[])
{
    ArpCache cache;
    EtherIf device;
    PcapDriver *pcap = new PcapDriver();
    uint8_t mac[6], mac1[6];
    RetType ret;
    IP *ip = IP::instance();
    RouteTable rt;
    Pktbuf *p;

    Pktbuf::init(1024, 1024, 1514);
    cache.init();
    rt.init();
    pcap->init(true, "../../tests/frag.pcap");
    pcap->init("eth2");
    device.NetIf::init("pcap0", Net::IP4Addr("192.168.100.29"), 1500, pcap);
    device.init(Net::MACAddr("00:23:24:44:71:91", mac), &cache);

    // 缓存
    cache.add(IP4Addr("192.168.101.28"), &device,
              ArpCache::STABLE, Net::MACAddr("78:24:af:86:8c:7d", mac1));
    // 路由
    rt.add(IP4Addr("192.168.101.0"), IP4Addr("255.255.255.0"), 0, &device);
    ret = ip->init(&rt);
    if (ret != OK) exit(-1);

    // 1. 小包
    for (;;) {
        if (OK != pcap->linkinput(&p))
            break;
        ret = device.input(p);
        printf("ret: %d\n", ret);
    }
    if (ret != FREED_PKT) exit(-1);
    pcap->destroy();

    // 2. 大包
    printf("=======\n");
    pcap->init(true, "../../tests/frag1.pcap");
    for (;;) {
        if (OK != pcap->linkinput(&p))
            break;
        ret = device.input(p);
        printf("ret: %d\n", ret);
    }
    if (ret != FREED_PKT) exit(-1);

    // 3. 发包1
    rt.print();
    printf("======\n");
    p = Pktbuf::alloc(Pktbuf::ALLOCATOR, 1380, Pktbuf::IP);
    Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);
    printf("p->hlist.head: %p, h: %p\n", p->hlist.head, h);
    ret = ip->output(p, IP4Addr("192.168.101.28"), 1);
    printf("ret: %d\n", ret);
    if (ret != OK) exit(-1);

    // 4.发包2
    printf("======\n");
    p = Pktbuf::alloc(Pktbuf::ALLOCATOR, 1480, Pktbuf::IP);
    h = p->hlist.locate(p->hlist.head);
    memset(h->payload, 1, h->len);
    printf("p->hlist.head: %p, h: %p\n", p->hlist.head, h);
    ret = ip->output(p, IP4Addr("192.168.101.28"), 1);
    printf("ret: %d\n", ret);
    if (ret != OK) exit(-1);

    // 5. 发包3
    printf("======\n");
    p = Pktbuf::alloc(Pktbuf::ALLOCATOR, 5430, Pktbuf::IP);
    h = p->hlist.locate(p->hlist.head);
    memset(h->payload, 0, h->len);
    printf("p->hlist.head: %p, h: %p\n", p->hlist.head, h);
    ret = ip->output(p, IP4Addr("192.168.101.28"), 1);
    printf("ret: %d\n", ret);
    if (ret != OK) exit(-1);

    pcap->destroy();
    rt.destroy();
    Pktbuf::destroy();
    delete pcap;
    return 0;
}