#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  std::string key;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long sumIdle, sumNonIdle;
  // Get consistent snapshot of system data
  std::vector<std::string> test(LinuxParser::CpuUtilization());
  std::istringstream linestream(test[0]);
  linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >>
      softirq >> steal >> guest >> guest_nice;

  sumIdle = idle + iowait;
  sumNonIdle = user + nice + system + irq + softirq + steal;

  return static_cast<float>(sumNonIdle) /
         (static_cast<float>(sumIdle) + static_cast<float>(sumNonIdle));
}