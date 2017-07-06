/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Endian.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-28
 **
 ** 描  述：大小段相关封装
 ** 注  意：1.
 ********************************************************************/
#ifndef ENDIAN_H
#define ENDIAN_H

#undef BIG_ENDIAN

#ifdef BIG_ENDIAN
#define ntoh64(x)  x
#define ntoh32(x)  x
#define ntoh16(x)  x
#define hton64(x)  x
#define hton32(x)  x
#define hton16(x)  x
#else  // LITTLE_ENDIAN
#define ntoh64(x)  (__builtin_bswap64(x))
#define ntoh32(x)  (__builtin_bswap32(x))
#define ntoh16(x)  (__builtin_bswap16(x))
#define hton64(x)  (__builtin_bswap64(x))
#define hton32(x)  (__builtin_bswap32(x))
#define hton16(x)  (__builtin_bswap16(x))
#endif

#endif /* ENDIAN_H */
