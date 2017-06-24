/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：EtherHdr.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：ethernet以及arp定义
 ** 注  意：1.
 ********************************************************************/
#ifndef ETHERHDR_H
#define ETHERHDR_H

#include <stdint.h>

namespace Proto {

    enum {
        ETH_P_IP     = 0x0800,
        ETH_P_ARP    = 0x0806,
        ETH_P_RARP   = 0x8035,
        ETH_P_8021Q  = 0x8100,
        ETH_P_IPV6   = 0x8600,
        ETH_P_PAUSE  = 0x8808,
        ETH_P_PPPOED = 0x8863,
        ETH_P_PPPOES = 0x8864,
    };

#pragma pack(1)
    struct EtherHdr {
        uint8_t dst[6];
        uint8_t src[6];
        uint16_t type;
    };

    typedef struct ArpHdr {
        /* 硬件地址类型 */
        uint16_t arp_hrd;
        /* arp承载协议类型 */
        uint16_t arp_pro;
        /* 硬件地址长度，就是6B */
        uint8_t  arp_hln;
        /* 协议地址长度，是ip长度 */
        uint8_t  arp_pln;
        /* arp操作吗，0x0001, 0x0002分别为请求与响应 */
        uint16_t arp_op;

        /* arp body, 长度20B */
        uint8_t  arp_shwaddr[6];
        uint32_t arp_sipaddr;
        uint8_t  arp_dhwaddr[6];
        uint32_t arp_dipaddr;
    } ArpHdr;
#pragma pack()

}

#endif /* ETHERHDR_H */
