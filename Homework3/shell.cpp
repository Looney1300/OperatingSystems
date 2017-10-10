#include<vector>
#include<chrono>
#include<sys/wait.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<string>
#include<sstream>
#include<regex>

//this is the actual command prompt and returns the command entered
std::string commandPrompt(){
	std::string input = "";
	std::cout<<"[cmd]: ";
	std::getline(std::cin, input);
	return input;
}

//this runs a child process command and tracks the total time the child process is running,
//and accumulates the total time process children are running. Uses a fork() and execvp().
void runAndTimeChildProcess(char* name, char** argv, std::chrono::duration<double>& totalTime){
	auto start = std::chrono::system_clock::now();
	//Run program and time how long parent is waiting
	if(fork()){
		wait(NULL);
	}else{
		execvp(name, argv);	
		std::cout << "No program found named '" << name << "'.\n";
		exit(13);
	}
	auto end = std::chrono::system_clock::now();
	//std::chrono::duration<double> addTime = end-start;
	//add new time to current tally while in children processes
	totalTime += end-start;
}

//this is to send a valid command given into the shell's history
void storeCommand(std::vector<std::string>& prevCmdList, std::string cmd){
	prevCmdList.push_back(cmd);
}

//this prints the shells history as a numbered list
void listHistory(const std::vector<std::string>& prevCmdList){
	std::cout<<" -- Command History --\n\n";
	int x = 0;
	for (std::string cmd : prevCmdList){
		std::cout<<++x<<". "<<cmd<<"\n";
	}
	std::cout<<std::endl;
}

//Parses through the command and compiles a char** array with each char* being each individual 
//argument given in the command line, the first
//assumed to be the name of the program to run.
//Returns size of newly created argv array, and deconstructively constructs the argv array.
int parseCommand(std::string command, char**& argv){
	std::string nthArg;
	std::vector<std::string> args = {};
	std::stringstream cmdStream;
	cmdStream << command;
	while (cmdStream >> nthArg){
		args.push_back(nthArg);
	}
	argv = new char*[args.size()+1];
	for(int i=0; i<args.size(); ++i){
		argv[i] = new char[args[i].size()+1];
		char* argCharPtr = new char[args[i].size()+1];
		strcpy(argv[i], args[i].c_str());
	}
	argv[args.size()] = NULL;
	return args.size()+1;
}

//Checks to make sure at least one argument is given, and throws a std::string error if not.
void checkArgv(const std::string& command){
	std::regex re("\\s*");
	if (std::regex_match(command, re)){
		std::string error = "";
		throw error;
	}
}

//Runs nth command in cmdHistory; selection made should be argv[1].
void runNthCmdInHistory(const std::vector<std::string>& cmdHistory, char**& argv, std::string& command){
	std::string error = "Invalid command history selection.";
	std::regex re("\\d");
	//for (int i = 0; i < sizeof(argv[1]); ++i){
		//if (!std::regex_match(argv[0][i], re)){
			//throw error;
		//}
	//}
	int nth = 0;
	if (!(nth = std::stoi(argv[1]))){
		throw error;
	}
	if (nth > 0 ){
		parseCommand(cmdHistory[nth-1], argv);
	}else{
		throw error;
	}
	command = cmdHistory[nth-1];
}

int main(){
	char** argv;
	int numOfArgs = 0;
	std::vector<std::string> cmdHistory = {};
	std::string command = "";
	std::chrono::duration<double> totalTimeInChildProcesses(0);
	while ((command=commandPrompt()) != "exit"){
		//Try if there are things in the command.
		try{
			checkArgv(command);
			//Store every Command.
			storeCommand(cmdHistory, command);
			//Parse the commands
			numOfArgs = parseCommand(command, argv);
			//TO-DO
			//implement a way to split command into it's several components to pass to runAndTimeChildProcess()
			//and/or strip the white space from each component.
			if (*argv[0] == '^'){
				if (numOfArgs == 3){
					try{
						runNthCmdInHistory(cmdHistory, argv, command);
					}catch(std::string error){
						std::cout<<error<<std::endl;
					}
				}else{
					std::cout<<"Error: incorrect number of arguments for '^ <history item>'.\n";
				}
			}
			if (command == "ptime"){
				std::cout<<"Time spent executing child processes: " << totalTimeInChildProcesses.count() <<"seconds\n";
			}else if (command == "history"){
				listHistory(cmdHistory);
			}else{
				runAndTimeChildProcess(argv[0], argv, totalTimeInChildProcesses);
			}
		}catch(std::string e){
			std::cout<<e<<std::endl;
		}
	}
	return 0;
}
