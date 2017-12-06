#ifndef THREADSAFEHASH_HPP
#define THREADSAFEHASH_HPP

#include <unordered_map>
#include <mutex>

template<typename TKey, typename TValue>
class ThreadSafeHash{
	public:
		ThreadSafeHash(){};
		TValue getValue(TKey key);
		void insertPair(TKey key, TValue value);
	private:
		std::mutex mutex2;
		std::mutex mutex;
		std::unordered_map<TKey, TValue> map;
};

template<typename TKey, typename TValue>
TValue ThreadSafeHash<TKey, TValue>::getValue(TKey key){
	std::lock_guard<std::mutex> lock(mutex);
	auto got = map.find(key);
	if (got == map.end()){
		std::cout<<"\nValue not found.\n";
		TValue v;
		return v;
	}
	return got->second;
}

template<typename TKey, typename TValue>
void ThreadSafeHash<TKey, TValue>::insertPair(TKey key, TValue value){
	std::lock_guard<std::mutex> lock(mutex2);
	std::pair<TKey, TValue> keyValuePair (key, value);
	map.insert(keyValuePair);
}

#endif
