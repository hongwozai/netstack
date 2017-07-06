/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：TcpHdr.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：Tcp头部等定义
 ** 注  意：1.
 ********************************************************************/
#ifndef TCPHDR_H
#define TCPHDR_H

#include <stdint.h>

namespace Proto {

#pragma pack(1)
    typedef struct TcpHdr {
        uint16_t source;
        uint16_t dest;
        uint32_t seq;
        uint32_t ack_seq;
#ifdef LITTLE_ENDIAN
        uint16_t res1:4;
        uint16_t doff:4;
        uint16_t fin:1;
        uint16_t syn:1;
        uint16_t rst:1;
        uint16_t psh:1;
        uint16_t ack:1;
        uint16_t urg:1;
        uint16_t res2:2;
#else /* BIG_ENDIAN */
        uint16_t doff:4;
        uint16_t res1:4;
        uint16_t res2:2;
        uint16_t urg:1;
        uint16_t ack:1;
        uint16_t psh:1;
        uint16_t rst:1;
        uint16_t syn:1;
        uint16_t fin:1;
#endif
        uint16_t window;
        uint16_t check;
        uint16_t urg_ptr;
    } TcpHdr;

    typedef struct PsuedoHdr {
        uint32_t saddr;
        uint32_t daddr;
        uint8_t  zero;
        uint8_t  protocol;
        uint16_t len;
    } PsuedoHdr;
#pragma pack()
}

#endif /* TCPHDR_H */
