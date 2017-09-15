#include "compute-e.hpp"

//Landon Henrie A01644602
//Assignment 2
//14 Sept 2017

#include<iostream>
#include<iomanip>

//Returns a string of an approximation of the value of e using a Taylor series of n iterations.
double eApprox(int numOfIterations){
	//Getting output into correct format overriding the defaults for console outupt.
	std::cout << std::fixed << std::setprecision(13);
	double epprox = 1;
	int prevFactorial = 1;
	for (int i = 1; i <= numOfIterations; i++){
		epprox += double(1)/(prevFactorial*i);
		prevFactorial = prevFactorial*i;
	}
	return epprox;
}

