/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：EtherIf.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：以太网网卡
 ** 注  意：1
 ********************************************************************/
#ifndef ETHERIF_H
#define ETHERIF_H

#include <stdint.h>

#include "NetIf.h"
#include "ArpCache.h"

class EtherIf : public NetIf
{
public:

    // 以太网网卡自己的初始化函数
    RetType init(uint8_t mac[6], ArpCache *cache);

    // 输出函数
    RetType output(Pktbuf *, uint32_t ip);

    // 输入函数
    RetType input(Pktbuf *);

public:

    uint8_t mac[6];

    ArpCache *cache;

private:

    /**
     * 将arp整合进来
     */
    RetType arp_input(Pktbuf *);

    RetType arp_output(bool isrequest, uint8_t dst_mac[6], uint32_t dst_ip);

};

#endif /* ETHERIF_H */
