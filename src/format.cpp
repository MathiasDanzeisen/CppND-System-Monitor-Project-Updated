
// for debugging
#include <iostream>

//
#include <chrono>
#include <iomanip>
#include <string>

#include "format.h"

using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::chrono::seconds sec{seconds};
  std::stringstream outstr;

  outstr << std::setfill('0') << std::setw(2)
         << std::chrono::duration_cast<std::chrono::hours>(sec).count() << ":"
         << std::setfill('0') << std::setw(2)
         << std::chrono::duration_cast<std::chrono::minutes>(sec).count() % 60
         << ":" << std::setfill('0') << std::setw(2) << sec.count() % 60
         << std::endl;
  return outstr.str();
}