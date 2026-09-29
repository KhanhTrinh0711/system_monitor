//
// Created by trinh on 8/17/26.
//

#ifndef SYS_MONITOR_MEMORY_H
#define SYS_MONITOR_MEMORY_H
#include <string>

using std::string;

struct MEMINFO {
    string MemTotal;
    string MemFree;
    string Cached;
    string SwapTotal;
    string SwapFree;
};

int memory(MEMINFO& mem_info);


#endif //SYS_MONITOR_MEMORY_H
