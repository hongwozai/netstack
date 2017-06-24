/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：ICMPHdr.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：ICMP系列头部定义
 ** 注  意：1.
 ********************************************************************/
#ifndef ICMPHDR_H
#define ICMPHDR_H

#include <stdint.h>

namespace Proto {

#pragma pack(1)
    typedef struct ICMPHdr {
        uint8_t  type;
        uint8_t  code;
        uint16_t check;
        uint16_t identifier;
        uint16_t seq;
    } ICMPHdr;
#pragma pack()
}

#endif /* ICMPHDR_H */
