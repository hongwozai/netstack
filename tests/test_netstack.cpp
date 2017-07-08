/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_netstack.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-08
 **
 ** 描  述：
 ** 注  意：
 ********************************************************************/
#include "PcapDriver.h"
#include "Netstack.h"
#include "Netutils.h"

using namespace Net;

int main(int argc, char *argv[])
{
    uint8_t mac[6];
    PcapDriver p;
    Netstack *stack = Netstack::instance();

    if (OK != stack->init()) {
        return 0;
    }
    stack->addEtherIf("pcap0",
                      IP4Addr("192.168.100.30"),
                      MACAddr("00:11:22:33:44:55", mac),
                      &p);
    if (OK != p.init(false, "eth0")) {
        return 0;
    }

    for (;;) {
        if (FINISHED_PKT != p.linkinput())
            break;
    }

    p.destroy();
    stack->destroy();
    return 0;
}