/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：ICMP.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-08
 **
 ** 描  述：icmp层处理逻辑
 ** 注  意：1. NOTE: icmp层的输入仅处理回显
 ********************************************************************/
#ifndef ICMP_H
#define ICMP_H

#include "Pktbuf.h"
#include "RetType.h"

class ICMP
{
public:

    static ICMP *instance() { static ICMP icmp; return &icmp; }

    RetType input(Pktbuf *p);

    RetType destnoreach();

    // 发送端口不可达信息
    // @param p 此处使用pktbuf，在非出错的情况下返回FREED_PKT
    RetType portnoreach(Pktbuf *p);

private:

    ICMP() {}

    ~ICMP() {}
};

#endif /* ICMP_H */
