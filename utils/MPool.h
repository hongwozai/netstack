/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：MPool.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-22
 **
 ** 描  述：定长内存池
 ** 注  意：1.
 ********************************************************************/
#ifndef MPOOL_H
#define MPOOL_H

#include <cstdlib>
#include <stdint.h>
#include "List.h"

template <class T>
class MPool
{
public:

    inline int  init(uint32_t num, uint32_t size = sizeof(T));

    inline T*   attach();

    inline void detach(T *n);

    inline void destroy();

private:

    List<T> freelist;

    void *disk;

    void *end;

    uint32_t num;

    uint32_t size;
};

template <class T>
inline int MPool<T>::init(uint32_t num, uint32_t size)
{
    uint64_t total;

    memset(this, 0, sizeof(*this));

    if (size < sizeof(ListLink)) {
        size = sizeof(ListLink);
    }
    total = num * size;

    disk = (T*)malloc(total);
    if (!disk) {
        return -1;
    }
    for (uint32_t i = 0; i < num; i++) {
        freelist.push((T*)((char*)disk + i * size));
    }
    end = (char*)disk + total;
    this->num  = num;
    this->size = size;
    return 0;
}

template <class T>
inline void MPool<T>::destroy()
{
    free(disk);
}

template <class T>
inline T *MPool<T>::attach()
{
    if (freelist.count == 0) {
        return (T*)malloc(size);
    }
    return freelist.pop();
}

template <class T>
inline void MPool<T>::detach(T *n)
{
    if (n >= disk && n < end) {
        freelist.push(n);
    } else {
        free(n);
    }
}

#endif /* MPOOL_H */
