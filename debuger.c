//
// Created by loyx on 2020/5/30.
//

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "debuger.h"
#include "BPtable.h"
#include "regs.h"

static void debug(int pid);
void getCodeSegBaseAddr(int pid, char *realpath);
void addBreakPoint(int pid, u_int64_t addr);
void examineMemory(int pid, u_int64_t addr);
void continuePro(int pid);
void quitPro();


void printValue();

static u_int64_t code_start_addr = 0x0;
static u_int64_t code_end_addr = 0x0;
static u_int64_t data_start_addr = 0x0;
static u_int64_t data_end_addr = 0x0;
static breakList BList;
static x86_reg reg;
bool ASLR;


void commonSetting(){
    setvbuf(stdout, NULL, _IONBF, 0);
    personality(personality(0xfffffffff) | ADDR_NO_RANDOMIZE);
    BList = TAB_emptyBreakList();
    reg = REG_newRegs();
}

void getCodeSegBaseAddr(int pid, char *realpath) {
    char maps_path[64];
    snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);

    FILE *fp = fopen(maps_path, "r");
    if (NULL == fp){
        printf("read file(%s) failed\n", maps_path);
        return;
    }

    char *line_ptr;
    char line_contents[512];
    while (1){
        line_ptr = fgets(line_contents, sizeof(line_contents), fp);
        if (NULL == line_ptr){
            break;
        }
        char *last_line_char = strrchr(line_ptr, '\n');
        if (NULL != last_line_char){
            *last_line_char = 0;
        }

        char delim[] = " ";
        char* token;
        bool isExecute = false, isCustomCode = false;

        token = strtok(line_ptr, delim);
        sscanf(token, "%lx-%lx", &code_start_addr, &code_end_addr);

        token = strtok(NULL, delim);
        if (token[2] == 'x') isExecute = true;

        token = strtok(NULL, delim);
        token = strtok(NULL, delim);
        token = strtok(NULL, delim);

        token = strtok(NULL, delim);
        if (strcmp(token, realpath) != 0) isCustomCode = true;

        if (isExecute && isCustomCode) return;
    }
    assert(0);
}

void startTrace(char* filename){
    commonSetting();

    int pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            break;
        case 0:
            ptrace(PTRACE_TRACEME, 0, NULL, NULL);
            execl(filename, filename, NULL);
            break;
        default: {
            wait(NULL);

            if (ASLR){
                char *process_realpath = realpath(filename, NULL);
                getCodeSegBaseAddr(pid, process_realpath);
            }
            debug(pid);
            break;
        }
    }

}

void startAttach(char* file_pid){
    commonSetting();

    int pid = (int)strtol(file_pid, NULL, 10);
    printf("attach pid:%d\n", pid);
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    wait(NULL);

    if (ASLR){
        getCodeSegBaseAddr(pid, NULL);
    }
    debug(pid);
}

void debug(int pid){
    static const int COM_MAX = 255;
    char command[COM_MAX];
    while (1){
        printf("\n====================\n"
               "b addr: add a breakpoint\n"
               "p $rbp: print $rbp\n"
               "x addr: show the memory\n"
               "c: continue\n"
               "q: quit\n"
               "====================\n"
               "mgdb$ ");
        fgets(command, COM_MAX, stdin);
        u_int64_t addr;
        switch (command[0]) {
            case 'b':
                addr = strtoul(command+2, NULL, 16);
                addBreakPoint(pid, addr);
                break;
            case 'x':
                addr = strtoul(command+2, NULL, 16);
                examineMemory(pid, addr);
                break;
            case 'p':
                printValue();
                break;
            case 'c':
                continuePro(pid);
                break;
            case 'q':
                quitPro();
                return;
        }
    }
    assert(0);
}

void printValue() {
//    REG_getRegs(reg, pid);
    printf("$rbp:0x%llx\n", reg->x86_64_r.rbp);
}

void quitPro() {
    // todo quit
    exit(0);
}

void continuePro(int pid) {
    ptrace(PTRACE_CONT, pid, NULL, NULL);

    int wait_val;
    wait(&wait_val);
    if (WIFSTOPPED(wait_val)){
        printf("breakpoint reach\n");
        REG_getRegs(reg, pid);
        reg->x86_64_r.rip -= 1;
        long origin = 0;
        REG_getData(pid, reg->x86_64_r.rip, (char *) &origin, 2);
        REG_setRegs(reg, pid);
        ptrace(PTRACE_POKETEXT, pid, reg->x86_64_r.rip, origin);
    }
}

void examineMemory(int pid, u_int64_t addr) {
    static int e_count = 0;
    addr = data_start_addr + addr;
    printf("examine memory=0x%lx\n", addr);
    char data[8];
    int len = 1;
    REG_getData(pid, addr, data, len);
    printf("(%d): %lx\n", e_count++, *(long*)data);
}

void addBreakPoint(int pid, u_int64_t addr) {
    addr = code_start_addr + addr;
    printf("Breakpoint address=0x%lx\n", addr);
    long origin = ptrace(PTRACE_PEEKTEXT, pid, (void *) addr, NULL);
    long breakpoint = TAB_newBreakpoint(BList, origin);
    ptrace(PTRACE_POKETEXT, pid, (void *)addr, breakpoint);
}

