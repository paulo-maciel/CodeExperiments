class LRUCache {
public:
    LRUCache(int capacity) {
        cap = capacity;
        l.resize(cap);
    }
    
    int get(int key) {
        if (cache.count(key) == 0) return -1;
        
        auto iter = cache[key];
        auto value = iter->second;
        l.erase(iter);
        
        cache[key] = l.insert(l.begin(), {key, value});
        
        return value;
    }
    
    void put(int key, int value) {
        // if existing, update.
        if (cache.count(key)) {
            // Update the node in the list
            auto iter = cache[key];
            iter->second = value;
            
            // move node to front by deleting from its current
            // position and moving to front and update cache.
            l.erase(iter);
            cache[key] = l.insert(l.begin(), {key, value});

        } else {
            auto iter = l.insert(l.begin(), {key,value});
            cache[key] = iter;
            // evict
            if (l.size() > cap) {
                auto key = l.back().first;
                l.pop_back();
                cache.erase(key);
            }
        }
        
        //cout << cache[key]->first << "," << cache[key]->second << endl;
    }
    
    unordered_map<int, list<pair<int,int>>::iterator> cache;
    list<pair<int,int>> l;
    int cap;
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */