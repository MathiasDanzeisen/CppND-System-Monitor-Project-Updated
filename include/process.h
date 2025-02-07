#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int Pid);
  int Pid() const;                         // See src/process.cpp
  std::string User();                      // See src/process.cpp
  std::string Command();                   // See src/process.cpp
  float CpuUtilization() const;            // See src/process.cpp
  std::string Ram();                       // See src/process.cpp
  long int UpTime() const;                 // See src/process.cpp
  bool operator<(Process const& a) const;  // See src/process.cpp

  // Declare any necessary private members
 private:
  int pid_{0};
  std::string cmd_;
  std::string user_;
  long startTime_{0};
};

#endif