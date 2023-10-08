#include <iostream>
#include "vector.hpp"
class Test {
public:
    Test() { std::cout << "Test()" << std::endl;}
    Test(int) { std::cout << "Test(int)" << std::endl;}
    Test(const Test&) { std::cout << "Test(const Test&)" << std::endl;}
    Test(Test&&) { std::cout << "Test(Test&&)" << std::endl;} 
    ~Test() { std::cout << "~Test()" << std::endl; }
};
int main() {
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
    return 0;
}