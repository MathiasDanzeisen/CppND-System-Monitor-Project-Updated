#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

// debugging
#include <iostream>>

// Constrcutor
//  * since init of private member is not done coherent, first update of system
//  monitor might be inaccurate
// * Sutract one to avoid division by zero
Processor::Processor()
    : prevIdle{LinuxParser::IdleJiffies() - 1},
      prevNonIdle{LinuxParser::ActiveJiffies() - 1} {}

// Return the aggregate CPU utilization
//  return: as relative value, intervall betwenn this call and last call of the method
float Processor::Utilization() {
  std::string key;
  float utilization;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long sumIdle, sumNonIdle, deltaIdle, deltaNonIdle;
  // Get consistent snapshot of system data
  std::vector<std::string> test(LinuxParser::CpuUtilization());
  std::istringstream linestream(test[0]);
  linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >>
      softirq >> steal >> guest >> guest_nice;

  // absolute values in tickes/jffies since the system of the system
  sumIdle = idle + iowait;
  sumNonIdle = user + nice + system + irq + softirq + steal;

  // calc delta since last call of the method
  deltaNonIdle = sumNonIdle - this->prevNonIdle;
  deltaIdle = sumIdle - this->prevIdle;

  this->prevIdle = sumIdle;
  this->prevNonIdle = sumNonIdle;

  return static_cast<float>(deltaNonIdle) /
         (static_cast<float>(deltaIdle) + static_cast<float>(deltaNonIdle));
}