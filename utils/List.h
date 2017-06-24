/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：List.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-02-07
 **
 ** 描  述：可多重的双链表的实现
 ** 注  意：
 ********************************************************************/
#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#define List_foreach(node, temp)                                        \
    for (ListLink *(temp) = (node); (temp) != 0; (temp) = (temp)->next)

#define List_safe_foreach(head, temp, pnode)                            \
    for ((temp) = (head), (pnode) = (head)->next;                       \
         (temp) != NULL ;                                               \
         (temp) = (pnode), ((pnode) ? ((pnode) = (pnode)->next) : (pnode)))

// 链接器
struct ListLink {
    struct ListLink *prev;
    struct ListLink *next;
};

template <class T>
class List
{
public:

    typedef ListLink Link;

public:

    inline void config(int offset = 0);

    inline void attach(Link *node, T *data);

    inline void detach(Link *node);

    inline T*   locate(Link *node);

    inline void push(T *data);

    inline void append(T *data);

    inline T*   pop();

    inline T*   pop_back();

public:

    Link *head;
    Link *tail;
    int count;
    int offset;
};

template <class T>
inline void List<T>::config(int offset)
{
    memset(this, 0, sizeof(*this));
    this->offset = offset;
}

template <class T>
inline void List<T>::attach(Link *node, T*data)
{
    Link *temp = (Link*)((char*)data + offset);

    temp->prev = node;
    temp->next = (node == 0) ? head : node->next;
    if (temp->prev == 0) head = temp;
    else                 temp->prev->next = temp;
    if (temp->next == 0) tail = temp;
    else                 temp->next->prev = temp;
    count++;
}

template <class T>
inline void List<T>::detach(Link *node)
{
    if (node->prev == 0) head = node->next;
    else                 node->prev->next = node->next;
    if (node->next == 0) tail = node->prev;
    else                 node->next->prev = node->prev;
    count--;
}

template <class T>
inline T *List<T>::locate(Link *node)
{
    return (T*)((char*)node - offset);
}

template <class T>
inline void List<T>::push(T *data)
{
    attach(0, data);
}

template <class T>
inline void List<T>::append(T *data)
{
    attach(tail, data);
}

template <class T>
inline T* List<T>::pop()
{
    T *ret = locate(head);
    detach(head);
    return ret;
}

template <class T>
inline T* List<T>::pop_back()
{
    T *ret = locate(tail);
    detach(tail);
    return ret;
}

#endif /* RESOURCE_LIST_H */
