#ifndef LRUCACHE_H
#define LRUCACHE_H
#include <unordered_map>
using std::unordered_map;
//LRU缓存
/*
基本思路是维护一个双向链表，链表的头部是最新的节点，而尾部是最旧的节点
当更新数据或查询节点数据时，为了快速找到对应的节点，需要维护一个key值到节点node的哈希表
更新节点时，直接从原来的位置删除节点，然后采用头插法排到链表最新位置
*/
template<typename keyType, typename valueType>
class LRUCache {
private:
    //双链表结构，定义节点
    struct node {
        node* next;
        node* pre;
        keyType key;
        valueType value;
        node() = default;
        node(const keyType& k, const valueType& v): key(k), value(v), next(nullptr), pre(nullptr) {}
    };
private:
    int cap_;
    node* head_;
    node* tail_;
    unordered_map<keyType, node*> key2Node;

    void remove(node* last) {
        last->pre->next = last->next;
        last->next->pre = last->pre;
    }
    void insert(node* now) {
        now->next = head_->next;
        now->next->pre = now;
        now->pre = head_;
        head_->next = now;
    }
    void makeNew(node* n) {
        remove(n);
        insert(n);
    }
public:
    explicit LRUCache(int capacity): cap_(capacity) {
        head_ = new node();
        tail_ = new node();
        head_->next = tail_;
        head_->pre = tail_;
        tail_->next = head_;
        tail_->pre = head_;
    }

    ~LRUCache() {
        node* it = head_;
        while(it != tail_) {
            node* temp = it->next;
            delete it;
            it = temp;
        }
        delete it;
    }
    
    valueType get(keyType key) {
        auto it = key2Node.find(key);
        if(it == key2Node.end()) return valueType();
        makeNew(it->second);
        return it->second->value;
    }
    //插入节点时，需要考虑当前key对应的节点是否已经存在
    //如果不存在，则需要创建新节点，但要考虑此时的容量是否已经满了
    void put(keyType key, valueType value) {
        auto it = key2Node.find(key);
        if(it == key2Node.end()) {
            if(cap_ == key2Node.size()) {
                node* last = tail_->pre;
                remove(last);
                key2Node.erase(last->key);
                delete last;
            }
            node* now = new node(key, value);
            insert(now);
            key2Node.insert({key, now});
        } else {
            it->second->value = value;
            makeNew(it->second);
        }
    }
};
#endif