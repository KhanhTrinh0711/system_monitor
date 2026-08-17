//
// Created by trinh on 8/17/26.
//

#include "memory.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <chrono>

using std::string;
using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::stod;

ifstream file("/proc/meminfo");