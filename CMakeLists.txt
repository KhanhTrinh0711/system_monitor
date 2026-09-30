cmake_minimum_required(VERSION 3.16)
project(sys-monitor CXX)
add_executable(
	sys-monitor
		core/main.cpp
		src/cpu.cpp
		src/cpu.h
		src/memory.h
		src/memory.cpp
		src/process.cpp
		src/process.h
)
