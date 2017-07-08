/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Netstack.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-08
 **
 ** 描  述：协议栈主体
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include "Netutils.h"
#include "Netstack.h"
#include "Protocol/Proto.h"

using namespace Net;
using namespace Proto;

RetType Netstack::init()
{
    RetType ret;

    // 初始化网卡列表
    devlist.config(offsetof(NetIf, link));

    // 初始化包内存池
    if (OK != (ret = Pktbuf::init(1024, 1024, 1514))) {
        return ret;
    }

    // arp缓存表
    if (OK != (ret = cache.init(5, 100))) {
        return ret;
    }
    // 路由表
    if (OK != (ret = fib.init(32))) {
        return ret;
    }
    // ip层初始化
    if (OK != (ret = IP::instance()->init(&fib))) {
        return ret;
    }
    return OK;
}

void Netstack::destroy()
{
    Pktbuf::destroy();
    cache.destroy();
    fib.destroy();
    while (devlist.count) {
        free(devlist.pop());
    }
}

RetType Netstack::addEtherIf(const char *name,
                             uint32_t    ip,
                             uint8_t     mac[6],
                             Driver     *driver)
{
    EtherIf *iface;
    uint32_t mask = genMask(ip);

    if (!name || !driver) {
        return ARGS_ERROR;
    }
    iface = (EtherIf*)malloc(sizeof(EtherIf));
    if (!iface) {
        return MEM_FAIL;
    }
    // 初始化网卡
    iface->NetIf::init(name, ip, 1500, driver);
    iface->init(mac, &cache);
    driver->setDev(iface);
    // 添加网卡链表
    devlist.append(iface);
    // 添加路由表
    fib.add(ip & mask, mask, 0, iface);

    return OK;
}

void Netstack::printEtherIf()
{
    List_foreach(devlist.head, temp) {
        EtherIf *dev = (EtherIf*)devlist.locate(temp);
        printf("dev: %7s ip: %s mac: %s",
               dev->name,
               IP4Addr(dev->ip),
               MACAddr(dev->mac));
    }
}

uint32_t Netstack::genMask(uint32_t ip)
{
    ip = (ntoh32(ip) & 0xFF000000) >> 24;

    if (ip >= 0x00 && ip <= 0x7F) {
        // 00000000 - 01111111
        return hton32(0xFF000000);
    } else if (ip > 0x7F && ip <= 0xBF) {
        // 10000000 - 10111111
        return hton32(0xFFFF0000);
    } else {
        // other
        return hton32(0xFFFFFF00);
    }
}