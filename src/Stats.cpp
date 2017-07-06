/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Stat.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-28
 **
 ** 描  述：统计变量定义
 ** 注  意：
 ********************************************************************/
#include "Stats.h"

uint64_t Stats::total_recv_pkt;
uint64_t Stats::total_send_pkt;

uint64_t Stats::ip_recv_pkt;
uint64_t Stats::ip_send_pkt;

uint64_t Stats::tcp_recv_pkt;
uint64_t Stats::tcp_send_pkt;

uint64_t Stats::udp_recv_pkt;
uint64_t Stats::udp_send_pkt;

uint64_t Stats::arp_recv_pkt;
uint64_t Stats::arp_send_pkt;

uint64_t Stats::icmp_recv_pkt;
uint64_t Stats::icmp_send_pkt;
