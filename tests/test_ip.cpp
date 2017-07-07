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
    uint8_t mac[6];
    RetType ret;
    IP *ip = IP::instance();
    RouteTable rt;
    Pktbuf *p;

    Pktbuf::init(1024, 1024, 1514);
    cache.init();
    pcap->init(true, "../../tests/frag.pcap");
    pcap->init("eth2");
    device.NetIf::init("pcap0", Net::IP4Addr("192.168.100.29"), 1500, pcap);
    device.init(Net::MACAddr("00:23:24:44:71:91", mac), &cache);

    rt.init();
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
    pcap->destroy();
    rt.destroy();

    Pktbuf::destroy();
    delete pcap;
    return 0;
}