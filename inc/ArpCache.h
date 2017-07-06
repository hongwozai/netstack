/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：ArpCache.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-25
 **
 ** 描  述：arp缓存
 ** 注  意：1. TODO: 对表项加锁，因为该表可能会有多个以太网卡操作
 ********************************************************************/
#ifndef ARPCACHE_H
#define ARPCACHE_H

#include <stdint.h>
#include "List.h"
#include "HList.h"
#include "NetIf.h"

class ArpCache
{
public:

    enum NodeType {
        // 发送完请求之后的状态
        INCOMPLETE,
        // 进入stable，需要判断时间，时间到了则回到incomplete状态
        STABLE,
        // 静态表项，永不消除
        STATIC,
    };

public:

    struct Node {
        HashLink link;
        // 节点类型
        NodeType type;
        // ip -> mac
        uint32_t ip;
        uint8_t  mac[6];
        NetIf   *device;
        // 数据包链表
        List<Pktbuf> plist;
        // 时间链链接点
        ListLink timelink;
        // 超时时间
        uint64_t expires;
    };

public:

    // 初始化
    // @param timeout 单位s, 默认100s
    // TODO: INCOMPLETE暂时与STABLE的超时时间一致，默认均为100s
    RetType init(uint8_t mod = 4, uint64_t timeout = 100);

    /**
     * add
     * 添加节点，后两个参数根据type而定
     * INCOMPLETE不用填写mac,STATIC与STABLE需要
     */
    Node* add(uint32_t ip, NetIf *device,
              NodeType type = INCOMPLETE, uint8_t mac[6] = 0);


    // 查找对应节点
    Node *find(uint32_t ip);

    // 超时处理
    void update();

    // 删除节点（用于用户操作，自身不调用）
    bool del(uint32_t ip);

    // 打印缓存表,调试使用
    void print();

    // 表项数目
    uint32_t getCount() { return count; }

    // 销毁arp缓存表
    void destroy();

private:

    // 时间链
    List<Node> timechain;

    // 桶
    HList     *buckets;

    // 桶大小-1
    uint32_t   mask;

    // 记录的超时时间，微秒(或cycle)
    uint64_t   timeout;

    // 节点的内存池
    MPool<Node> npool;

    // 有几个表项
    uint32_t    count;
};

#endif /* ARPCACHE_H */
