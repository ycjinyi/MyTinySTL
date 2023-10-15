#include <iostream>
#include <functional>
#include "vector.hpp"
#include "string.hpp"
#include "shared_ptr.hpp"
#include "lrucache.hpp"
#include "designpattern.hpp"
#include "ngxmemorypool.hpp"

class Test {
public:
    Test(): number(0) { std::cout << "Test()" << std::endl;}
    Test(int num): number(num) { std::cout << "Test(int)" << std::endl;}
    Test(const Test& t): number(t.number) { std::cout << "Test(const Test&)" << std::endl;}
    Test(Test&& t): number(t.number) { std::cout << "Test(Test&&)" << std::endl;} 
    ~Test() { std::cout << "~Test()" << std::endl; }
    int number;
    char* pointer;
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

typedef std::function<void()> func;

void clean(Test* t) {
    std::cout << "release t" << std::endl;
    free(t->pointer);
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
    vector<Test>::iterator it = vec.begin();
    for(; it != vec.end(); ++it) {
        std::cout << (*it).number << std::endl;
    }
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
    /*
    {
        shared_ptr<Test> sp(new Test(20));
        std::cout << sp->number << std::endl;
        std::cout << sp.use_count() << std::endl;
        shared_ptr<Test> sp1(sp);
        std::cout << sp1->number << std::endl;
        std::cout << sp.use_count() << std::endl;
    }
    std::cout << "------" << std::endl;
    */

    /*
    LRUCache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << cache.get(2) << std::endl;
    */

    /*
    observer1 ob1;
    object obj;
    obj.subscribe(2, &ob1);
    obj.dispatch(2);
    */

    /*
    ngx_mem_pool pool(512);
    Test* p = (Test*)pool.ngx_palloc(sizeof(Test));
    p->pointer = (char*)malloc(sizeof(char));
    func f = std::bind(&clean, p);
    ngx_pool_cleanup_s* ptr = pool.ngx_pool_cleanup_add();
    ptr->handler = f;
    */
   
    return 0;
}