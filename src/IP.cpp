/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：IP.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-07
 **
 ** 描  述：ip层处理流程
 ** 注  意：1. NOTE: 不支持IP选项，遇到便跳过
 ********************************************************************/
#include <cstdio>
#include "IP.h"

#include "Netutils.h"
#include "Protocol/Proto.h"

using namespace Net;
using namespace Proto;

RetType IP::init(RouteTable *rt, uint8_t mod, uint64_t timeout)
{
    RetType ret;
    if (OK != (ret = fragtlb.init(mod, timeout))) {
        return ret;
    }
    rtlb = rt;
    return OK;
}

RetType IP::input(Pktbuf *p)
{
    int hlen;
    RetType ret;
    bool isdefrag = false;
    Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);
    IPHdr *ip = (IPHdr*)h->payload;

    // 安检
    if (h->len < sizeof(IPHdr)             ||
        ip->ip_v != 0x4                    ||
        ntoh16(ip->ip_len) < sizeof(IPHdr) ||
        h->len < (uint32_t)(ip->ip_hl << 2)) {
        return MALFORMATION_PKT;
    }
    hlen = ip->ip_hl << 2;

    // 校验
    if (Checksum(ip, hlen) != 0) {
        return CHECKSUM_ERROR;
    }

    // 判断是否为自己的数据包
    if (ip->ip_dst != p->device->ip) {
        return NOTME_PKT;
    }

    // 判断是否需要重组
    if ((ntoh16(ip->ip_off) & IP_MF) != 0     ||
        (ntoh16(ip->ip_off) & IP_OFFMASK) != 0) {

        ret = fragtlb.add(ip->ip_dst, ip->ip_src, ip->ip_id, p);
        if (ret != OK_WITH_DEFRAG)
            return ret;
        // 重组之后继续向下走
        isdefrag = true;
    }

    // 协议分发（重组后p已经是真正的数据包了,非当前数据包）
    switch(ip->ip_p) {
    case IPPROTO_UDP:
        break;
    case IPPROTO_TCP:
        break;
    case IPPROTO_ICMP:
        break;
    default:
        // 其余协议不支持
        return NOT_SUPPORT_PKT;
    }
    if (isdefrag)
        return FREED_PKT;
    return OK;
}

RetType IP::output(Pktbuf *p, uint32_t dst_ip)
{
    // 分片流程
    return OK;
}

RetType IP::output(Pktbuf *p, uint32_t dst_ip, NetIf *device)
{
    return device->output(p, dst_ip);
}

RetType IP::forward(Pktbuf *p)
{
    // 1. 查路由表
    // 2. 转发
    return OK;
}