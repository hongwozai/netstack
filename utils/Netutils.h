/********************************************************************
 ** Copyright(c) 2016, luzeya
 ** All rights reserved
 **
 ** 文件名：Netutils.h
 ** 创建人：路泽亚
 ** 创建日期： 2016-12-04
 **
 ** 描  述：网络基础,校验、ip转换、大小端转换、mac转换
 ** 注  意：1.
 ********************************************************************/
#ifndef NET_NETUTILS_H
#define NET_NETUTILS_H

#include <stdint.h>

namespace Net {

    // 翻转字节序
    void     Evert(void *data, int len);

    // 网络校验码（注意校验码的结果已经求反(Fold不用)，传第三个参数时需要再求反）
    uint16_t Checksum(void *data, int len, uint16_t chksum = 0);

    // 带伪首部的校验和
    uint16_t ChecksumPseudo(void *data,    int len,
                            uint32_t src,  uint32_t dst,
                            uint8_t proto, uint16_t total,
                            uint16_t chksum = 0);

    // 伪首部的校验和（求反）,所有参数都要求大端序
    uint16_t ChecksumPseudo(uint32_t src,  uint32_t dst,
                            uint8_t proto, uint16_t total,
                            uint16_t chksum);

    // 折叠，用于非线性缓冲区校验，没有求反
    uint16_t ChecksumFold(void *data, int len, uint16_t chksum = 0);

    // 点分十进制转换为ipv4
    uint32_t IP4Addr(const char *str);

    // ipv4转换为点分十进制，ip为大端序，且buf至少大于16，用\0结尾
    const char*    IP4Addr(uint32_t ip, const char *buf);

    // 同上，并非线程安全
    const char*    IP4Addr(uint32_t ip);

    // 字符串转mac
    uint8_t* MACAddr(const char *s, uint8_t *mac);

    // mac转字符串，buf至少大于18，用\0结尾
    const char*    MACAddr(uint8_t *mac, const char *buf);

    // 同上，并非线程安全
    const char*    MACAddr(uint8_t *mac);

    // 直接输出十六进制字符
    void     Hexdump(void *data, int len);

    // 按格式输出到buffer中，buffer长度应是len的4倍以上(\0结尾)，返回buffer
    char*    Hexdump(void *data, int len, char *buffer);
}

#endif /* NET_NETUTILS_H */
