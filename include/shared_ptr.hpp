#ifndef SHARED_PTR_H
#define SHARED_PTR_H
template<typename T>
void mySwap(T& l, T& r) {
    T temp = std::move(r);
    r = std::move(l);
    l = std::move(temp);
}
template<typename T>
class shared_ptr {
public:
    //构造和析构函数
    shared_ptr(T* ptr):
        ptr_(ptr), 
        count_(new size_t {1}) 
    { }
    shared_ptr(const shared_ptr<T>& rhs):
        ptr_(rhs.ptr_), 
        count_(rhs.count_)  
    {
        ++*count_;
    }
    shared_ptr(shared_ptr<T>&& rhs) noexcept:
        ptr_(rhs.ptr_), 
        count_(rhs.count_) 
    {
        rhs.ptr_ = nullptr;
        rhs.count_ = nullptr;
    }
    ~shared_ptr() {
        reset();
    }

    //运算符重载
    shared_ptr<T>& operator= (const shared_ptr<T>& rhs) {
        if(this == &rhs) return *this;
        reset();
        ptr_ = rhs.ptr_;
        count_ = rhs.count_;
        ++*count_;
        return *this;
    }
    shared_ptr<T>& operator= (shared_ptr<T>&& rhs) noexcept {
        if(this == &rhs) return *this;
        reset();
        ptr_ = rhs.ptr_;
        count_ = rhs.count_;
        rhs.ptr_ = nullptr;
        rhs.count_ = nullptr;
        return *this;
    }
    T* operator-> () {
        return ptr_;
    }
    
    //操作
    T* get() {
        return ptr_;
    }
    const T* get() const {
        return ptr_;
    }
    void swap(shared_ptr<T>& rhs) {
        if(this == &rhs) return;
        mySwap(ptr_, rhs.ptr_);
        mySwap(count_, rhs.count_);
    }
    void reset() {
        --*count_;
        if(*count_ <= 0) {
            delete ptr_;
            delete count_;
            ptr_ = nullptr;
            count_ = nullptr;
        }
    }
    void reset(T* ptr) {
        reset();
        ptr_ = ptr;
        count_ = new size_t {1};
    }
    size_t use_count() const {
        return *count_;
    }
    
private:
    T* ptr_;
    size_t* count_;
};
#endif