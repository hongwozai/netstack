/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：test_arpcache.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：arpcache
 ** 注  意：1.
 ********************************************************************/
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ArpCache.h"

int main(int argc, char *argv[])
{
    ArpCache cache;
    bool ret;

    // 1.
    cache.init(4, 1);

    // 2.
    cache.add(0, 0);
    cache.print();

    ret = cache.del(0);
    if (ret != true) exit(-1);

    ret = cache.del(0);
    if (ret != false) exit(-1);

    // 3.
    cache.add(1, 0);
    ArpCache::Node *n = cache.find(1);
    if (!n) exit(-1);

    // 4.
    for (int i = 0; i < 3000; i++) {
        cache.add(i, 0);
    }
    for (int i = 0; i < 3000; i++) {
        cache.del(i);
    }

    // 5.
    if (cache.getCount() != 1) exit(-1);
    Errno en = cache.add(1, 0);
    if (en != OK) exit(-1);
    if (cache.getCount() != 2) exit(-1);
    // sleep(2);
    // cache.update();
    // if (cache.getCount() != 0) exit(-1);

    cache.destroy();
    return 0;
}
