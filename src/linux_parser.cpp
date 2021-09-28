#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>
// for debugging
#include <iostream>

//
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
//  See:
//  https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=34e431b0ae398fc54ea69ff85ec700722c9da773
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  int value;
  int MemTotal{0}, MemAvailable{0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          MemTotal = value;
        }
        if (key == "MemAvailable") {
          MemAvailable = value;
          break;
        }
      }
    }
  }
  return (float(MemAvailable) / float(MemTotal));
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  float uptimesec;
  float idlesec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> idlesec >> uptimesec;
  }
  return static_cast<long>(uptimesec);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// Parse and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::string key;
  // desciption at
  //  https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.rst#18-miscellaneous-kernel-statistics-in-procstat
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }

  // NonIdle = user + nice + system + irq + softirq + steal
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  return user + nice + system + irq + softirq + steal;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::string key;
  // desciption at
  //  https://github.com/torvalds/linux/blob/master/Documentation/filesystems/proc.rst#18-miscellaneous-kernel-statistics-in-procstat
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }

  // Idle = idle + iowait
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  return idle + iowait;
}

// Read and return CPU utilization
//  in case of error: return vector with one empty string
vector<string> LinuxParser::CpuUtilization() {
  vector<string> CpuUtilization;
  string key;
  string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    linestream >> key;
    if (key != "intr") {
      CpuUtilization.push_back(line);
    } else {
      CpuUtilization.push_back("");
      break;
    }
  }
  return CpuUtilization;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value, processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          processes = value;
          break;
        }
      }
    }
  }
  return processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value, procs_running;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          procs_running = value;
          break;
        }
      }
    }
  }
  return procs_running;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
