/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Driver.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：驱动的虚基类
 ** 注  意：
 ********************************************************************/
#ifndef DRIVER_H
#define DRIVER_H

#include "Pktbuf.h"

class Driver
{
public:

    // 底层的输出
    // @return 返回发送的字节数，< 0为失败
    virtual int linkoutput(Pktbuf *) = 0;

    // 底层的输入
    virtual Errno linkinput(Pktbuf **) = 0;

    // 获取驱动信息
    virtual const char *info() = 0;

};

#endif /* DRIVER_H */
