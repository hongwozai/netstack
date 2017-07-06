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

    /**
     * 正常处理时的信息与动作
     */
    MALFORMATION_PKT,
    NOTME_PKT,
    // 数据包正在使用中
    USE_PKT,
    NO_PKT,

    // 不应当出现
    OTHER,
};

#endif /* ERROR_H */
