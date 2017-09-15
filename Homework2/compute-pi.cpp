
#include "compute-pi.hpp"

//Landon Henrie A01644602
//Assignment 2
//14 Sept 2017

#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>

//Returns value of Pi as a string accurate to a certain number of digits.
std::string piAccTo(int digitsAcc){
	//Approximating pi using an integer fraction to double then to ostringstream, then to string.
	std::ostringstream sstr("");
	sstr << std::fixed << std::setprecision(digitsAcc);
	sstr << double(2549491779)/811528438;
	std::string strPi = sstr.str();
	//Next ostringstream for getting the correct number of digits of accuracy.
	std::ostringstream endSstr("");
	for (int i = 0; i <= digitsAcc; i++){
		endSstr << strPi[i];
	}
	std::string newPi = endSstr.str();
	return newPi;
}

