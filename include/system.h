#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   // TODO: See src/system.cpp
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float MemoryUtilization();          // See src/system.cpp
  long UpTime();                      // See src/system.cpp
  int TotalProcesses();               // TODO: See src/system.cpp
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();               // Done: See src/system.cpp
  std::string OperatingSystem();      // Done: See src/system.cpp

  // Define any necessary private members
 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string os_ = {};
  std::string kernel_{};
};

#endif