/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Error.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：错误类型定义
 ** 注  意：1.
 ********************************************************************/
#ifndef ERROR_H
#define ERROR_H

enum RetType {
    OK = 0,

    // 错误状态
    ARGS_ERROR,
    MEM_FAIL,
    SEND_FAIL,
    INTERNL_ERROR,
    CHECKSUM_ERROR,

    /**
     * 数据包释放指示，出现这两个的时候不进行释放
     */
    USE_PKT,
    FREED_PKT,

    /**
     * 正常处理时的信息与动作
     */
    MALFORMATION_PKT,
    NOTME_PKT,
    NO_PKT,

    /**
     * ip层
     */
    // 分片重组完成
    OK_WITH_DEFRAG,
    // 没有合适的路由表项
    NO_ROUTE,
    // 数据过大（超过65535）
    TOO_BIG,

    // 不支持
    NOT_SUPPORT_PKT,
    // 不应当出现
    OTHER,
};

#endif /* ERROR_H */
