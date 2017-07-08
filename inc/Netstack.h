/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Netstack.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：主类，入口
 ** 注  意：1. TODO: 定制参数传递？
 ********************************************************************/
#ifndef NETSTACK_H
#define NETSTACK_H

#include "IP.h"
#include "List.h"
#include "EtherIf.h"
#include "ArpCache.h"
#include "RouteTable.h"

class Netstack
{
public:

    static Netstack *instance() { static Netstack s; return &s; }

    // 初始化协议栈
    RetType init();

    // 销毁资源
    void    destroy();

    // 添加以太网网络设备
    RetType addEtherIf(const char *name,
                       uint32_t    ip,
                       uint8_t     mac[6],
                       Driver     *driver);

    // 打印网卡列表，用于调试
    void    printEtherIf();

public:

    void main();

private:

    Netstack() {}

    ~Netstack() {}

    // 根据ip生成掩码
    // eg. 第二类的生成255.255.0.0
    uint32_t genMask(uint32_t ip);

public:

    // 网卡列表
    List<NetIf> devlist;

    // arp缓存
    ArpCache cache;

    // 路由表
    RouteTable fib;
};

#endif /* NETSTACK_H */
