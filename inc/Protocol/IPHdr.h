/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：IPHdr.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：ip层协议头定义以类型宏定义
 ** 注  意：1
 ********************************************************************/
#ifndef IPHDR_H
#define IPHDR_H

#include <stdint.h>

namespace Proto {

    enum {
        IP_RF =  0x8000,			/* reserved fragment flag */
        IP_DF =  0x4000,			/* dont fragment flag */
        IP_MF =  0x2000,			/* more fragments flag */
        IP_OFFMASK = 0x1fff,		/* mask for fragmenting bits */
    };

    enum {
        IPPROTO_IP = 0,	   /* Dummy protocol for TCP.  */
        IPPROTO_HOPOPTS = 0,   /* IPv6 Hop-by-Hop options.  */
        IPPROTO_ICMP = 1,	   /* Internet Control Message Protocol.  */
        IPPROTO_IGMP = 2,	   /* Internet Group Management Protocol. */
        IPPROTO_IPIP = 4,	   /* IPIP tunnels (older KA9Q tunnels use 94).  */
        IPPROTO_TCP = 6,	   /* Transmission Control Protocol.  */
        IPPROTO_EGP = 8,	   /* Exterior Gateway Protocol.  */
        IPPROTO_PUP = 12,	   /* PUP protocol.  */
        IPPROTO_UDP = 17,	   /* User Datagram Protocol.  */
        IPPROTO_IDP = 22,	   /* XNS IDP protocol.  */
        IPPROTO_TP = 29,	   /* SO Transport Protocol Class 4.  */
        IPPROTO_DCCP = 33,	   /* Datagram Congestion Control Protocol.  */
        IPPROTO_IPV6 = 41,     /* IPv6 header.  */
        IPPROTO_ROUTING = 43,  /* IPv6 routing header.  */
        IPPROTO_FRAGMENT = 44, /* IPv6 fragmentation header.  */
        IPPROTO_RSVP = 46,	   /* Reservation Protocol.  */
        IPPROTO_GRE = 47,	   /* General Routing Encapsulation.  */
        IPPROTO_ESP = 50,      /* encapsulating security payload.  */
        IPPROTO_AH = 51,       /* authentication header.  */
        IPPROTO_ICMPV6 = 58,   /* ICMPv6.  */
        IPPROTO_NONE = 59,     /* IPv6 no next header.  */
        IPPROTO_DSTOPTS = 60,  /* IPv6 destination options.  */
        IPPROTO_MTP = 92,	   /* Multicast Transport Protocol.  */
        IPPROTO_ENCAP = 98,	   /* Encapsulation Header.  */
        IPPROTO_PIM = 103,	   /* Protocol Independent Multicast.  */
        IPPROTO_COMP = 108,	   /* Compression Header Protocol.  */
        IPPROTO_SCTP = 132,	   /* Stream Control Transmission Protocol.  */
        IPPROTO_UDPLITE = 136, /* UDP-Lite protocol.  */
        IPPROTO_RAW = 255,	   /* Raw IP packets.  */
        IPPROTO_MAX
    };

#pragma pack(1)
    typedef struct IPHdr {
#ifdef BIG_ENDIAN
        uint8_t ip_hl:4;		/* header length */
        uint8_t ip_v:4;		/* version */
#else /* LITTLE_ENDIAN */
        uint8_t ip_v:4;		/* version */
        uint8_t ip_hl:4;		/* header length */
#endif
        uint8_t  ip_tos;			/* type of service */
        uint16_t ip_len;			/* total length */
        uint16_t ip_id;			/* identification */
        uint16_t ip_off;			/* fragment offset field */
        uint8_t  ip_ttl;			/* time to live */
        uint8_t  ip_p;			/* protocol */
        uint16_t ip_sum;			/* checksum */
        uint32_t ip_src, ip_dst;	/* source and dest address */
    } IPHdr;
#pragma pack()

}

#endif /* IPHDR_H */
