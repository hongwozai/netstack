/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：FragTable.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-06
 **
 ** 描  述：ip分片表
 ** 注  意：1.
 ********************************************************************/
#ifndef FRAGTABLE_H
#define FRAGTABLE_H

#include <stdint.h>
#include "List.h"
#include "HList.h"
#include "Pktbuf.h"
#include "RetType.h"

class FragTable
{
public:

    struct Node {
        HashLink link;
        // 索引
        uint32_t dst_ip;
        uint32_t src_ip;
        uint16_t ipid;
        bool     havelast;
        // 分片列表
        List<Pktbuf> plist;
        // 时间链
        ListLink timelink;
        uint64_t expires;
    };

public:

    // TODO: timeout超时单位为s
    RetType init(uint8_t mod = 6, uint64_t timeout = 100);

    // 添加新的分片
    // @param p 即是传出参数，也是传入参数；当返回值为OK时，p为重组的包
    // @return 返回USE_PKT，数据包还没有重组完成;
    //         返回OK_WITH_DEFRAG 数据包重组完成
    //         返回OK，遇到可能错误的数据包，不进入重组的队列，该包仍然需要丢弃
    RetType add(uint32_t dst_ip, uint32_t src_ip, uint16_t ipid, Pktbuf *&p);

    // 超时接口
    void update();

    // 销毁
    void destroy();

    // 调试接口
    void print();

    // 获得条目数，用于调试
    uint32_t getCount() { return count; }

private:

    // 删除节点
    bool del(Node *node);

    // 判断分组是否到齐，到齐则重组分片
    Pktbuf *defrag(Node *node);

private:

    inline uint32_t hash(uint32_t dst_ip, uint32_t src_ip, uint16_t ipid);

private:

    // 桶
    HList     *buckets;

    // 时间链
    List<Node> timechain;

    // 桶大小-1
    uint32_t   mask;

    // 记录的超时时间，微秒(或cycle)
    uint64_t   timeout;

    // 节点的内存池
    MPool<Node> npool;

    // 有几个表项
    uint32_t    count;
};

inline uint32_t FragTable::hash(uint32_t dst_ip, uint32_t src_ip, uint16_t ipid)
{
    return (dst_ip ^ src_ip ^ ipid) & mask;
}

#endif /* FRAGTABLE_H */
