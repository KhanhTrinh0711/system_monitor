//
// Created by trinh on 8/17/26.
//

#include "memory.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::round;
using std::string;

int memory(MEMINFO &mem_info) {
    ifstream mem_file("/proc/meminfo");
    if (!mem_file.is_open()) {
        cout << "Unable to open memory files" << endl;
        return -1;
    }

    mem_info = {};
    unsigned long long total_kb = 0;
    unsigned long long free_kb = 0;
    unsigned long long cached_kb = 0;
    unsigned long long swap_total_kb = 0;
    unsigned long long swap_free_kb = 0;
    string line;

    while (getline(mem_file, line)) {
        std::istringstream line_stream(line);
        string key;
        unsigned long long value;
        string unit;
        if (!(line_stream >> key >> value >> unit)) {
            continue;
        }

        if (key == "MemTotal:") {
            total_kb = value;
            mem_info.MemTotal = line;
        } else if (key == "MemFree:") {
            free_kb = value;
            mem_info.MemFree = line;
        } else if (key == "Cached:") {
            cached_kb = value;
            mem_info.Cached = line;
        } else if (key == "SwapTotal:") {
            swap_total_kb = value;
            mem_info.SwapTotal = line;
        } else if (key == "SwapFree:") {
            swap_free_kb = value;
            mem_info.SwapFree = line;
        }
    }

    if (mem_info.MemTotal.empty() || mem_info.MemFree.empty() ||
        mem_info.Cached.empty() || mem_info.SwapTotal.empty() ||
        mem_info.SwapFree.empty()) {
        cout << "Unable to read required memory information from /proc/meminfo" << endl;
        return -1;
    }

    const auto to_gigabytes = [](unsigned long long kilobytes) {
        return round((kilobytes / (1024.0 * 1024.0)) * 10.0) / 10.0;
    };

    cout << "==================== MEMORY INFORMATION ===================" << endl;
    cout << std::left << std::setw(10) << "RAM" << ": " << to_gigabytes(total_kb) << " Gb" << endl;
    cout << std::left << std::setw(10) << "FreeRAM" << ": " << to_gigabytes(free_kb) << " Gb" << endl;
    cout << std::left << std::setw(10) << "Cached" << ": " << to_gigabytes(cached_kb) << " Gb" << endl;
    cout << std::left << std::setw(10) << "Swap" << ": " << to_gigabytes(swap_total_kb) << " Gb" << endl;
    cout << std::left << std::setw(10) << "FreeSwap" << ": " << to_gigabytes(swap_free_kb) << " Gb" << endl;
    cout << "===========================================================" << endl;

    return 0;
}
