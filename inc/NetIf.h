/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：NetIf.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：网卡接口(虚类)
 ** 注  意：1.
 ********************************************************************/
#ifndef NETIF_H
#define NETIF_H

#include <stdint.h>
#include "List.h"
#include "RetType.h"
#include "Pktbuf.h"
#include "Driver.h"

class NetIf
{
public:

    // 初始化函数
    inline RetType init(char *name, uint32_t ip, uint16_t mtu, Driver *d);

    // 输出函数，pktbuf的数据部分应该为ip头部
    // @param pktbuf 输入
    virtual RetType output(Pktbuf *, uint32_t ip) = 0;

    // 输入函数
    // @param pktbuf 输出
    virtual RetType input(Pktbuf *) = 0;

    // 开启网卡功能
    virtual void up()   { isup = true; }

    // 关闭网卡功能
    virtual void down() { isup = false; }

public:

    ListLink link;

    // 网卡名称
    char    *name;

    // 是否开启
    bool     isup;

    // 是否插着网线
    // TODO: 此处暂时不用，恒为true
    bool     isrunning;

    // mtu值
    uint16_t mtu;

    // 网卡的ip地址
    // TODO: 暂时仅支持IPv4
    uint32_t ip;

    // 网卡所使用的驱动
    Driver  *driver;

    /**
     * 以下是统计数据
     */
    uint64_t rbyt;
    uint64_t tbyt;
    uint64_t rpkt;
    uint64_t tpkt;
};

inline RetType NetIf::init(char *name, uint32_t ip, uint16_t mtu, Driver *d)
{
    this->name = name;
    this->ip   = ip;
    this->mtu  = mtu;
    this->driver = d;

    rbyt = tbyt = rpkt = tpkt;
    isup = isrunning = true;
    return OK;
}

#endif /* NETIF_H */
