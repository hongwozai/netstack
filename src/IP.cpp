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
#include <ctime>
#include <cstdio>
#include <cassert>
#include <cstdlib>

#include "IP.h"
#include "ICMP.h"
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
    srand(time(0));
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
        p->header(sizeof(IPHdr));
        break;
    case IPPROTO_TCP:
        p->header(sizeof(IPHdr));
        break;
    case IPPROTO_ICMP:
        ret = ICMP::instance()->input(p);
        break;
    default:
        // 其余协议不支持
        return NOT_SUPPORT_PKT;
    }
    // TODO: 返回值问题，如果上层发生错误，同时又是分片数据包，
    //                 那么给下层的返回值就一定是FREED_PKT，否则就会多次释放
    if (ret != USE_PKT && ret != FREED_PKT) {
        if (isdefrag) {
            Pktbuf::free(p);
            return FREED_PKT;
        }
        return ret;
    }
    return ret;
}

RetType IP::output(Pktbuf *p, uint32_t dst_ip, uint8_t pro)
{
    RouteTable::Node *n;

    // 检查数据包
    if (p->total_len > 65535 - sizeof(IPHdr)) {
        return TOO_BIG;
    }
    // 查路由表
    if (NULL == (n = rtlb->nextHop(dst_ip))) {
        return NO_ROUTE;
    }
    if (!n->device)
        return NO_ROUTE;

    // 原始套接字
    if (pro == 0xff) {
        return n->device->output(p, n->gateway ? n->gateway : dst_ip);
    }
    // 输出
    return output(p, dst_ip, pro, n->device, n->gateway ? n->gateway:dst_ip);
}

RetType IP::output(Pktbuf *p, uint32_t dst_ip, uint8_t pro,
                   NetIf *device, uint32_t nexthop)
{
    IPHdr *ip;
    Pktbuf::hunk *h;
    Pktbuf *frag = NULL;
    // 当前分片发送长度
    uint32_t  sendlen = 0;
    // 当前发送的总长度（偏移的正常数值，一定为8的倍数）
    uint32_t  sendtotal = 0;
    uint16_t  ipid = rand();

    /**
     * 分片流程
     * 首先计算在该设备mtu下，最大可以发送多少字节
     * 然后构造新的数据包，并拷贝数据
     * 最后构造ip数据包并发送
     */
    if (p->total_len > device->mtu - sizeof(IPHdr)) {
        sendlen = ((device->mtu - sizeof(IPHdr)) >> 3) << 3;
        // 循环构造数据包
        while ((frag = p->split(sendlen))) {
            if (OK != frag->header(-(int)sizeof(IPHdr)))
                return MEM_FAIL;

            h = frag->hlist.locate(frag->hlist.head);
            ip = (IPHdr*)h->payload;
            ip->ip_v   = 0x4;
            ip->ip_hl  = 0x5;
            ip->ip_tos = 0x0;
            ip->ip_len = hton16(frag->total_len);
            ip->ip_id  = ipid;
            ip->ip_off = 0;
            ip->ip_off |= IP_MF;
            ip->ip_off |= sendtotal >> 3;
            ip->ip_off = hton16(ip->ip_off);
            ip->ip_ttl = 64;
            ip->ip_p   = pro;
            ip->ip_sum = 0x0;
            ip->ip_src = device->ip;
            ip->ip_dst = dst_ip;
            ip->ip_sum = Checksum(ip, sizeof(IPHdr));
            // 发送
            device->output(frag, dst_ip);
            sendtotal += sendlen;
        }
    }
    // 无需分片直接发送流程（或是最后一个分片发送的流程）
    if (OK != p->header(-(int)sizeof(IPHdr))) {
        return MEM_FAIL;
    }
    h = p->hlist.locate(p->hlist.head);
    ip = (IPHdr*)h->payload;
    ip->ip_v   = 0x4;
    ip->ip_hl  = 0x5;
    ip->ip_tos = 0x0;
    ip->ip_len = hton16(p->total_len);
    ip->ip_id  = (sendlen != 0) ? ipid : 0;
    if (sendlen != 0) {
        ip->ip_off = sendtotal >> 3;
        ip->ip_off = hton16(ip->ip_off);
    } else {
        ip->ip_off = 0;
    }
    ip->ip_ttl = 64;
    ip->ip_p   = pro;
    ip->ip_sum = 0x0;
    ip->ip_src = device->ip;
    ip->ip_dst = dst_ip;
    ip->ip_sum = Checksum(ip, sizeof(IPHdr));
    return device->output(p, nexthop);
}

RetType IP::forward(Pktbuf *p)
{
    // 1. 获得目的ip
    // 2. 查路由表
    // 3. 转发
    return OK;
}