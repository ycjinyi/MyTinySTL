#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H
#include <iostream>
// 对象池实现
template<typename T> 
class forward_list {
public:
    forward_list(): _head(new node(T())), _rear(_head) {}
    ~forward_list() {
        node* ptr = _head;
        while(ptr != nullptr) {
            _head = _head->_next;
            delete ptr;
            ptr = _head;
        }
        _head = nullptr;
        _rear = nullptr;
    }
    template<typename ...Arg>
    void emplace_back(Arg&& ...args) {
        node* temp = new node(std::forward<Arg>(args)...);
        _rear->_next = temp;
        _rear = temp;
    }
    void push_back(const T& val) {
        emplace_back(val);
    }
    void push_back(T&& val) {
        emplace_back(std::move(val));
    }
    void pop_front() {
        if(empty()) return;
        node* ptr = _head->_next;
        _head->_next = ptr->_next;
        delete ptr;
        // 原先只有一个节点
        if(_head->_next == nullptr) {
            _rear = _head;
        }
    }
    bool empty() const {
        return _head == _rear;
    }
private:
    class node {
    public:
        template<typename ...Arg>
        node(Arg&& ...args): _data(std::forward<Arg>(args)...), _next(nullptr) 
        {}
        ~node() {
            _data.~T();
            _next = nullptr;
        }
        // 对于operator new和delete而言, 本身就是静态方法
        // 因为分配内存时对象未构造, 释放内存时对象已经析构了
        void* operator new(size_t size) {
            if(_pool == nullptr) {
                size *= _cap;
                //std::cout << _cap << std::endl;
                _pool = (node*)malloc(size);
                if(_pool == nullptr) throw std::bad_alloc();
                _arrays[_idx++] = _pool;
                // 将静态链表串联
                node* ptr = _pool;
                for(; ptr != _pool + _cap - 1; ++ptr) {
                    ptr->_next = ptr + 1;
                }
                ptr->_next = nullptr;
                _cap *= 2;
            }
            node* ptr = _pool;
            _pool = _pool->_next;
            return (void*)ptr;
        }
        void operator delete(void* ptr) {
            node* _ptr = (node*) ptr;
            // 归还到静态链表中
            if(_pool == nullptr) {
                _pool = _ptr;
                return;
            }
            _ptr->_next = _pool->_next;
            _pool = _ptr;
        }
        T _data;
        node* _next;
    private:
        // 使用静态链表的方式分配空间
        static node* _pool;
        static unsigned int _cap;
        static node* _arrays[15];
        static unsigned int _idx;
    };
    node* _head;
    node* _rear;
};
template<typename T>
typename forward_list<T>::node* forward_list<T>::node::_pool = nullptr;
template<typename T>
unsigned int forward_list<T>::node::_cap = 1;
template<typename T>
typename forward_list<T>::node* forward_list<T>::node::_arrays[15] = 
    {nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr};
template<typename T>
unsigned int forward_list<T>::node::_idx = 0;
#endif