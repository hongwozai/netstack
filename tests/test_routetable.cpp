/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_routetable.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-06
 **
 ** 描  述：测试路由表
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include "Netutils.h"
#include "RouteTable.h"

using namespace Net;

int main(void)
{
    RouteTable rt;
    RouteTable::Node *n;

    rt.init();

    // 测试基本功能
    rt.add(IP4Addr("192.168.100.29"), IP4Addr("255.255.0.0"),
           IP4Addr("192.168.103.1"), NULL);
    rt.add(IP4Addr("192.168.100.28"), IP4Addr("255.255.255.0"),
           IP4Addr("192.168.103.1"), NULL);
    n = rt.nextHop(IP4Addr("192.168.100.31"));
    if (!n) exit(-1);
    if (rt.getCount() != 2) exit(-1);

    // 测试默认路由
    rt.print();
    if (n->network != IP4Addr("192.168.100.28")) exit(-1);
    printf("n->ipaddr: %s\n", IP4Addr(n->network));

    n = rt.nextHop(IP4Addr("172.73.74.75"));
    if (n) exit(-1);

    rt.add(0, 0, IP4Addr("192.168.103.1"), 0);
    n = rt.nextHop(IP4Addr("172.73.74.75"));
    if (!n) exit(-1);
    rt.print();

    if (rt.getCount() != 3) exit(-1);

    // 测试add的覆盖功能
    rt.add(0, 0, IP4Addr("192.168.104.1"), 0);
    if (rt.getCount() != 3) exit(-1);

    // 测试删除功能
    rt.del(IP4Addr("192.168.100.28"), IP4Addr("255.255.255.0"));
    if (rt.getCount() != 2) exit(-1);

    rt.print();

    rt.destroy();
    return 0;
}