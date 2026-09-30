//
// Created by trinh on 9/29/26.
//

#include "process.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

using std::cout;
using std::endl;
using std::ifstream;
using std::getline;
using std::string;

namespace fs = std::filesystem;

struct ProcessSample {
    ProcessInfo info;
    unsigned long long cpu_ticks;
};

bool read_process(const fs::path& process_path, ProcessSample& sample) {
    const string pid_string = process_path.filename().string();
    if (pid_string.empty() ||
        !std::all_of(pid_string.begin(), pid_string.end(), [](unsigned char c) {
            return std::isdigit(c) != 0;
        })) {
        return false;
    }

    std::istringstream pid_stream(pid_string);
    if (!(pid_stream >> sample.info.pid)) {
        return false;
    }

    ifstream stat_file(process_path / "stat");
    string stat_line;
    if (!getline(stat_file, stat_line)) {
        return false;
    }

    const std::size_t name_start = stat_line.find('(');
    const std::size_t name_end = stat_line.rfind(')');
    if (name_start == string::npos || name_end == string::npos || name_end <= name_start) {
        return false;
    }
    sample.info.name = stat_line.substr(name_start + 1, name_end - name_start - 1);

    std::istringstream stat_stream(stat_line.substr(name_end + 2));
    stat_stream >> sample.info.state >> sample.info.ppid;

    unsigned long long ignored;
    for (int field = 5; field <= 13; ++field) {
        if (!(stat_stream >> ignored)) {
            return false;
        }
    }

    unsigned long long user_ticks;
    unsigned long long system_ticks;
    if (!(stat_stream >> user_ticks >> system_ticks)) {
        return false;
    }
    sample.cpu_ticks = user_ticks + system_ticks;

    for (int field = 16; field <= 23; ++field) {
        if (!(stat_stream >> ignored)) {
            return false;
        }
    }

    long long resident_pages;
    if (!(stat_stream >> resident_pages)) {
        return false;
    }

    const long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0 && resident_pages > 0) {
        sample.info.ram_mb = static_cast<double>(resident_pages) * page_size / (1024.0 * 1024.0);
    }

    ifstream cmdline_file(process_path / "cmdline");
    sample.info.command.assign(std::istreambuf_iterator<char>(cmdline_file),
                               std::istreambuf_iterator<char>());
    std::replace(sample.info.command.begin(), sample.info.command.end(), '\0', ' ');

    const std::size_t command_start = sample.info.command.find_first_not_of(" \t\r\n");
    if (command_start == string::npos) {
        sample.info.command = "[" + sample.info.name + "]";
    } else {
        sample.info.command = sample.info.command.substr(command_start);
    }

    return true;
}

std::map<int, ProcessSample> read_processes() {
    std::map<int, ProcessSample> processes;
    std::error_code error;
    fs::directory_iterator directory("/proc", error);
    const fs::directory_iterator end;

    while (!error && directory != end) {
        ProcessSample sample{};
        if (read_process(directory->path(), sample)) {
            processes[sample.info.pid] = sample;
        }
        directory.increment(error);
    }

    return processes;
}

unsigned long long read_total_cpu_ticks() {
    ifstream stat_file("/proc/stat");
    string line;
    if (!getline(stat_file, line)) {
        return 0;
    }

    std::istringstream stat_stream(line);
    string cpu_label;
    stat_stream >> cpu_label;

    unsigned long long total_ticks = 0;
    unsigned long long value;
    while (stat_stream >> value) {
        total_ticks += value;
    }
    return total_ticks;
}

int process() {
    std::error_code error;
    if (!fs::exists("/proc", error) || error) {
        cout << "Unable to access /proc" << endl;
        return -1;
    }

    const std::map<int, ProcessSample> previous_processes = read_processes();
    const unsigned long long previous_total_ticks = read_total_cpu_ticks();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    const unsigned long long current_total_ticks = read_total_cpu_ticks();
    std::map<int, ProcessSample> current_processes = read_processes();
    const unsigned long long total_ticks = current_total_ticks >= previous_total_ticks
        ? current_total_ticks - previous_total_ticks
        : 0;

    cout << "============================================ PROCESS INFORMATION =========================================" << endl;
    cout << std::left << std::setw(8) << "PID"
         << std::setw(20) << "NAME"
         << std::setw(8) << "STATE"
         << std::setw(8) << "PPID"
         << std::setw(9) << "CPU%"
         << std::setw(10) << "RAM(MB)"
         << "COMMAND" << endl;
    cout << "==========================================================================================================" << endl;

    for (auto& [pid, sample] : current_processes) {
        const auto previous = previous_processes.find(pid);
        if (previous != previous_processes.end() &&
            total_ticks > 0 &&
            sample.cpu_ticks >= previous->second.cpu_ticks) {
            sample.info.cpu_percent =
                static_cast<double>(sample.cpu_ticks - previous->second.cpu_ticks) /
                static_cast<double>(total_ticks) * 100.0;
        }

        const string name = sample.info.name.size() > 19
            ? sample.info.name.substr(0, 16) + "..."
            : sample.info.name;
        const string command = sample.info.command.size() > 60
            ? sample.info.command.substr(0, 57) + "..."
            : sample.info.command;
        std::ostringstream cpu_text;
        cpu_text << std::fixed << std::setprecision(1) << sample.info.cpu_percent << "%";
        std::ostringstream ram_text;
        ram_text << std::fixed << std::setprecision(1) << sample.info.ram_mb;

        cout << std::left << std::setw(8) << sample.info.pid
             << std::setw(20) << name
             << std::setw(8) << sample.info.state
             << std::setw(8) << sample.info.ppid
             << std::setw(9) << cpu_text.str()
             << std::setw(10) << ram_text.str()
             << command << endl;
    }

    return 0;
}
