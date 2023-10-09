#include <iostream>
#include "vector.hpp"
#include "string.hpp"
class Test {
public:
    Test() { std::cout << "Test()" << std::endl;}
    Test(int) { std::cout << "Test(int)" << std::endl;}
    Test(const Test&) { std::cout << "Test(const Test&)" << std::endl;}
    Test(Test&&) { std::cout << "Test(Test&&)" << std::endl;} 
    ~Test() { std::cout << "~Test()" << std::endl; }
};
std::ostream& operator<< (std::ostream& os, const Test& t) {
    os << "cout << Test";
    return os;
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

    string s1 = "aaa";
    std::cout << s1 << std::endl;
    string s2 = std::move(s1);
    std::cout << s2 << std::endl;
    string s3 = "ddd" + s2;
    std::cout << s3 << std::endl;
    bool eq = s3 == s2;
    std::cout << eq << std::endl;

    return 0;
}