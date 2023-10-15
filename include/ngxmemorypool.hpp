#ifndef NGX_MEMORY_POOL_H
#define NGX_MEMORY_POOL_H
#include <functional>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long size_t;
typedef unsigned long upointer_t;

// 清理函数
using ngx_pool_cleanup_pt = std::function<void()>;
// 内存池的数据清理信息，链表组织，数据可以通过绑定器传入
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt handler;
    ngx_pool_cleanup_s *next;
};

// 内存池分配的大块内存的头部信息，双链表组织
struct ngx_pool_large_s {
    ngx_pool_large_s *next;
    void *alloc;
};

// 内存池分配的小块内存的头部信息，链表组织
struct ngx_pool_data_t {
    uchar *last;
    uchar *end;
    ngx_pool_data_t *next;
    uint failed;
};

// 内存池的头部信息，包含小块内存、大块内存、数据清理的链表头部
struct ngx_pool_s {
    ngx_pool_data_t d;
    size_t max;
    ngx_pool_data_t *current;
    ngx_pool_large_s *large;
    ngx_pool_cleanup_s *cleanup;
};

// 把data向上调整至align的倍数: 加上align - 1而忽略align - 1的二进制位
#define ngx_align(data, align) (((data) + (align - 1)) & ~(align - 1));
// 指针对齐，需要根据系统位数决定不同字节数
#define NGX_ALIGNMENT sizeof(long)

const size_t NGX_PAGESIZE = 4096;
const size_t NGX_MAX_ALLOC_FROM_POOL = NGX_PAGESIZE - 1;
const size_t NGX_DEFAULT_POOL_SIZE = 16 * 1024;
const size_t NGX_POOL_ALIGNMENT = 16;
const size_t NGX_MIN_POOL_SIZE = 
    ngx_align(sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s), NGX_POOL_ALIGNMENT);

class ngx_mem_pool {
public:
    ngx_mem_pool(size_t size);
    ~ngx_mem_pool();
    // 考虑内存对齐，申请size大小的内存
    void* ngx_palloc(size_t size);
    // 不考内存对齐
    void* ngx_pnalloc(size_t size);
    // 调用ngx_palloc实现内存分配，初始化为0
    void* ngx_pcalloc(size_t size);
    // 释放大块内存
    void ngx_pfree(void* p);
    // 重置内存池的内存资源
    void ngx_reset_pool();
    // 添加待清理的数据
    ngx_pool_cleanup_s* ngx_pool_cleanup_add();
private:
    // 创建指定size大小的内存池，但是小块内存池不超过1个页面大小4KB
    void ngx_create_pool(size_t size);
    // 内存池的销毁函数
    void ngx_destroy_pool();
    // 分配小块内存
    void* ngx_palloc_small(size_t size, uint align);
    // 分配大块内存
    void* ngx_palloc_large(size_t size);
    // 分配小块内存池
    void* ngx_palloc_block(size_t size);
    // 指针的内存对齐
    uchar* ngx_align_ptr(upointer_t pointer, upointer_t align);
private:
    ngx_pool_s* pool_;
};

#endif