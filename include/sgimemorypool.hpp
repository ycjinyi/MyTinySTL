#ifndef SGI_MEMORY_POOL_H
#define SGI_MEMORY_POOL_H
#include <cstdlib>
#include <mutex>

// 需要保证线程安全性
template<typename T>
class sgi_allocator {
public:
    // 内存的开辟和释放
    T* allocate(size_t size) {
        // 如果需要开辟的内存过大，则直接malloc
        if(size > memory_param::_MAX_BYTES) {
            return static_cast<T*> (malloc(size));
        }
        // 反之从自由链表中寻找目标内存块
        size_t index = freelist_index(size);
        // 如果自由链表在该处没有下挂内存块，则需要分配
        if(_freelist[index] == nullptr) {

        } else { 
        // 如果有内存块，则直接分配出去
            obj* head = _freelist[index];
            _freelist[index] = head->next;
            return head->data;
        }
    }
    void deallocate(T* ptr, size_t size) {
        
    }
    // 内存的扩容/缩容
    T* reallocate(T* ptr, size_t oldSize, size_t newSize) {

    }
    // 对象的构造和析构
    template<typename ...Arg>
    void construct(T* ptr, Arg&& ...arg) {
        new (ptr) T(std::forward<Arg>(arg)...);
    }
    void destroy(T* ptr) {
        ptr->~T();
    }
private:
    enum class memory_param {
        _ALIGN = 8, // 自由链表按照_ALIGN大小的字节数对齐且等差递增
        _MAX_BYTES = 128, // chunk块的最大字节数
        _NFREELISTS = 16 // 自由链表的个数
    };
    // 自由链表中保存的chunk块的头部信息
    union obj {
        obj* next;
        char* data;
    };
    // 自由链表
    static volatile obj* _freelist[memory_param::_NFREELISTS];
    // 已分配的内存情况
    static char* _start_free;
    static char* _end_free;
    static size_t _heap_size;
    // 用于线程同步
    static std::mutex _mutex;
private:
    // 内存大小向上取整
    static size_t round_up(size_t bytes) {
        return (bytes + (size_t)memory_param::_ALIGN - 1) 
            & ~((size_t)memory_param::_ALIGN - 1);
    }
    // 计算大小为bytes的内存块在自由链表中的编号, 从0开始
    static size_t freelist_index(size_t bytes) {
        return ((bytes + (size_t)memory_param::_ALIGN - 1) 
            / (size_t)memory_param::_ALIGN) - 1;
    }
    // 
    static void* refill(size_t size) {

    }
};

template<typename T>
volatile typename sgi_allocator<T>::obj* 
    sgi_allocator<T>::_freelist[sgi_allocator<T>::memory_param::_NFREELISTS] = 
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
template<typename T>
char* sgi_allocator<T>::_start_free = nullptr;
template<typename T>
char* sgi_allocator<T>::_end_free = nullptr;
template<typename T>
size_t sgi_allocator<T>::_heap_size = 0;
template<typename T>
std::mutex sgi_allocator<T>::_mutex;
#endif