/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：RouteTable.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-06
 **
 ** 描  述：路由表
 ** 注  意：1. 不支持任何路由算法，默认全部为静态条目，不动态添加删除
 ********************************************************************/
#ifndef ROUTETABLE_H
#define ROUTETABLE_H

#include <stdint.h>
#include "NetIf.h"
#include "RetType.h"

class RouteTable
{
public:

    // key为network,netmask, value为gateway,device
    struct Node {
        ListLink link;
        uint32_t network;
        uint32_t netmask;
        uint32_t gateway;
        NetIf   *device;
    };

public:

    // 初始化
    RetType init(int num = 32);

    // 销毁
    void    destroy();

    // 添加路由表项
    RetType add(uint32_t network, uint32_t netmask, uint32_t gateway,
                NetIf   *device);

    // 删除路由表项
    RetType del(uint32_t network, uint32_t netmask);

    // 根据ip找一跳
    Node   *nextHop(uint32_t ip);

    // 打印，调试使用
    void    print();

    // 获得总条目数
    int     getCount();

private:

    inline int count(uint32_t netmask);

private:

    // 0-32 0为默认网关
    List<Node> array[33];

    MPool<Node> npool;
};

inline int RouteTable::count(uint32_t netmask)
{
    int s = 0;
    while (netmask) {
        s += netmask & 0x1;
        netmask >>= 1;
    }
    return s;
}

#endif /* ROUTETABLE_H */
