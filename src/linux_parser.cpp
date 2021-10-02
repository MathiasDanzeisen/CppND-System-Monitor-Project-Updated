#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

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
// return uptime of the system in seconds
long LinuxParser::UpTime() {
  string line;
  float uptimesec;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimesec;
  }
  return static_cast<long>(uptimesec);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  int pid_;
  std::string comm;
  char state;
  int ppid, pgrp, session, tty_nr, tpgid, flags;
  long minflt, cminflt, majflt, cmajflt;
  long utime, stime, cutime, cstime;

  string cmd;
  string line;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pid_ >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime;
  }

  return utime + stime;
}

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

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmd;
  string line;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) +
                       kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
  }
  return cmd;
}

// Read and return the memory used by a process
//  return: the memory in kB
int LinuxParser::Ram(int pid) {
  string line;
  string key;
  int value, memory;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          memory = value;
          break;
        }
      }
    }
  }

  return memory;
}

// Read and return the user ID associated with a process
int LinuxParser::Uid(int pid) {
  string line;
  string key;
  int value, uid;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
          break;
        }
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  int uid;
  int value;
  string line;
  string ret_user, user, key;

  uid = Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> key >> value) {
        if (value == uid) {
          ret_user = user;
        }
      }
    }
  }
  return ret_user;
}

// Read and return the start of a process
// return: start time in seconds after the system boot
long LinuxParser::StartTime(int pid) {
  // see: https://man7.org/linux/man-pages/man5/proc.5.html
  int pid_;
  std::string comm;
  char state;
  int ppid, pgrp, session, tty_nr, tpgid, flags;
  long minflt, cminflt, majflt, cmajflt;
  long utime, stime, cutime, cstime, priority, nice, num_threads, itrealvalue,
      starttime;

  string cmd;
  string line;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pid_ >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime >> priority >> nice >> num_threads >>
        itrealvalue >> starttime;
  }

  return starttime / sysconf(_SC_CLK_TCK);
}
// Read and return the uptime of a process
// return: start time in seconds after the system boot
long LinuxParser::UpTime(int pid) {
  return LinuxParser::UpTime() - LinuxParser::StartTime(pid);
}