//
// Created by trinh on 8/6/26.
//

#include "cpu.h"
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

ifstream file("/proc/cpuinfo");

struct CpuStats {
    unsigned long long idle;
    unsigned long long total;
};

CpuStats read_cpu_stats() {
    std::ifstream file("/proc/stat");
    std::string line;
    CpuStats stats{0, 0};

    if (file.is_open() && std::getline(file, line)) {

        if (line.rfind("cpu ", 0) == 0) {
            std::stringstream ss(line);
            std::string label;
            unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

            ss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

            unsigned long long idle_time = idle + iowait;
            unsigned long long non_idle_time = user + nice + system + irq + softirq + steal;

            stats.idle = idle_time;
            stats.total = idle_time + non_idle_time;
        }
    }
    return stats;
}

double cpu_usage() {
    CpuStats prev = read_cpu_stats();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CpuStats curr = read_cpu_stats();

    double total_delta = static_cast<double>(curr.total - prev.total);
    double idle_delta = static_cast<double>(curr.idle - prev.idle);

    if (total_delta == 0) return 0.0;

    double cpu_usage = ((total_delta - idle_delta) / total_delta) * 100.0;
    return cpu_usage;
}

int cpu() {
    ifstream file("/proc/cpuinfo");
    if (!file.is_open()) {
        cout << "Error opening /proc/cpuinfo" << endl;
        return 1;
    }

    string line;
    string model_name = "";
    string cpu_cores = "";
    string cache_size = "";

    while (getline(file, line)) {
        if (model_name.empty() && line.rfind("model name", 0) == 0) {
            model_name = line;
        }
        else if (cpu_cores.empty() && line.rfind("cpu cores", 0) == 0) {
            cpu_cores = line;
        }
        else if (cache_size.empty() && line.rfind("cache size", 0) == 0) {
            cache_size = line;
        }

        if (!model_name.empty() && !cpu_cores.empty() && !cache_size.empty()) {
            break;
        }
    }
    file.close();

    long threads = sysconf(_SC_NPROCESSORS_ONLN);
    double usage = cpu_usage();

    cout << "====================== CPU INFORMATION ====================" << endl;
    cout << model_name << endl;
    cout << cpu_cores << endl;
    cout << "Threads: " << static_cast<int>(threads) << endl;
    cout << cache_size << endl;
    cout << "CPU Usage: " << usage << "%" << endl;
    cout << "===========================================================" << endl;

    return 0;
}