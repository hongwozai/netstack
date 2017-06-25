/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_pcapdriver.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：测试pcap驱动
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "PcapDriver.h"

int main(int argc, char *argv[])
{
    PcapDriver pd;
    Errno no;
    int ret;

    Pktbuf::init(1024, 1024, 1514);
    // 1.
    no = pd.init(true, "../../tests/1.pcap");
    if (no != OK) exit(-1);

    // 2.
    Pktbuf *p = Pktbuf::alloc(Pktbuf::FIXEDPOOL, 1500, Pktbuf::NONE);
    if (!p) exit(-1);
    exit(0);

    ret = pd.linkoutput(p);
    printf("ret: %d\n", ret);
    if (ret != -1) exit(-1);
    Pktbuf::free(p);

    // 3.
    int i = 0;
    while (1) {
        i++;
        pd.linkinput(&p);
    }
    if (i != 13) exit(-1);

    pd.destroy();
    return 0;
}
