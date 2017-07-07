/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：IP.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：IP层处理流程
 ** 注  意：1.
 ********************************************************************/
#ifndef IP_H
#define IP_H

#include "Pktbuf.h"
#include "FragTable.h"
#include "RouteTable.h"
#include "NetIf.h"

class IP
{
public:

    static IP *instance() { static IP ip; return &ip; }

    RetType init(RouteTable *rt, uint8_t mod = 6, uint64_t timeout = 100);

    RetType input(Pktbuf *p);

    RetType output(Pktbuf *p, uint32_t dst_ip);

    RetType output(Pktbuf *p, uint32_t dst_ip, NetIf *device);

    RetType forward(Pktbuf *p);

    void destroy();

private:

    FragTable fragtlb;

    RouteTable *rtlb;
};

#endif /* IP_H */
