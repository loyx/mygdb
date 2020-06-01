cmake_minimum_required(VERSION 3.10)
project(debuger C)

set(CMAKE_C_STANDARD 99)

add_executable(debuger main.c regs.c regs.h debuger.c debuger.h BPtable.c BPtable.h)