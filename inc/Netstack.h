/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Netstack.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：主类，入口
 ** 注  意：1.
 ********************************************************************/
#ifndef NETSTACK_H
#define NETSTACK_H

#include "IP.h"
#include "List.h"
#include "EtherIf.h"
#include "ArpCache.h"

class Netstack
{
public:

    static Netstack *instance() { static Netstack stack; return &stack; }

public:

    void main();

public:

    // 网卡列表
    List<NetIf> devlist;

    // arp缓存
    ArpCache arp_cache;

    // ip层处理
    IP ip;
};

#endif /* NETSTACK_H */
