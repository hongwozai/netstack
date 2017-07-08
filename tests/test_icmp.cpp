/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_icmp.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-08
 **
 ** 描  述：测试icmp层
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "ICMP.h"
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
    pcap->init(true, "../../tests/icmp_test.pcap");
    // if (OK != pcap->init(false, "eth2", "")) {
    //     exit(-1);
    // }
    pcap->init("eth2");
    pcap->setDev(&device);
    device.NetIf::init("pcap0", Net::IP4Addr("192.168.100.29"), 1500, pcap);
    device.init(Net::MACAddr("00:23:24:44:71:91", mac), &cache);

    // 缓存
    // cache.add(IP4Addr("192.168.101.28"), &device,
    //           ArpCache::STABLE, Net::MACAddr("78:24:af:86:8c:7d", mac1));
    // 路由
    rt.add(IP4Addr("192.168.0.0"), IP4Addr("255.255.0.0"), 0, &device);
    ret = ip->init(&rt);
    if (ret != OK) exit(-1);

    // 小包
    for (int i = 0;; i++) {
        if (FINISHED_PKT == (ret = pcap->linkinput())) {
            break;
        }
        if (ret == FREED_PKT && i != 33)
            exit(-1);
    }
    if (ret != FINISHED_PKT) exit(-1);
    pcap->destroy();

    rt.destroy();
    Pktbuf::destroy();
    delete pcap;
    return 0;
}
