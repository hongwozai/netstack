/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_etherif.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-30
 **
 ** 描  述：
 ** 注  意：
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Netutils.h"
#include "EtherIf.h"
#include "ArpCache.h"
#include "PcapDriver.h"

int main(int argc, char *argv[])
{
    ArpCache cache;
    EtherIf device;
    PcapDriver *pcap = new PcapDriver();
    uint8_t mac[6];
    RetType ret;

    Pktbuf::init(1024, 1024, 1514);
    cache.init();
    pcap->init(true, "../../tests/1arp.pcap");
    pcap->init("eth2");
    device.NetIf::init("pcap0", Net::IP4Addr("192.168.100.30"), 1500, pcap);
    device.init(Net::MACAddr("08:00:27:f5:c1:ca", mac), &cache);

    // 包存下来，并发送arp请求
    Pktbuf *p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 1111, Pktbuf::ETHER);
    Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);
    memset(h->payload, 1, 1111);
    ret = device.output(p, Net::IP4Addr("192.168.100.27"));
    if (ret != USE_PKT) {
        printf("ret: %d\n", ret);
        exit(-1);
    }

    // 检查cache是否存在
    cache.print();
    ArpCache::Node *n = cache.find(Net::IP4Addr("192.168.100.27"));
    if (!n) exit(-1);
    if (n->plist.count != 1) exit(-1);

    // 接受响应
    for (;;) {
        if (OK != pcap->linkinput(&p))
            break;
        ret = device.input(p);
        if (ret != OK && ret != USE_PKT) exit(-1);
    }
    cache.print();
    pcap->destroy();

    // 接受请求
    pcap->init(true, "../../tests/2arp.pcapng");
    pcap->init("eth2");
    device.NetIf::init("pcap1", Net::IP4Addr("192.168.100.29"), 1500, pcap);
    device.init(Net::MACAddr("00:23:24:44:71:91", mac), &cache);
    for (;;) {
        if (OK != pcap->linkinput(&p))
            break;
        ret = device.input(p);
        if (ret != OK) exit(-1);
    }
    return 0;
}
