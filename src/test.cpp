#include <iostream>
#include "vector.hpp"
#include "string.hpp"
#include "shared_ptr.hpp"
class Test {
public:
    Test() { std::cout << "Test()" << std::endl;}
    Test(int num): number(num) { std::cout << "Test(int)" << std::endl;}
    Test(const Test&) { std::cout << "Test(const Test&)" << std::endl;}
    Test(Test&&) { std::cout << "Test(Test&&)" << std::endl;} 
    ~Test() { std::cout << "~Test()" << std::endl; }
    int number;
};
std::ostream& operator<< (std::ostream& os, const Test& t) {
    os << "cout << Test";
    return os;
}

namespace stdtest{
template<typename T>
struct remove_reference {
    //定义类型成员，类型在前，名字在后
    typedef T type;
};

template<typename T>
struct remove_reference<T&> {
    typedef T type;
};

template<typename T>
struct remove_reference<T&&> {
    typedef T type;
};

template<typename T>
typename remove_reference<T>::type&& move(T&& e) {
    return static_cast<typename remove_reference<T>::type&&> (e);
}
}

int main() {
    /*
    vector<Test> vec;
    vec.reserve(20);
    Test a(10);
    std::cout << "------" << std::endl;
    vec.push_back(a);
    vec.emplace_back(a);
    std::cout << "------" << std::endl;
    vec.push_back(10);
    vec.emplace_back(10);
    std::cout << "------" << std::endl;
    vec.push_back(Test());
    vec.emplace_back(Test());
    std::cout << vec[2] << std::endl;
    */

    /*
    string s1 = "aaa";
    std::cout << s1 << std::endl;
    string s2 = stdtest::move(s1);
    std::cout << s2 << std::endl;
    string s3 = "ddd" + s2;
    std::cout << s3 << std::endl;
    bool eq = s3 == s2;
    std::cout << eq << std::endl;
    */
    {
        shared_ptr<Test> sp(new Test(20));
        std::cout << sp->number << std::endl;
        std::cout << sp.use_count() << std::endl;
        shared_ptr<Test> sp1(sp);
        std::cout << sp1->number << std::endl;
        std::cout << sp.use_count() << std::endl;
    }
    std::cout << "------" << std::endl;
    return 0;
}