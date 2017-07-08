/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：PcapDriver.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：pcap驱动
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <climits>
#include <cstring>
#include <cstdlib>

#include "Log.h"
#include "PcapDriver.h"

RetType PcapDriver::init(bool isoffline, const char *name,
                         const char *filter)
{
    this->isoffline = isoffline;
    if (isoffline) {
        pcap = pcap_open_offline(name, errbuf);
        if (!pcap) {
            err("pcap error: (%s)\n", errbuf);
            return INTERNL_ERROR;
        }
        return OK;
    }
    pcap = pcap_open_live(name, 65535, 1, INT_MAX, errbuf);
    if (!pcap) {
        err("pcap error: (%s)\n", errbuf);
        return INTERNL_ERROR;
    }
    pcap_compile(pcap, &fp, "inbound", 1, 0);
    pcap_setfilter(pcap, &fp);
    return OK;
}

RetType PcapDriver::init(const char *name)
{
    pcap_send = pcap_open_live(name, 65535, 1, INT_MAX, errbuf);
    if (!pcap) {
        err("pcap_send error: (%s)\n", errbuf);
        return INTERNL_ERROR;
    }
    return OK;
}

int PcapDriver::linkoutput(Pktbuf *pkt)
{
    int  bufp = 0;
    // 足够的空间
    char buf[65600];

    if (!pcap_send)
        return INTERNL_ERROR;

    if (pkt->hlist.count == 1) {
        // 快速发包，不复制数据
        Pktbuf::hunk *h = pkt->hlist.locate(pkt->hlist.head);
        if (-1 == pcap_inject(pcap_send, h->payload, pkt->total_len)) {
            err("pcap inject1 error: (%s)\n", pcap_geterr(pcap_send));
            Pktbuf::free(pkt);
            return INTERNL_ERROR;
        }
        Pktbuf::free(pkt);
        return pkt->total_len;
    }

    List_foreach(pkt->hlist.head, temp) {
        Pktbuf::hunk *h = pkt->hlist.locate(temp);
        memcpy(buf + bufp, h->payload, h->len);
        bufp += h->len;
    }
    if (bufp != pkt->total_len) {
        err("pkt.hlist.len != pkt.total_len!\n");
        return INTERNL_ERROR;
    }
    if (-1 == pcap_inject(pcap_send, buf, bufp)) {
        err("pcap inject2 error: (%s)\n", pcap_geterr(pcap_send));
        Pktbuf::free(pkt);
        return INTERNL_ERROR;
    }
    Pktbuf::free(pkt);
    return pkt->total_len;
}

RetType PcapDriver::linkinput(Pktbuf **ppkt)
{
    int ret;
    Pktbuf *p;
    const u_char *data;
    Pktbuf::AllocType type;
    struct pcap_pkthdr *header;

    ret = pcap_next_ex(pcap, &header, &data);
    switch (ret) {
    case 1:
        type = (header->caplen > 1514) ? Pktbuf::ALLOCATOR:Pktbuf::FIXEDPOOL;
        p = Pktbuf::alloc(type, header->caplen, Pktbuf::NONE);
        if (!p)
            return MEM_FAIL;
        memcpy(p->hlist.locate(p->hlist.head)->payload,
               data,
               header->caplen);
        *ppkt = p;
        return OK;
    case 0:
        // openlive timeout
        return NO_PKT;
    case -1:
        err("pcap_next_ex: %s\n", pcap_geterr(pcap));
        return INTERNL_ERROR;
    case -2:
        // open offline no more pkt
        return NO_PKT;
    default:
        return NO_PKT;
    }
}

void PcapDriver::destroy()
{
    pcap_close(pcap);
}