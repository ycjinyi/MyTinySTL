#include "ngxmemorypool.hpp"
#include <cstdlib>
#include <algorithm>
#include <cstring>

ngx_mem_pool::ngx_mem_pool(size_t size) {
    ngx_create_pool(size);
}

ngx_mem_pool::~ngx_mem_pool() {
    ngx_destroy_pool();
}

// 创建指定size大小的内存池，但是小块内存池不超过1个页面大小4KB
void ngx_mem_pool::ngx_create_pool(size_t size) {
    // 内存分配
    size_t s = std::min(std::max(NGX_MIN_POOL_SIZE, size), NGX_MAX_ALLOC_FROM_POOL);
    void* p = malloc(s);
    if(p == nullptr) throw std::bad_alloc();
    pool_ = (ngx_pool_s*) p;
    // 指定小块内存的头部信息
    pool_->d.last = (uchar*) p + sizeof(ngx_pool_s);
    pool_->d.end = (uchar*) p + s;
    pool_->d.next = nullptr;
    pool_->d.failed = 0;
    // 指定其它信息
    pool_->max = s - sizeof(ngx_pool_data_t);
    pool_->current = &pool_->d;
    pool_->large = nullptr;
    pool_->cleanup = nullptr;
}

// 考虑内存对齐，申请size大小的内存
void* ngx_mem_pool::ngx_palloc(size_t size) {
    //大于小块内存大小，则直接分配大块内存
    if(size > pool_->max) {
        return ngx_palloc_large(size);
    }
    return ngx_palloc_small(size, 1);
}

// 不考内存对齐
void* ngx_mem_pool::ngx_pnalloc(size_t size) {
    if(size > pool_->max) {
        return ngx_palloc_large(size);
    }
    return ngx_palloc_small(size, 0);
}

// 调用ngx_palloc实现内存分配，初始化为0
void* ngx_mem_pool::ngx_pcalloc(size_t size) {
    void* ptr = ngx_palloc(size);
    if(ptr == nullptr) return nullptr;
    memset(ptr, 0, size);
    return ptr;
}

// 释放大块内存
void ngx_mem_pool::ngx_pfree(void* p) {
    ngx_pool_large_s* ptr = pool_->large;
    while(ptr) {
        if(ptr->alloc == p) {
            free(p);
            ptr->alloc = nullptr;
            return;
        }
        ptr = ptr->next;
    }
}

// 重置内存池的内存资源
void ngx_mem_pool::ngx_reset_pool() {
    // 首先释放外部资源
    ngx_pool_cleanup_s* cptr = pool_->cleanup;
    while(cptr != nullptr) {
        if(cptr->handler != nullptr) cptr->handler(cptr->data);
        cptr = cptr->next;
    }
    // 然后释放大块内存
    ngx_pool_large_s* lptr = pool_->large;
    while(lptr != nullptr) {
        if(lptr->alloc != nullptr) {
            free(lptr->alloc);
            lptr->alloc = nullptr;
        }
        lptr = lptr->next;
    }
    // 清空小块内存池
    ngx_pool_data_t* ptr = &pool_->d;
    // 注意第一块内存池的头部更长
    if(ptr != nullptr) {
        ptr->last = (uchar*)&pool_->d + sizeof(ngx_pool_s);
        ptr->failed = 0;
        ptr = ptr->next;
    }
    while(ptr != nullptr) {
        ptr->last = (uchar*)ptr + sizeof(ngx_pool_data_t);
        ptr->failed = 0;
        ptr = ptr->next;
    }
    // 重置头部信息
    pool_->current = &pool_->d;
    pool_->large = nullptr;
    pool_->cleanup = nullptr;
}

// 内存池的销毁函数
void ngx_mem_pool::ngx_destroy_pool() {
    // 先释放除小内存池的所有资源
    ngx_reset_pool();
    // 释放小内存池的资源
    ngx_pool_data_t* ptr = pool_->current;
    while(ptr != nullptr) {
        ngx_pool_data_t* temp = ptr->next;
        free(ptr);
        ptr = temp;
    }
    pool_ = nullptr;
}

// 添加待清理的数据
ngx_pool_cleanup_s* ngx_mem_pool::ngx_pool_cleanup_add() {
    ngx_pool_cleanup_s* ptr = (ngx_pool_cleanup_s*)ngx_palloc(sizeof(ngx_pool_cleanup_s));
    if(ptr == nullptr) return nullptr;
    ptr->handler = nullptr;
    ptr->data = nullptr;
    ptr->next = pool_->cleanup;
    pool_->cleanup = ptr;
    return ptr;
}

// 分配小块内存
void* ngx_mem_pool::ngx_palloc_small(size_t size, uint align) {
    uchar* m = nullptr;
    // 从当前的小内存池开始向后找
    ngx_pool_data_t* p = pool_->current;
    do {
        // 找到当前的空余位置
        m = p->last;
        if(align == 1) {
            //需要将指针的地址向上对齐
            m = ngx_align_ptr((upointer_t) m, NGX_ALIGNMENT);
        }
        // 如果剩余的空间足够，则直接返回
        if((size_t)(p->end - m) >= size) {
            p->last = m + size;
            return m;
        }
        // 不够则从下一块内存池中继续寻找
        p = p->next;
    } while(p != nullptr);
    // 如果都没有找到，则直接创建一块新的小内存池，并返回分配的首地址
    return ngx_palloc_block(size);
}

// 分配新的小块内存池
void* ngx_mem_pool::ngx_palloc_block(size_t size) {
    // 内存分配
    size_t s = pool_->max + sizeof(ngx_pool_data_t);
    void* p = malloc(s);
    if(p = nullptr) return nullptr;
    // 指定参数
    ngx_pool_data_t* head = (ngx_pool_data_t*) p;
    uchar* m = (uchar*) p + sizeof(ngx_pool_data_t);
    // 需要将指针的地址向上对齐
    m = ngx_align_ptr((upointer_t) m, NGX_ALIGNMENT);
    // last直接向后偏移，将当前的需要的内存预留出来
    head->last = m + size;
    head->end = (uchar*) p + s;
    head->failed = 0;
    // 增加失败计数, 并向后移动current指针
    ngx_pool_data_t* ptr = pool_->current;
    while(ptr->next) {
        if(++ptr->failed > 4) {
            pool_->current = ptr->next;
        }
        ptr = ptr->next;
    }
    //将当前内存池挂在最后
    ptr->next = head;
    if(++ptr->failed > 4) {
        pool_->current = ptr->next;
    }
    // 返回分配的内存
    return m;
}

// 分配大块内存
void* ngx_mem_pool::ngx_palloc_large(size_t size) {
    // 先分配内存
    void* p = malloc(size);
    if(p == nullptr) return nullptr;
    // 在大块内存的头部链表中找到空的头部, 只找前3个
    int n = 0;
    for(ngx_pool_large_s* ptr = pool_->large;
        ptr != nullptr; ptr = ptr->next) {
        if(ptr->alloc == nullptr) {
            ptr->alloc = p;
            return p;
        }
        if(++n > 3) break;
    }
    // 如果没有找到，就在小块内存池中分配一块大块内存的头部
    ngx_pool_large_s* ptr = (ngx_pool_large_s*)ngx_palloc_small(sizeof(ngx_pool_large_s), 1);
    if(ptr == nullptr) {
        // 注意内存泄漏
        free(p);
        return nullptr;
    }
    ptr->alloc = p;
    ptr->next = pool_->large;
    pool_->large = ptr;
    return p;
}

// 指针的内存对齐
uchar* ngx_mem_pool::ngx_align_ptr(upointer_t pointer, upointer_t align) {
    // 直接根据机器位数将指针转换为对应的整形, 然后向上调整对齐后再转回指针
    return (uchar*) ((pointer + (align - 1)) & ~(align - 1));
}
