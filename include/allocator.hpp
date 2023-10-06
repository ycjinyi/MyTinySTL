#ifndef ALLOCATOR_H
#define ALLOCATOR_H
//空间配置器：内存开辟和释放，对象构造和析构
template<typename T>
class allocator {
public:
    T* allocate(size_t size) {
        return static_cast<T*> (malloc(size * sizeof(T)));
    }
    void deallocate(T* ptr) {
        free(ptr);
    }
    template<typename ...Arg>
    void construct(T* ptr, Arg&& ...args) {
        //在指定位置构造T
        new (ptr) T(std::forward<Arg>(args)...);
    }
    void destroy(T* ptr) {
        ptr->~T();
    }
};
#endif