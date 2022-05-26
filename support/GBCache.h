#ifndef _GBCache_h
#define _GBCache_h

#include <list>
#include <optional>
#include <tuple>
#include <unordered_map>

template<class K, class V, class Hash>
class GBCache {
  std::size_t capacity;
  std::list<std::pair<K, V>> items;
  std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, Hash> index;
public:
  GBCache(std::size_t capacity) : capacity(capacity) {};
  std::optional<V> get(const K& k) {
    auto itr = index.find(k);
    if (itr == index.end()) return {};
    items.splice(items.begin(), items, itr->second);
    return itr->second->second;
  };
  bool put(const K& k, const V& v) {
    if (index.count(k)) return false;
    if (items.size() == capacity) {
      index.erase(items.back().first);
      items.pop_back();
    }
    items.emplace_front(k, v);
    index.emplace(k, items.begin());
    return true;
  };
};

#endif

