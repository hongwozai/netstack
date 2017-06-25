/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：PcapDriver.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：pcap驱动，可以读取文件，也可以监听端口
 ** 注  意：1.
 ********************************************************************/
#ifndef PCAPDRIVER_H
#define PCAPDRIVER_H

#include <pcap.h>
#include "Driver.h"

class PcapDriver : public Driver
{
public:

    // 驱动初始化
    Errno init(bool isoffline, const char *name,
               const char *filter = "inbound");

    // 底层的输出
    int   linkoutput(Pktbuf *);

    // 底层的输入
    Errno linkinput(Pktbuf **);

    // 获取驱动信息
    const char *info() { return pcap_lib_version(); }

    // 驱动资源销毁
    void destroy();

private:

    bool    isoffline;

    pcap_t *pcap;

    bpf_program fp;

    char    errbuf[PCAP_ERRBUF_SIZE];
};

#endif /* PCAPDRIVER_H */
