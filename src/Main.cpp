/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Main.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：程序入口
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include "Netutils.h"

int main(int argc, char *argv[])
{
    printf("hello world!\n");
    Net::Hexdump((char*)"123123", 6);
    return 0;
}
