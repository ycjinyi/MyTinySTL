#ifndef DESIGN_PATTERN_H
#define DESIGN_PATTERN_H
#include <iostream>
#include <unordered_map>
#include <vector>
//单例模式
class singleton {
public:
    singleton(const singleton&) = delete;
    singleton& operator= (const singleton&) = delete;
    static singleton* getInstance() {
        /* 懒汉模式，静态局部变量只会初始化一次
        static singleton* ptr = new singleton();
        return ptr;
        */
       return ptr_;
    }
private:
    singleton() = default;
    /*
    饿汉模式，通过动态初始化在main函数执行之前初始化完毕
    */
    static singleton* ptr_;
};
singleton* singleton::ptr_ = new singleton();

//观察者模式
//观察者, 提供关注的主题更新后的回调函数
class observer {
public:
    virtual void notify(int topic) = 0;
};
//不同的观察者可以根据主题执行不同的动作
class observer1: public observer {
private:
    void func1() {
        std::cout << "func1() running" << std::endl;
    }
    void func2() {
        std::cout << "func2() running" << std::endl;
    }
public:
    void notify(int topic) override {
        switch(topic) {
            case 1:
                func1();
                break;
            case 2:
                func2();
                break;
            default:
                break;
        }
    }
};
//主题，用于观察者订阅主题，和发布主题通知订阅者
class object {
public:
    void subscribe(int topic, observer* ob) {
        auto it = table_.find(topic);
        if(it == table_.end()) table_.insert({topic, std::vector<observer*> (1, ob)});
        else it->second.push_back(ob);
    }
    void dispatch(int topic) {
        auto it = table_.find(topic);
        if(it == table_.end()) return;
        std::vector<observer*>& lists = it->second;
        for(observer* ob: lists) {
            ob->notify(topic);
        }
    }
private:
    std::unordered_map<int, std::vector<observer*>> table_;
};
#endif