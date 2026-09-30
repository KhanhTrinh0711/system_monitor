# SYSTEM MONITOR

## Descriptions

A simplify version of Linux htop, take simple snapshot of CPU, Memory and Process information and print out in a simple CLI.

An exercise on how to interact with /proc/ and how the LINUX system stored system data and how htop work.

**UBUNTU LINUX**

### Project structure

    CPU:
        Get CPU information from /proc/cpuinfo, calculate CPU usage with user, nice, system, idle, 
        iowait, irq, softirq and steal.
        CPU temp from /sys/class/thermal/thermal_zone6/temp (main zone)

    Memory:
        Data stored in /proc/meminfo.

    Process:
        Select only directory that is a string of number which equivalent to a PID, print out 
        necessary information from .../status only


#### How to use

    build:
        cmake ..
        cmake --build .

    run application:
        ./sys-monitor
            1. CPU information
            2. Memory information
            3. Process list
            q. Quit