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

/* uncomment the following line to use 'long long' integers */
//#define HAS_LONG_LONG 

//#ifdef HAS_LONG_LONG
	#define mul_mod(a,b,m) (( (long long) (a) * (long long) (b) ) % (m))
//#else
	//#define mul_mod(a,b,m) std::fmod( (double) a * (double) b, m)
//#endif

/* return the inverse of x mod y */
int inv_mod(int x, int y){
	int q, u, v, a, c, t;

	u = x;
	v = y;
	c = 1;
	a = 0;
	do {
		q = v / u;

		t = c;
		c = a - q * c;
		a = t;

		t = u;
		u = v - q * u;
		v = t;
	} while (u != 0);
	a = a % y;
	if (a < 0)
		a = y + a;
	return a;
}

/* return (a^b) mod m */
int pow_mod(int a, int b, int m){
	int r, aa;

	r = 1;
	aa = a;
	while (1) {
		if (b & 1)
			r = mul_mod(r, aa, m);
		b = b >> 1;
		if (b == 0)
			break;
		aa = mul_mod(aa, aa, m);
	}
	return r;
}

/* return true if n is prime */
int is_prime(int n){
	int r, i;
	if ((n % 2) == 0)
		return 0;

	r = (int)(sqrt(n));
	for (i = 3; i <= r; i += 2)
		if ((n % i) == 0)
			return 0;
	return 1;
}

/* return the prime number immediatly after n */
int next_prime(int n){
	do {
		n++;
	} while (!is_prime(n));
	return n;
}

unsigned int computePiDigit(int n){
	int av, a, vmax, N, num, den, k, kq, kq2, t, v, s, i;
	double sum = 0;

	N = (int)((n + 20) * std::log(10) / std::log(2));

	for (a = 3; a <= (2 * N); a = next_prime(a)) {

		vmax = (int)(std::log(2 * N) / std::log(a));
		av = 1;
		for (i = 0; i < vmax; i++)
			av = av * a;

		s = 0;
		num = 1;
		den = 1;
		v = 0;
		kq = 1;
		kq2 = 1;

		for (k = 1; k <= N; k++) {

			t = k;
			if (kq >= a) {
				do {
					t = t / a;
					v--;
				} while ((t % a) == 0);
				kq = 0;
			}
			kq++;
			num = mul_mod(num, t, av);

			t = (2 * k - 1);
			if (kq2 >= a) {
				if (kq2 == a) {
					do {
						t = t / a;
						v++;
					} while ((t % a) == 0);
				}
				kq2 -= a;
			}
			den = mul_mod(den, t, av);
			kq2 += 2;

			if (v > 0) {
				t = inv_mod(den, av);
				t = mul_mod(t, num, av);
				t = mul_mod(t, k, av);
				for (i = v; i < vmax; i++)
					t = mul_mod(t, a, av);
				s += t;
				if (s >= av)
					s -= av;
			}

		}

		t = pow_mod(10, n - 1, av);
		s = mul_mod(s, t, av);
		sum = std::fmod(sum + (double)s / (double)av, 1.0);
	}

	return static_cast<unsigned int>(sum * 1e9 / 100000000);
}

// ------------------------------------------------------------------
//
// Code adapted from this source: https://web.archive.org/web/20150627225748/http://en.literateprograms.org/Pi_with_the_BBP_formula_%28Python%29
//
// ------------------------------------------------------------------
double powneg(unsigned long long b, long long pow){
	double r = std::pow(b, -pow);
	return 1.0 / r;
}

unsigned long long s(unsigned long long j, unsigned long long n){
	const unsigned long long D = 14;
	const unsigned long long M = static_cast<unsigned long long>(std::pow(16, D));
	const unsigned long long SHIFT = 4 * D;
	const unsigned long long MASK = M - 1;

	unsigned long long s = 0;
	unsigned long long k = 0;
	while (k <= n)
	{
		unsigned long long r = 8 * k + j;
		unsigned long long v = static_cast<unsigned long long>(std::pow(16ul, n - k)) % r;
		s = (s + (v << SHIFT) / r) & MASK;
		k += 1;
	}
	unsigned long long t = 0;
	k = n + 1;
	while (true)
	{
		unsigned long long xp = static_cast<unsigned long long>(powneg(16, n - k) * M);
		unsigned long long newt = t + xp / (8 * k + j);
		if (t == newt)
			break;
		else
			t = newt;
		k += 1;
	}

	return s + t;
}

unsigned long long piDigitHex(unsigned long long n){
	const unsigned long long D = 14;
	const unsigned long long M = static_cast<unsigned long long>(std::pow(16, D));
	const unsigned long long SHIFT = 4 * D;
	const unsigned long long MASK = M - 1;

	n -= 1;
	unsigned long long x = (4 * s(1, n) - 2 * s(4, n) - s(5, n) - s(6, n)) & MASK;

	return x;
}

void worker(ThreadSafeHash* piMap, ThreadSafeQ* q){
	int digit;
	while(q->getNextJob(digit)){
		piMap->insertPair(digit, computePiDigit(digit));
		std::cout<<".";
		std::cout.flush();
	}
}

int main(){
	key_t SHARED_KEY;
	key_t SHARED_KEY1;
	int NUMOFDIGITS=1000;
	
	std::vector<int> jobs;
	for (int i=1; i <= NUMOFDIGITS; ++i){
		jobs.push_back(i);
	}
	
	ThreadSafeHash* map = new ThreadSafeHash();
	int m_id1 = shmget(SHARED_KEY1, sizeof(ThreadSafeHash), 0666|IPC_CREAT);
	ThreadSafeHash* piMap = static_cast<ThreadSafeHash*>(shmat(m_id1, 0, 0));
	piMap = map;
	
	
	ThreadSafeQ* thq = new ThreadSafeQ(jobs);
	int m_id = shmget(SHARED_KEY, sizeof(thq), 0666|IPC_CREAT);
	ThreadSafeQ* thSfQ = static_cast<ThreadSafeQ*>(shmat(m_id, 0, 0));
	
	//thSfQ->enqueJobs(jobs);
	thSfQ = thq;
	
	std::vector<std::thread*> threads;
	for (int i=0; i<std::thread::hardware_concurrency(); ++i){
		threads.push_back(new std::thread(worker, piMap, thSfQ));
	}
	for (std::thread* thread: threads){
		thread->join();
	}
	
	std::string pi = "\n3.";
	std::cout << pi;
	for (int i : jobs){
		std::cout<<piMap->getValue(i);		
	}
	std::cout<<std::endl;

	return 0;
}
