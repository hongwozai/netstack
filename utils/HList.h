/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：HList.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-02-05
 **
 ** 描  述：模仿linux内核hash表
 ** 注  意：1.
 ********************************************************************/
#ifndef RESOURCE_HLIST_H
#define RESOURCE_HLIST_H

#define HList_foreach(n, t)                             \
    for (HashLink *(t) = (n); t != NULL; t = t->next)

#define HList_safe_foreach(head, temp, pnode)                           \
    for ((temp) = (head), ((head) ? ((pnode) = (head)->next) : (head)); \
         (temp) != NULL ;                                               \
         (temp) = (pnode), ((pnode) ? ((pnode) = (pnode)->next) : (pnode)))

// 链接器结构
struct HashLink {
    HashLink *next;
    HashLink **pprev;
};

class HList
{
public:

    typedef HashLink Link;

public:

    inline void init();

    inline void push(Link *link);

    inline Link* pop();

    inline void del(Link *n);

    inline void add_after(Link *m, Link *n);

    inline void add_before(Link *m, Link *n);

public:

    HashLink *head;
};

inline void HList::init()
{
    head = 0;
}

inline void HList::push(Link *link)
{
    link->next  = head;
    link->pprev = &head;
    if (link->next)
        link->next->pprev = &link->next;
    head = link;
}

inline HList::Link* HList::pop()
{
    Link *temp = head;
    del(head);
    return temp;
}

inline void HList::del(Link *n)
{
    if (!n)
        return;
    if (n->next)
        n->next->pprev = n->pprev;
    if (n->pprev)
        *n->pprev = n->next;
}

inline void HList::add_after(Link *m, Link *n)
{
    n->pprev = &m->next;
    n->next  = m->next;

    m->next = n;
    if (n->next)
        n->next->pprev = &n->next;
}

inline void HList::add_before(Link *m, Link *n)
{
    n->pprev = m->pprev;
    n->next  = m;

    m->pprev = &n->next;
    *n->pprev = n;
}

#endif /* RESOURCE_HLIST_H */
