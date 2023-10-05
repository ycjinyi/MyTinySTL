#ifndef VECTOR_H
#define VECTOR_H
template<typename T>
class vector {
public:
    //构造和析构函数
    vector(int size = 1)
        : size_(size)
        , top_(0)
        , data_(new T [size_]) 
    {}
    vector(const vector<T>& src)
        : size_(src.size_)
        , top_(src.top_)
        , data_(new T [size_]) {
        //不_要使用memcopy, 需要通过operator=来决定是深拷贝还是浅拷贝
        for(int i = 0; i < top_; ++i) {
            data_[i] = src.data_[i];
        }
    }
    vector(vector<T>&& src)
        : size_(src.size_) 
        , top_(src.top_)
        , data_(src.data_){
        src.size_ = 0;
        src.top_ = 0;
        src.data_ = nullptr;
    }
    ~vector() {
        size_ = 0;
        top_ = 0;
        delete [] data_;
    }
    
    //运算符重载
    vector<T>& operator= (const vector<T>& src) {
        if(this == &src) return *this;
        delete [] data_;
        size_ = src.size_;
        top_ = src.top_;
        data_ = new T[size_];
        for(int i = 0; i < top_; ++i) {
            data_[i] = src.data_[i];
        }
        return *this;
    }
    vector<T>& operator= (vector<T>&& src) {
        if(this == &src) return *this;
        delete [] data_;
        size_ = src.size_;
        top_ = src.top_;
        data_ = src.data_;
        src.size_ = 0;
        src.top_ = 0;
        src.data_ = nullptr;
    }
    T& operator[](int pos) {
        if(pos >= top || pos < 0) {
            throw "out of bundary!";
        }
        return data[pos];
    }
    
    //容器操作
    void push_back(const T& src) {
        if(full()) expand();
        data[top_] = src;
    }
    template<typename ...Args>
    void emplace_back(Args&& ...args) {
    }
    void pop_back() {
        if(top_ == 0) return;
        --top_;
    }
    T& front() {
        if(top_ == 0) throw "vector is empty!";
        return data[0];
    }
    T& back() {
        if(top_ == 0) throw "vector is empty!";
        return data[top_ - 1];
    }
    int capacity() { return size_; }
    int size() { return top_; }
    bool empty() { return size_ == 0; }
    bool full() { return size_ == top_; }

private:
    //扩容操作
    void expand() {
        size_ = size_ == 0 ? 1 : 2 * size_;
        T* temp = new T[size_];
        for(int i = 0; i < top_; ++i) {
            temp[i] = data[i];
        }
        delete [] data_;
        data_ = temp;
    }   
private:
    T* data_;
    int size_;
    int top_;
};

#endif