/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_mpool.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：1.
 ** 注  意：
 ********************************************************************/
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "MPool.h"
#include "Netutils.h"

int main(int argc, char *argv[])
{
    MPool<int> intpool;
    intpool.init(1024);
    int *a = intpool.attach();

    if (!a)
        exit(-1);

    memset(a, 0, sizeof(*a));

    int *array[1400] = {0};
    for (int i = 0; i < 1400; i++) {
        array[i] = intpool.attach();
    }
    for (int i = 0; i < 1400; i++) {
        intpool.detach(array[i]);
    }
    intpool.destroy();
    return 0;
}
