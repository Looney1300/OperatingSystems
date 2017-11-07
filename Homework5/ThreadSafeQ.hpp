#ifndef THREADSAFEQ_HPP
#define THREADSAFEQ_HPP

class ThreadSafeQ{
	public:
		ThreadSafeQ(std::vector<int> jobs);
		getNextJob(int& job);
	private:
		std::queue<int> q;
};

//Initializes the queue with all jobs in vector.
ThreadSafeQ::ThreadSafeQ(std::vector<int> jobs){
	for (int job: jobs){
		q.push(job);
	}
}

//This will return true if there is a nextJob, and uses the first parameter
// called job to store the job in for use.
bool ThreadSafeQ::getNextJob(int& job){
	if (q.size() == 0){
		return false;
	}
	//Mutex here
	job = queue.pop();
	return true;
}
	

#endif
