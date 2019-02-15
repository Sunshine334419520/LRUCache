/**
* @Author: YangGuang
* @Date:   2019-02-13
* @Email:  guang334419520@126.com
* @Filename: lru_canche.h
* @Last modified by:  YangGuang
*/
#ifndef BASE_LRU_CACHE_H
#define BASE_LRU_CACHE_H

#include <unordered_map>

const std::size_t kDefaultCacheSize = 256;

template <typename Key, typename Value>
struct Node {
    Key key;
    Value value;
    Node<Key, Value>* prev;
    Node<Key, Value>* next;

    Node() = default;
    Node(Key k, Value v, Node* p = nullptr, Node* n = nullptr)
        : key(k), value(v), prev(p), next(n) { 
        next->prev = this;
        prev->next = this;
    }
};

template <typename Key, typename Value>
class LRUCache {
 public:
    explicit LRUCache(std::size_t capacity);

    void Insert(Key key, Value value);
    // 查找这个key在当前的cache中没，如果在返回true，并且设置value，
    // 如果不在返回false
    bool LookUp(Key key, Value* value);
    
 private:
     void DeleteNode(Node<Key, Value>* node) {
         node->prev->next = node->next;
         node->next->prev = node->prev;
     }

     void InsertHead(Node<Key, Value>* node) {
         node->next = head_->next;
         node->prev = head_;
         head_->next->prev = node;
         head_->next = node;
     }
     // 用于保存cache的数据结构的一个缩写，使用hash table + bothway list
     // 来作为cache的数据结构，使用hash table的作用是为了将查找复杂度从O(n)
     // 降到O(1).
     using CacheTable = std::unordered_map<Key, Node<Key, Value>*>;

     CacheTable cache_table_;
     std::size_t capacity_;     // cache的最大容量.
     Node<Key, Value>* tail_;   // node tail.
     Node<Key, Value>* head_;   // node head.
};

template <typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(std::size_t capacity)
    : capacity_(capacity) {
    head_ = new Node<Key, Value>();
    tail_ = new Node<Key, Value>();
    head_->prev = nullptr;
    head_->next = tail_;
    tail_->prev = head_;
    tail_->next = nullptr;
}

template<typename Key, typename Value>
inline void LRUCache<Key, Value>::Insert(Key key, Value value) {
    typename CacheTable::iterator iter = cache_table_.find(key);
    if (iter != cache_table_.end()) {
        DeleteNode(iter->second);
        iter->second->value = value;
        InsertHead(iter->second);
        return;
    }

    if (cache_table_.size() >= capacity_) {
        Node<Key, Value>* free_node = tail_->prev;
        cache_table_.erase(free_node->key);
        DeleteNode(free_node);
        delete free_node;
    }

    Node<Key, Value>* new_node = 
        new Node<Key,Value>(key, value, head_, head_->next);
    cache_table_.insert(std::make_pair(key, new_node));
}

template<typename Key, typename Value>
inline bool LRUCache<Key, Value>::LookUp(Key key, Value * value) {
    typename CacheTable::iterator iter = cache_table_.find(key);
    if (iter != cache_table_.end()) {
        *value = iter->second->value;
        DeleteNode(iter->second);
        InsertHead(iter->second);
        return true;
    }

    return false;
}


#endif // !BASE_LRU_CACHE_H
