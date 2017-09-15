#include "compute-fib.hpp"

//Landon Henrie A01644602
//Assignment 2
//14 Sept 2017

//Returns a double of the nth Fibinacci sequence (1,1,1,2,3,5,...).
double fibNum(int n){
	if (n == 0 || n == 1 || n == 2){
		return 1;
	}
	double nthFib = fibNum(n-1) + fibNum(n-2);
	return nthFib;
}


