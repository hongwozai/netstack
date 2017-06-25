/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Log.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-24
 **
 ** 描  述：日志部分
 ** 注  意：1.
 ********************************************************************/
#ifndef LOG_H
#define LOG_H

#define log(fmt, args...)                                       \
    fprintf(stdout, "[%s %d] "fmt, __FILE__, __LINE__, ##args);

#define err(fmt, args...)                                       \
    fprintf(stderr, "[%s %d] "fmt, __FILE__, __LINE__, ##args);

#define syserr(fmt, args...)                                            \
    fprintf(stderr, "[%s %d] "fmt"(%m)", __FILE__, __LINE__, ##args);

#define fatal(fmt, args...)                                     \
    fprintf(stderr, "[%s %d] "fmt, __FILE__, __LINE__, ##args);

#endif /* LOG_H */
