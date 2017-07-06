/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：RouteTable.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-06
 **
 ** 描  述：路由表
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstddef>
#include <cstring>
#include "Netutils.h"
#include "RouteTable.h"

RetType RouteTable::init(int num)
{
    memset(this, 0, sizeof(*this));
    for (int i = 0; i < 33; i++) {
        array[i].config(offsetof(Node, link));
    }
    if (0 != npool.init(num)) {
        return MEM_FAIL;
    }
    return OK;
}

RetType RouteTable::add(uint32_t network, uint32_t netmask, uint32_t gateway,
                        NetIf *device)
{
    Node *n;
    int index;

    // 如果有相同项则覆盖，无则追加
    index = count(netmask);
    List_foreach(array[index].head, temp) {
        n = array[index].locate(temp);
        if ((n->network & netmask) == (network & netmask)) {
            n->gateway = gateway;
            n->device  = device;
            return OK;
        }
    }
    // 新建
    n = npool.attach();
    if (!n) {
        return MEM_FAIL;
    }
    n->network = network;
    n->netmask = netmask;
    n->gateway = gateway;
    n->device  = device;

    array[index].append(n);
    return OK;
}

RetType RouteTable::del(uint32_t network, uint32_t netmask)
{
    Node *n;
    int index;

    index = count(netmask);
    List_foreach(array[index].head, temp) {
        n = array[index].locate(temp);
        if ((n->network & netmask) == (network & netmask)) {
            // 释放节点，并立即返回
            array[index].detach(temp);
            npool.detach(n);
            return OK;
        }
    }
    return OK;
}

RouteTable::Node* RouteTable::nextHop(uint32_t ip)
{
    for (int i = 32; i >= 0; --i) {
        List_foreach(array[i].head, temp) {
            Node *n = array[i].locate(temp);
            if ((ip & n->netmask) == (n->network & n->netmask)) {
                // 找到匹配项
                return n;
            }
        }
    }
    // 此处是连默认路由都找不到，默认路由是默认掩码为0的表项
    return NULL;
}

void RouteTable::destroy()
{
    ListLink *temp, *next;
    for (int i = 0; i < 33; ++i) {
        List_safe_foreach(array[i].head, temp, next) {
            Node *n = array[i].locate(temp);
            array[i].detach(temp);
            npool.detach(n);
        }
    }
    npool.destroy();
}

void RouteTable::print()
{
    char network[16], gateway[16], netmask[16];
    for (int i = 0; i < 33; i++) {
        List_foreach(array[i].head, temp) {
            Node *n = array[i].locate(temp);
            // 打印条目
            printf("ip: %s, netmask: %s, gw: %s, iface: %s\n",
                   Net::IP4Addr(n->network, network),
                   Net::IP4Addr(n->netmask, netmask),
                   Net::IP4Addr(n->gateway, gateway),
                   n->device ? n->device->name : "NULL");
        }
    }
}

int RouteTable::getCount()
{
    int s = 0;
    for (int i = 0; i < 33; ++i) {
        s += array[i].count;
    }
    return s;
}