#include "process.h"

#include <unistd.h>

#include <cctype>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid; }

// Return this process's ID
int Process::Pid() const { return this->pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  long activetime_s =
      LinuxParser::ActiveJiffies(this->Pid()) / sysconf(_SC_CLK_TCK);

  return static_cast<float>(activetime_s) / static_cast<float>(this->UpTime());
}

// Return the command that generated this process
string Process::Command() {
  if (cmd_.empty()) {
    cmd_ = LinuxParser::Command(this->Pid());
  }
  return this->cmd_;
}

// Return this process's memory utilization
string Process::Ram() {
  return to_string(LinuxParser::Ram(this->Pid()) / 1000);
}

// Return the user (name) that generated this process
string Process::User() {
  if (user_.empty()) {
    user_ = LinuxParser::User(this->Pid());
  }

  return user_;
}

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(this->Pid()); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (a.CpuUtilization()) < this->CpuUtilization();
}
