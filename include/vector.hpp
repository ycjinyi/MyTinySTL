#ifndef VECTOR_H
#define VECTOR_H
#include "allocator.hpp"
template<typename T, typename Alloc = allocator<T>> 
class vector {
public:
    //构造和析构函数
    explicit vector(int size = 1)
        : alloc_()
        , first_(alloc_.allocate(size))
        , last_(first_)
        , end_(first_ + size)
    {}
    vector(cosnt vector<T>& src) 
        : alloc_()
        , first_(alloc_.allocate(src.end_ - src.first_))
        , last_(first_ + src.last_ - src.first_)
        , end_(first_ + src.end_ - src.first_) 
    {
        int size = src.last_ - src.first_;
        for(int i = 0; i < size; ++i) {
            //在指定位置进行构造对象
            alloc_.construct(first_ + i, src[i]);
        }
    }
    vector(vector<T>&& src) 
        : alloc_()
        , first_(src.first_)
        , last_(src.last_)
        , end_(src.end_)
    {
        src.first_ = nullptr;
        src.last_ = nullptr;
        src.end_ = nullptr;
    }
    ~vector() {
        free();
    }

    //运算符重载
    vector<T>& operator= (const vector<T>& src) {
        if(this == &src) return *this;
        free();
        first_ = alloc_.allocate(src.end_ - src.first_);
        last_ = first_ + src.last_ - src.first_;
        end_ = first_ + src.end_ - src.first_;
        int size = src.last_ - src.first_;
        for(int i = 0; i < size; ++i) {
            alloc_.construct(first_ + i, src[i]);
        }
    }
    vector<T>& operator= (vector<T>&& src) {
        first_(src.first_);
        last_(src.last_);
        end_(src.end_);
        src.first_ = nullptr;
        src.last_ = nullptr;
        src.end_ = nullptr;
    }
    T& operator[] (size_t p) {
        int size = last_ - first_;
        if(p < 0 || p >= size) throw "out of boundary!";
        return first_[p];
    }

    //容器操作
    void push_back(const T& val) {
        emplace_back(val);
    }
    template<typename ...Arg>
    void emplace_back(Arg&& ...args) {
        if(full()) expand();
        alloc_.construct(last_++, std::forward<Arg>(args)...);
    }
    void pop_back() {
        if(empty()) return;
        alloc_.destroy(--last_);
    }

    T back() const {
        if(empty()) throw "out of boundary!";
        return *(last_ - 1);
    }
    T front() const {
        if(empty()) throw "out of boundary!";
        return *first_;
    }
    bool empty() const {
        return first_ == last_;
    }
    bool full() const {
        return last_ == end_;
    }

private:
    //扩容操作
    void expand() {
        int size = end_ - first_;
        size = size == 0 ? 1 : 2 * size;
        T* temp = alloc_.allocate(size);
        int count = last_ - first_;
        for(int i = 0; i < count; ++i) {
            alloc_.construct(temp + i, first_[i]);
        }
        free();
        first_ = temp;
        last_ = first_ + count;
        end_ = first_ + size;
    }
    void free() {
        //先析构所有有效元素，再释放整个空间
        for(T* p = first_; p != last_; ++p) {
            alloc_.destroy(p);
        }
        alloc_.deallocate(first_);
        first_ = nullptr;
        last_ = nullptr;
        end_ = nullptr;
    }
private:
    T* first_;
    T* last_;
    T* end_;
    Alloc alloc_;
};
#endif