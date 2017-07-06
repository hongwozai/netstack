/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_stat.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-28
 **
 ** 描  述：测试统计是否可以使用
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>

#include "Stats.h"

int main(int argc, char *argv[])
{
    STATINC(total_recv_pkt);
    if (Stats::total_recv_pkt != 1) exit(-1);
    return 0;
}
