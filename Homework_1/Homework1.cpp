//Landon Henrie A01644602
//Assignment 1
//5 Sept 2017

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
//Returns a string of the nth Fibinacci sequence (1,1,1,2,3,5,...).
double fibNum(int n){
	if (n == 0 || n == 1 || n == 2){
		return 1;
	}
	double nthFib = fibNum(n-1) + fibNum(n-2);
	return nthFib;
}
//Will output a help menu on how to operate the program to the console.
void dispHelpMenu(){
	std::cout << "--- Assign 1 Help ---\n"
	<< "-fib [n] : Compute the fibonnacci of [n]\n"
	<< "-e [n] : Compute the value of 'e' using [n] iterations\n"
	<< "-pi [n] : Compute Pi to [n] digits\n";
}
//Error Handling std::stoi if not an integer.
int stringToInt(std::string integer){
	for (char s : integer){
		if (s != '0' && s != '1' && s != '2' && s != '3' && s != '4' && s != '5' && s != '6' && s != '7' && s != '8' && s != '9'){
			std::string errorMsg = "Could not convert [n] parameter to integer.";
			throw errorMsg;
		}
	}
	std::string strInt = integer;
	return std::stoi(strInt);
}
//For throwing error if number of parameters given is not correct/workable.
void rightArgs(){
	std::ostringstream ostrstr("");
	ostrstr << "Not workable number of parameters.";
	dispHelpMenu();
	std::string errorMsg = ostrstr.str();
	throw errorMsg;
	return;
}

//Approximates Pi, e, and calculates the Fibinacci sequence accoridng to user input when running the program from the console.
int main(int argc, char* argv[]){
	if (!(argc > 1)){
		dispHelpMenu();
		return 0;
	}

	for (int param = 1; param < argc ; param += 2){
		try {
			std::string argvStr = argv[param];
			if (argc - 1 == param){
				rightArgs();
			}
			std::string argvStr1 = argv[param + 1];
			int n = stringToInt(argvStr1);
			if (argvStr == "-fib"){
				std::cout << fibNum(n) << std::endl;
			}else if (argvStr == "-e"){
				std::cout << eApprox(n) << std::endl;
			}else if (argvStr == "-pi"){
				std::cout << piAccTo(n) << std::endl;
			}else{
				dispHelpMenu();
			}
		}catch (std::string errorMsg){
			std::cout << errorMsg << std::endl;
		}
	}

	return 0;
}
