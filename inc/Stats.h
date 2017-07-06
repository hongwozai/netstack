/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Stats.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：统计部分
 ** 注  意：1。
 ********************************************************************/
#ifndef STATS_H
#define STATS_H

#include <stdint.h>

#define STATINC(x) ((Stats::x)++)

namespace Stats {

    extern uint64_t total_recv_pkt;
    extern uint64_t total_send_pkt;

    extern uint64_t ip_recv_pkt;
    extern uint64_t ip_send_pkt;

    extern uint64_t tcp_recv_pkt;
    extern uint64_t tcp_send_pkt;

    extern uint64_t udp_recv_pkt;
    extern uint64_t udp_send_pkt;

    extern uint64_t arp_recv_pkt;
    extern uint64_t arp_send_pkt;

    extern uint64_t icmp_recv_pkt;
    extern uint64_t icmp_send_pkt;
}

#endif /* STATS_H */
