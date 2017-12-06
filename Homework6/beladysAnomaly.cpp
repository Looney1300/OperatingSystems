#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sstream>

#include "ThreadSafeQ.hpp"
#include "ThreadSafeHash.hpp"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <vector>
#include <thread>
#include <random>
#include <ctime>

//Generates a random sequence of integers [lowerLimit, upperLimit), 
// that is non-inclusively for the upperlimit, but inclusive of the lowerlimit.
void randomSequenceOfInts(int list[], int size, int lowerLimit, int upperLimit){
	for (int i=0; i < size; ++i){
		list[i] = rand()%(upperLimit-lowerLimit) + lowerLimit;
	}
}

//Checks if in memory and if not processes a page fault be swapping
// value with the memory that has been in memory the longest.
bool isInMemory(int value, int memory[], int size){
	static int whichMemToReplace = 0;
	for (int i=0; i<size; ++i){
		if(memory[i] == value){
			return true;
		}
	}
	memory[whichMemToReplace%size] = value;
	++whichMemToReplace;
	return false;
}

//Return the number of times the anomaly is detected in the given
// sequence using a FIFO queue/replacement skeme.
// It returns a string, conataining details of the results, as follows:
//   Page Faults: 628 @ Frame Size: 88
//   Page Faults: 631 @ Frame Size: 89
// If no anomaly is detected, an empty string is returned.
std::string beladysAnomaly(int lengthOfSequence, int numOfPages, int maxFramesInMem){
	std::stringstream resultss("");
	int sequence[lengthOfSequence] = {};
	randomSequenceOfInts(sequence, lengthOfSequence, 1, numOfPages+1);
	
	//Outer for loop is for increasing the number of pages in memory.
	int pageFaults0 = 0;
	for (int x=1; x<=maxFramesInMem; ++x){
		int pageFaults1 = 0;
		//Next for loop is for going through each item in the sequence.
		int memory[x]={};
		for (int i=0; i<lengthOfSequence; ++i){
			if (!isInMemory(sequence[i], memory, x)){
				++pageFaults1;
			}
		}
		if (pageFaults1 > pageFaults0 && pageFaults0 > 0){
			resultss << "\tPage Faults: " << pageFaults0 << " @ Frame Size: " << (x-1) << "\n";
			resultss << "\tPage Faults: " << pageFaults1 << " @ Frame Size: " << x << "\n";
		}
		pageFaults0 = pageFaults1;
	}
	return resultss.str();
}

void worker(ThreadSafeHash<int, std::string>* piMap, ThreadSafeQ<int>* q){
	const int MAXFRAMES = 100;
	const int SEQUENCESIZE = 1000;
	const int PAGESINPROCESS = 250;
	int job;
	while(q->getNextJob(job)){
		piMap->insertPair(job, beladysAnomaly(SEQUENCESIZE, PAGESINPROCESS, MAXFRAMES));
	}
}

int main(){
	std::srand(time(NULL));
	const int MAXFRAMES = 100;
	const int SEQUENCESIZE = 1000;
	const int PAGESINPROCESS = 250;
	const int NUMOFSAMPLES = 100;
	
	std::vector<int> jobs;
	
	for (int i=1; i <= NUMOFSAMPLES; ++i){
		jobs.push_back(i);
	}

	ThreadSafeQ<int>* thSfQ = new ThreadSafeQ<int>(jobs);
	ThreadSafeHash<int, std::string>* piMap = new ThreadSafeHash<int, std::string>();
	std::vector<std::thread*> threads;
	//worker(piMap, thSfQ);
	
	for (int i=0; i<std::thread::hardware_concurrency(); ++i){
		threads.push_back(new std::thread(worker, piMap, thSfQ));
	}
	for (std::thread* thread: threads){
		thread->join();
	}
	std::cout<<"\nLength of memory reference strings: "<<SEQUENCESIZE<<std::endl
		<<"Frames of physical memory: "<<MAXFRAMES<<std::endl<<std::endl;
	
	int count = 0;
	for (int i : jobs){
		std::string result = piMap->getValue(i);
		if (result != ""){
			++count;
			std::cout<<"Belady's Anomaly Found!\nSequence: "<<i<<std::endl;
			std::cout<<result;
		}	
			
	}
	std::cout<<"Number of times Belady's Anomaly was detected: "<<count<<std::endl;

	return 0;
}
