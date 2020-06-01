//
// Created by loyx on 2020/5/30.
//

#ifndef DEBUGER_REGS_H
#define DEBUGER_REGS_H


#include <elf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/personality.h>
#include <linux/version.h>

typedef union x86_regs_union_* x86_reg;
struct i386_user_regs_struct {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    uint32_t xds;
    uint32_t xes;
    uint32_t xfs;
    uint32_t xgs;
    uint32_t orig_eax;
    uint32_t eip;
    uint32_t xcs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t xss;
};

union x86_regs_union_{
    struct user_regs_struct x86_64_r;
    struct i386_user_regs_struct i386_r;
};

x86_reg REG_newRegs();
void REG_getRegs(x86_reg reg, pid_t pid);
void REG_setRegs(x86_reg reg, pid_t pid);
void REG_getData(pid_t pid, u_int64_t addr, char* str, int len);


#endif //DEBUGER_REGS_H
