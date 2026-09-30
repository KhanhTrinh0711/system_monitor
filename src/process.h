//
// Created by trinh on 9/29/26.
//

#ifndef SYS_MONITOR_PROCESS_H
#define SYS_MONITOR_PROCESS_H

#include <string>

struct ProcessInfo {
    int pid;
    int ppid;
    std::string name;
    char state;
    double cpu_percent;
    double ram_mb;
    std::string command;
};

int process();

#endif // SYS_MONITOR_PROCESS_H
