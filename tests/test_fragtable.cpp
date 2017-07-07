/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_fragtable.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-07
 **
 ** 描  述：分片表
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "FragTable.h"

int main(int argc, char *argv[])
{
    FragTable ft;

    /**
     * 这里仅简单测试，对分片的测试在ip层中
     */
    ft.init(6, 1);
    ft.print();
    ft.update();
    if (ft.getCount() != 0) exit(-1);
    ft.destroy();
    return 0;
}

