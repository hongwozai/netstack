/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：EtherIf.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：以太网网卡
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Stats.h"
#include "Protocol/Proto.h"
#include "EtherIf.h"
#include "Netutils.h"
#include "Netstack.h"

using namespace Net;
using namespace Proto;

RetType EtherIf::init(uint8_t mac[6], ArpCache *cache)
{
    memcpy(this->mac, mac, 6);
    this->cache = cache;
    return OK;
}

RetType EtherIf::output(Pktbuf *pkt, uint32_t dst_ip)
{
    EtherHdr *ether;
    Pktbuf::hunk *h;
    ArpCache::Node *n;
    uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    if (OK != pkt->header( - (int)sizeof(EtherHdr))) {
        return MEM_FAIL;
    }
    h = pkt->hlist.locate(pkt->hlist.head);
    ether = (EtherHdr*)h->payload;
    memcpy(ether->src, mac, 6);

    // TODO: 暂时发送的都是IPV4
    ether->type = hton16(ETH_P_IP);

    // 查询目的mac
    if (!(n = cache->find(dst_ip))) {
        n = cache->add(dst_ip, this);
        if (!n) {
            return MEM_FAIL;
        }
        // 挂链
        n->plist.append(pkt);
        // 发送
        if (SEND_FAIL == arp_output(true, broadcast, dst_ip)) {
            cache->del(dst_ip);
            return SEND_FAIL;
        }
        return USE_PKT;
    } else {
        switch (n->type) {
        case ArpCache::INCOMPLETE:
            // 挂链
            n->plist.append(pkt);
            return USE_PKT;
        case ArpCache::STATIC:
        case ArpCache::STABLE:
            // 直接发送
            memcpy(ether->dst, n->mac, 6);
            driver->linkoutput(pkt);
            break;
        }
    }
    return OK;
}

RetType EtherIf::input(Pktbuf *pkt)
{
    VlanHdr  *vlan;
    EtherHdr *ether;
    RetType   ret;
    Pktbuf::hunk *h = pkt->hlist.locate(pkt->hlist.head);
    uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // 安检代码
    if (h->len < sizeof(EtherHdr)) {
        return MALFORMATION_PKT;
    }

    /**
     * 取出以太网头部并根据type值进行处理
     */
    ether = (EtherHdr*)h->payload;

    // NOTE: 检查是否目的mac是否是属于自己的MAC包或是广播包
    if (memcmp(ether->dst, mac, 6) != 0 &&
        memcmp(ether->dst, broadcast, 6) != 0) {
        return NOTME_PKT;
    }

    switch (ntoh16(ether->type)) {
    case ETH_P_8021Q:
        if (h->len < sizeof(EtherHdr) + sizeof(VlanHdr)) {
            return MALFORMATION_PKT;
        }
        vlan = (VlanHdr*)(h->payload + sizeof(EtherHdr));
        pkt->isvlan = true;
        pkt->vlanid = vlan->vlanid;
        pkt->header(sizeof(EtherHdr) + sizeof(VlanHdr));

    case ETH_P_IP:
        // ret = Netstack::instance()->ip.input();
        break;

    case ETH_P_ARP:
        // arp包不会被存储
        pkt->header(sizeof(EtherHdr));
        return arp_input(pkt);

    default:
        // NOTE: 该协议栈当前不处理pppoe,ipv6的包，其余数据包全部丢掉
        ret = OK;
        break;
    }

    if (ret != USE_PKT) {
        Pktbuf::free(pkt);
    }
    return ret;
}

RetType EtherIf::arp_input(Pktbuf *p)
{
    ArpHdr *arp;
    EtherHdr *ether;
    ArpCache::Node *n;
    ListLink *next, *temp;
    Pktbuf::hunk *h = p->hlist.locate(p->hlist.head);

    // 安检
    arp = (ArpHdr*)h->payload;
    if (h->len < sizeof(ArpHdr)            ||
        arp->arp_hrd != hton16(0x1)        ||
        // 仅支持v4
        arp->arp_pro != hton16(ETH_P_IP)   ||
        arp->arp_hln != 0x6                ||
        (arp->arp_op != hton16(0x0001) && arp->arp_op != hton16(0x0002))) {
        return MALFORMATION_PKT;
    }

    switch (ntoh16(arp->arp_op)) {
    case 0x0001:
        /**
         * 请求流程，是则回响应，不是则不做任何处理
         */
        if (arp->arp_dipaddr == ip) {
            // NOTE: 发送失败不做任何处理
            arp_output(false, arp->arp_shwaddr, arp->arp_sipaddr);
        }
        // NOTE: 被动监听arp
        break;

    case 0x0002:
        // 响应
        if (arp->arp_dipaddr != ip) {
            break;
        }
        n = cache->find(arp->arp_sipaddr);
        if (n) {
            switch (n->type) {
            case ArpCache::INCOMPLETE:
                // 修改状态
                n->type = ArpCache::STABLE;
                // 发送链表上的所有数据包
                List_safe_foreach(n->plist.head, temp, next) {
                    Pktbuf *p = n->plist.locate(temp);

                    // 添加目的mac
                    h = p->hlist.locate(p->hlist.head);
                    ether = (EtherHdr*)h->payload;
                    memcpy(ether->dst, arp->arp_shwaddr, 6);

                    // 发送
                    driver->linkoutput(p);
                    // 释放
                    n->plist.detach(temp);
                    Pktbuf::free(p);
                }

            case ArpCache::STABLE:
                memcpy(n->mac, arp->arp_shwaddr, 6);
                break;

            case ArpCache::STATIC:
                // nothing
                break;
            }
        }
        // n为空直接忽略该响应包
        break;

    default:
        // 不可能走
        return MALFORMATION_PKT;
    }

    return OK;
}

RetType EtherIf::arp_output(bool isrequest,
                            uint8_t dst_mac[6], uint32_t dst_ip)
{
    int bytes;
    ArpHdr *arp;
    EtherHdr *ether;
    Pktbuf::hunk *h;
    Pktbuf *p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 60, Pktbuf::NONE);
    if (!p) {
        return MEM_FAIL;
    }

    h = p->hlist.locate(p->hlist.head);
    ether = (EtherHdr*)h->payload;
    arp   = (ArpHdr*)(ether + 1);

    // 构造数据包
    memcpy(ether->dst, dst_mac, 6);
    memcpy(ether->src, mac, 6);
    ether->type = hton16(ETH_P_ARP);
    arp->arp_hrd = hton16(0x1);
    arp->arp_pro = hton16(ETH_P_IP);
    arp->arp_hln = 0x6;
    arp->arp_pln = 0x4;
    arp->arp_op = isrequest ? hton16(0x0001) : hton16(0x0002);
    memcpy(arp->arp_shwaddr, mac, 6);
    memcpy(arp->arp_dhwaddr, dst_mac, 6);
    arp->arp_sipaddr = ip;
    arp->arp_dipaddr = dst_ip;

    // 发送数据包
    if ((bytes = driver->linkoutput(p)) < 0) {
        Pktbuf::free(p);
        return SEND_FAIL;
    }
    Pktbuf::free(p);
    return OK;
}