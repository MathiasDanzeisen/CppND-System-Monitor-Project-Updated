#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid; }

// Return this process's ID
int Process::Pid() const { return this->pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// Return the command that generated this process
string Process::Command() {
  if (cmd_.empty()) {
    cmd_ = LinuxParser::Command(this->Pid());
  }
  return this->cmd_;
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// Return the user (name) that generated this process
string Process::User() {
  if (user_.empty()) {
    user_ = LinuxParser::User(this->Pid());
  }

  return user_;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  //return this->Pid() < (a.Pid());
}