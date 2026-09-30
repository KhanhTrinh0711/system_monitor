//
// Created by trinh on 8/6/26.
//

#include "../src/cpu.h"
#include "../src/memory.h"
#include "../src/process.h"

int main() {
    cpu();
    MEMINFO mem_info;
    memory(mem_info);
    process();
    return 0;
}
