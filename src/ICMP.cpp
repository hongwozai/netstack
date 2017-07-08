/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：ICMP.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-08
 **
 ** 描  述：icmp层处理逻辑
 ** 注  意：1.
 ********************************************************************/
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "IP.h"
#include "ICMP.h"
#include "Netutils.h"
#include "Protocol/Proto.h"

using namespace Net;
using namespace Proto;

#define SWAP(a, b) { typeof(a) temp = b; b = a; a = temp; }

RetType ICMP::input(Pktbuf *p)
{
    IPHdr *ip;
    ICMPHdr *icmp;
    Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);

    // 安检
    if (h->len < sizeof(ICMPHdr)) {
        return MALFORMATION_PKT;
    }

    // 只处理回射请求
    ip = (IPHdr*)h->payload;
    icmp = (ICMPHdr*)(ip + 1);
    p->header(sizeof(IPHdr));
    if (icmp->type == 8 && icmp->code == 0) {
        icmp->type = 0;
        icmp->code = 0;
        icmp->check = 0;
        icmp->check = p->checksum();
        IP::instance()->output(p, ip->ip_src, IPPROTO_ICMP);
        return FREED_PKT;
    }
    return OK;
}