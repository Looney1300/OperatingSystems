#ifndef THREADSAFEQ_HPP
#define THREADSAFEQ_HPP

#include <mutex>
#include <vector>
#include <queue>

class ThreadSafeQ{
	public:
		ThreadSafeQ();
		ThreadSafeQ(std::vector<int>& jobs);
		void enqueJobs(std::vector<int>& jobs);
		bool getNextJob(int& job);
	private:
		std::mutex mutex;
		std::queue<int> q;
};

ThreadSafeQ::ThreadSafeQ(){

}

//Initializes the queue with all jobs in vector.
ThreadSafeQ::ThreadSafeQ(std::vector<int>& jobs){
	enqueJobs(jobs);
}

void ThreadSafeQ::enqueJobs(std::vector<int>& jobs){
	for (int job: jobs){
		q.push(job);
	}
}	

//This will return true if there is a nextJob, and uses the first parameter
// called job to store the job in for use.
bool ThreadSafeQ::getNextJob(int& job){
	std::lock_guard<std::mutex> lock(mutex);
	if (!q.empty()){
		job = q.front();
		q.pop();
		return true;
	}
	return false;
}
	

#endif
