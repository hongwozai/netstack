/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Pktbuf.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：数据包结构，协议栈通用的结构
 ** 注  意：1.
 ********************************************************************/
#ifndef PKTBUF_H
#define PKTBUF_H

#include <stdint.h>

#include "Errno.h"
#include "List.h"
#include "MPool.h"

// 避免双引用的问题
class NetIf;

class Pktbuf
{
public:

    // 内存分配类型
    enum AllocType {
        ALLOCATOR,
        FIXEDPOOL,
        STATIC,
    };

    // 空间预留的类型
    enum ReserveType {
        TCP,
        UDP,
        IP,
        ETHER,
        NONE,
    };

public:

    struct hunk {
        ListLink  link;
        // 分配类型，释放时使用
        AllocType type;
        // 指向真正数据部分（可能有预留空间）
        char     *payload;
        // 数据部分长度（非真正分配长度）
        uint32_t  len;
        // 数据部分指针
        char      data[1];
    };

public:

    // 回退保留的一定字节数，若无空间则申请数据部分并添加
    Errno header(int size);

    // 将pktbuf连接到当前pktbuf的数据部分
    Errno cat(Pktbuf *);

    // 将pktbuf链接到当前pktbuf链表的下一个
    Errno addNext(Pktbuf *);

public:

    /**
     * alloc
     * @param type 数据包分配的类型
     * @param len  数据包应分配的总长度
     */
    static Pktbuf *alloc(AllocType type, uint32_t len, ReserveType layer);

    // 释放控制部分与数据部分
    static void free(Pktbuf *);

private:

    // 数据包链表
    ListLink link;

    // 数据部分总长度
    uint32_t total_len;

    // vlan相关
    bool     isvlan;
    uint16_t vlanid;

    // 数据包内容链表
    List<hunk> hlist;

    // 从哪个网口收取的数据包
    NetIf *device;

private:

    // 控制部分使用的内存池
    static MPool<Pktbuf> ppool;

    // 数据部分使用的内存池
    static MPool<hunk>   hpool;

};

#endif /* PKTBUF_H */
