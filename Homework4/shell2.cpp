#include<vector>
#include<chrono>
#include<sys/wait.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<string>
#include<string.h>
#include<stdio.h>
#include<sstream>
#include<regex>
#include<signal.h>
#include<functional>

//this is the actual command prompt and returns the command entered
std::string commandPrompt(){
	std::string input = "";
	std::cout<<"["<<get_current_dir_name()<<":] ";
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
void checkIfBlank(const std::string& command){
	std::regex re("\\s*");
	if (std::regex_match(command, re)){
		std::string error = "";
		throw error;
	}
}

//Runs nth command in cmdHistory; selection made should be argv[1].
void runNthCmdInHistory(const std::vector<std::string>& cmdHistory, char**& argv, std::string& command){
	std::string error = "Invalid command history selection.";
	//std::regex re("\\d");
	//for (int i = 0; i < sizeof(argv[1]); ++i){
		//if (!std::regex_match(argv[0][i], re)){
			//throw error;
		//}
	//}
	
	//Conditions for throwing errors
	int nth = 0;
	if (!(nth = std::stoi(argv[1]))){
		throw error;
	}
	if (nth >= cmdHistory.size()){
		throw error;
	}
	
	if (nth > 0 ){
		parseCommand(cmdHistory[nth-1], argv);
	}else{
		throw error;
	}
	command = cmdHistory[nth-1];
	if (*argv[0] == '^'){
		runNthCmdInHistory(cmdHistory, argv, command);
	}
}

//Handles a signal by printing a blank line.
void signalHandler(const int signalId){
	return;
}

//Pipe function to allow piping between two programs.
void pipe(const std::string command, std::chrono::duration<double>& totalTime){
	char** argv1;
	char** argv2;

	//Getting two char** argv's from the one already made, 
	//splitting it by the '|' character/argument in argv.
	std::string command1 = "";
	std::string command2 = "";
	std::stringstream ss;
	ss.str(command);
	getline(ss, command1, '|');
	getline(ss, command2);
	parseCommand(command1, argv1);
	parseCommand(command2, argv2);
	
	//Constants setup
	const int PIPE_COUNT = 2;
	const int PIPE_READ_END = 0;
	const int PIPE_WRITE_END = 1;
	const int STDIN = 0;
	const int STDOUT = 1;
	
	//Pipe creation
	int pids[PIPE_COUNT];
	pipe(pids);
	
	//Duplicate stdout and stdin to use to connect the 
	//ends back together later
	int savedStdout = dup(STDOUT);
	int savedStdin = dup(STDIN);
	
	//First process: put the pipe's write end in place of stdout.
	pid_t pid1 = fork();
	if (pid1 == 0){
		dup2(pids[PIPE_WRITE_END], STDOUT);
		runAndTimeChildProcess(argv1[0], argv1, totalTime);
		exit(111);
	}
	//Second process: put the pipe's read end in place of stdin, 
	//and close the pipe's write end to tell 2nd process no more 
	//data is coming for it to read.
	pid_t pid2 = fork();
	if (pid2 == 0){
		dup2(pids[PIPE_READ_END], STDIN);
		close(pids[PIPE_WRITE_END]);
		runAndTimeChildProcess(argv2[0], argv2, totalTime);
		exit(111);
	}
	int status;
	waitpid(pid1, &status, 0);
	close(pids[PIPE_WRITE_END]);
	close(pids[PIPE_READ_END]);
	waitpid(pid2, &status, 0);
	
	dup2(savedStdout, STDOUT);
	dup2(savedStdin, STDIN);

}

//returns true if pipe character is in the command.
bool isPipeCmd(std::string command){
	for (char i : command){
		if (i == '|') return true;
	}
	return false;
}

//Changes working directory to directory given it.
void changeDir(char**& argv){
	//std::stringstream ss;
	//ss<<get_current_dir_name()
	//char directory[] = get_current_dir_name() + argv[1];
	int ret = chdir(argv[1]);
	if(ret == -1){
		throw "No such directory.";
	}
}

int main(){
	
	//Signal handling:
	signal(SIGINT, signalHandler);
	
	char** argv;
	int numOfArgs = 0;
	std::vector<std::string> cmdHistory = {};
	std::string command = "";
	std::chrono::duration<double> totalTimeInChildProcesses(0);
	
	char ptime[] = "ptime";
	char carat[] = "^";
	char pwd[] = "pwd";
	char history[] = "history";
	char cd[] = "cd";
	
	while ((command=commandPrompt()) != "exit"){
		//Try if there are things in the command.
		try{
			checkIfBlank(command);
			//Store every Command.
			storeCommand(cmdHistory, command);
			//Parse the commands
			numOfArgs = parseCommand(command, argv);
			
			if (strcmp(carat, argv[0]) == 0){
				if (numOfArgs == 3){
					runNthCmdInHistory(cmdHistory, argv, command);
				}else{
					std::string error = "Error: incorrect number of arguments for '^ <history item>'.";
					throw error;
				}
			}
			if (strcmp(ptime, argv[0]) == 0){
				std::cout<<"Time spent executing child processes: " << totalTimeInChildProcesses.count() <<"seconds\n";
			}else if (strcmp(history, argv[0]) == 0){
				listHistory(cmdHistory);
			}else if(strcmp(cd, argv[0]) == 0){
				if (numOfArgs == 3){
					changeDir(argv);
				}else{
					std::string error = "Error: incorrect number of arguments for 'cd <directory>'.";
					throw error;
				}
			}else if(strcmp(pwd, argv[0]) == 0){
				std::cout<<get_current_dir_name()<<std::endl;
			}else if(isPipeCmd(command)){
				pipe(command, totalTimeInChildProcesses);	
			}else{
				runAndTimeChildProcess(argv[0], argv, totalTimeInChildProcesses);
			}
		}catch(std::string e){
			std::cout<<e<<std::endl;
		}
	}
	return 0;
}
