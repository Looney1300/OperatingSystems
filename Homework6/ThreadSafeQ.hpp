#ifndef THREADSAFEQ_HPP
#define THREADSAFEQ_HPP

#include <mutex>
#include <vector>
#include <queue>

template<typename T>
class ThreadSafeQ{
	public:
		ThreadSafeQ();
		ThreadSafeQ(std::vector<T>& jobs);
		void enqueJobs(std::vector<T>& jobs);
		bool getNextJob(T& job);
	private:
		std::mutex mutex;
		std::queue<T> q;
};

template<typename T>
ThreadSafeQ<T>::ThreadSafeQ(){

}

//Initializes the queue with all jobs in vector.
template<typename T>
ThreadSafeQ<T>::ThreadSafeQ(std::vector<T>& jobs){
	enqueJobs(jobs);
}

template<typename T>
void ThreadSafeQ<T>::enqueJobs(std::vector<T>& jobs){
	for (T job: jobs){
		q.push(job);
	}
}	

//This will return true if there is a nextJob, and uses the first parameter
// called job to store the job in for use and alteration.
template<typename T>
bool ThreadSafeQ<T>::getNextJob(T& job){
	std::lock_guard<std::mutex> lock(mutex);
	if (!q.empty()){
		job = q.front();
		q.pop();
		return true;
	}
	return false;
}
	

#endif
