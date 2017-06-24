/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：UdpHdr.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：UDP定义
 ** 注  意：1.
 ********************************************************************/
#ifndef UDPHDR_H
#define UDPHDR_H

#include <stdint.h>

namespace Proto {

#pragma pack(1)
    typedef struct UdpHdr {
        uint16_t source;
        uint16_t dest;
        uint16_t len;
        uint16_t check;
    } UdpHdr;
#pragma pack()

}

#endif /* UDPHDR_H */
