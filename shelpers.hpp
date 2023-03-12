#pragma once

#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
//#include <initializer_list> // Added this line

bool splitOnSymbol(std::vector<std::string>& words, int i, char c);
std::vector<std::string> tokenize(const std::string& s);

struct Command{
  std::string exec; //the name of the executable
  //remember argv[0] should be the name of the program (same as exec)
  //Also, argv should end with a nullptr!
  std::vector<const char*> argv; 
  int fdStdin, fdStdout;
  bool background;
};


std::ostream& operator<<(std::ostream& outs, const Command& c);


std::vector<Command> getCommands(const std::vector<std::string>& tokens);
