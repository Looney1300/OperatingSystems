#ifndef THREADSAFEHASH_HPP
#define THREADSAFEHASH_HPP

#include <unordered_map>
#include <mutex>

class ThreadSafeHash{
	public:
		ThreadSafeHash(){};
		int getValue(int key);
		void insertPair(int key, int value);
	private:
		std::mutex mutex2;
		std::mutex mutex;
		std::unordered_map<int, int>* map = new std::unordered_map<int, int>;
};

int ThreadSafeHash::getValue(int key){
	std::lock_guard<std::mutex> lock(mutex);
	auto got = map->find(key);
	if (got == map->end()){
		std::cout<<"\nValue not found.\n";
		return -1;
	}
	return got->second;
}

void ThreadSafeHash::insertPair(int key, int value){
	std::lock_guard<std::mutex> lock(mutex2);
	std::pair<int, int> keyValuePair (key, value);
	map->insert(keyValuePair);
}

#endif
