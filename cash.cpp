#include <iostream>
#include <sched.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <cstdlib>
#include <limits.h>
#include <csignal>
using namespace std;

void changeDirectory(const vector<string>& args){
  if(args.size() > 2){
    cerr << "Too many arguments \n";
    return;
  };
  const char* path = (args.size() > 1) ? args[1].c_str() : getenv("~");
  if(path == nullptr){
    cerr << "cd: Home not set \n";
  } else if(chdir(path) != 0){
    perror("cd failed");
    };
};

string printWorkingDirectory(){
  char buff[PATH_MAX];
  if(getcwd(buff, sizeof(buff)) != nullptr){
    return string(buff);
  }
  return "unknown";
}

int main(){
  
  string input;
  signal(SIGINT, SIG_IGN);

  while(true){
    cout << "$" << printWorkingDirectory() << "> ";
    if(!getline(cin, input)){
      cout << endl;
      break;
    };


    //stringstream work here:
    stringstream ss(input);
    string token;
    vector<string> tokens;

    //pushback token string to the vector till the end of the line
    while(ss >> token){
      tokens.push_back(token);
    };

    if(tokens.empty()){
      continue;
    };
    if(tokens[0] == "cd"){
      changeDirectory(tokens);
      continue;
    };

    if(tokens[0]== "exit"){
      break;
    };


    //handle external commands and parent/child process:
    pid_t pid = fork(); //this creates a vector named c_args, which holds the memory address of chars in c_args
      //as C stores strings as individual chars and execvp uses C instead of C++

    if(pid < 0){
      perror("fork failed");
    } else if(pid == 0){
      signal(SIGINT, SIG_DFL);
      vector<char*> c_args;
      for(auto& arg : tokens){
        c_args.push_back(const_cast<char*>(arg.c_str()));
      };
      c_args.push_back(nullptr);

      execvp(c_args[0], c_args.data());
      perror("Command not found");
      _exit(1);
    } else{
      int status;
      waitpid(pid, &status, 0);
    };     
  };
  return 0;
};
