#ifndef STRING_H
#define STRING_H
#include <memory>
#include <iostream>
#include <string.h>
class string {
friend std::ostream& operator<< (std::ostream& os, const string& t);
friend string operator+ (const string& lhs, const string& rhs);
public:
    //构造和析构函数
    //注意分配的空间大小是strlen的长度+1
    string(const char* ptr = nullptr) {
        if(ptr != nullptr) {
            ptr_.reset(new char[strlen(ptr) + 1]);
            strcpy(ptr_.get(), ptr);
            return;
        }
        ptr_.reset(new char[1] {'\0'});
    }
    string(const string& rhs) {
        ptr_.reset(new char[strlen(rhs.ptr_.get()) + 1]);
        strcpy(ptr_.get(), rhs.ptr_.get());
    }
    string(string&& rhs) noexcept {
        ptr_ = std::move(rhs.ptr_);
    }
    ~string() = default;

    //操作符
    string& operator= (const string& rhs) {
        if(this == &rhs) return *this;
        ptr_.reset(new char[strlen(rhs.ptr_.get()) + 1]);
        strcpy(ptr_.get(), rhs.ptr_.get());
        return *this;
    }
    string& operator= (string&& rhs) noexcept {
        ptr_ = std::move(rhs.ptr_);
        return *this;
    }
    //注意引用的返回，一定不是局部变量
    char& operator[](size_t p) {
        return ptr_[p];
    }
    const char& operator[](size_t p) const {
        return ptr_[p];
    }
    bool operator> (const string& rhs) const{
        return strcmp(ptr_.get(), rhs.ptr_.get()) > 0;
    }
    bool operator< (const string& rhs) const {
        return strcmp(ptr_.get(), rhs.ptr_.get()) < 0;
    }
    bool operator== (const string& rhs) const {
        return strcmp(ptr_.get(), rhs.ptr_.get()) == 0;
    }

    //其它操作
    const char* c_str() const {
        return ptr_.get();
    }
private:
    std::unique_ptr<char []> ptr_;
};
std::ostream& operator<< (std::ostream& os, const string& t) {
    os << t.ptr_.get();
    return os;
} 
string operator+ (const string& lhs, const string& rhs) {
    string temp(new char[strlen(lhs.ptr_.get()) + strlen(rhs.ptr_.get()) + 1]);
    strcpy(temp.ptr_.get(), lhs.ptr_.get());
    strcat(temp.ptr_.get(), rhs.ptr_.get());
    //由于返回值优化，这里并没有再构造新的对象
    return temp;
}
#endif