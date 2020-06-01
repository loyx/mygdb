//
// Created by loyx on 2020/5/30.
//

#include "regs.h"

/*
 * On i386, pt_regs and user_regs_struct are the same,
 * but on 64 bit x86, user_regs_struct has six more fields:
 * fs_base, gs_base, ds, es, fs, gs.
 * PTRACE_GETREGS fills them too, so struct pt_regs would overflow.
 */


#define x86_64_regs reg->x86_64_r
#define i386_regs reg->i386_r
static struct iovec x86_io;

x86_reg REG_newRegs(){
    x86_reg reg = (x86_reg)alloca(sizeof(union x86_regs_union_));
    x86_io.iov_base = reg;
    return reg;
}

static long get_regs_error;
void REG_getRegs(x86_reg reg, pid_t pid){
    if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)) {
        /*x86_io.iov_base = &x86_regs_union; - already is */
        x86_io.iov_len = sizeof(*reg);
        get_regs_error = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &x86_io);
    } else {
        /* Use old method, with heuristical detection of 32-bitness */
        x86_io.iov_len = sizeof(x86_64_regs);
        get_regs_error = ptrace(PTRACE_GETREGS, pid, NULL, &x86_64_regs);
        if (!get_regs_error && x86_64_regs.cs == 0x23) {
            x86_io.iov_len = sizeof(i386_regs);
            /*
             * The order is important: i386_regs and x86_64_regs
             * are overlaid in memory!
             */
            i386_regs.ebx = x86_64_regs.rbx;
            i386_regs.ecx = x86_64_regs.rcx;
            i386_regs.edx = x86_64_regs.rdx;
            i386_regs.esi = x86_64_regs.rsi;
            i386_regs.edi = x86_64_regs.rdi;
            i386_regs.ebp = x86_64_regs.rbp;
            i386_regs.eax = x86_64_regs.rax;
            /*i386_regs.xds = x86_64_regs.ds; unused by strace */
            /*i386_regs.xes = x86_64_regs.es; ditto... */
            /*i386_regs.xfs = x86_64_regs.fs;*/
            /*i386_regs.xgs = x86_64_regs.gs;*/
            i386_regs.orig_eax = x86_64_regs.orig_rax;
            i386_regs.eip = x86_64_regs.rip;
            /*i386_regs.xcs = x86_64_regs.cs;*/
            /*i386_regs.eflags = x86_64_regs.eflags;*/
            i386_regs.esp = x86_64_regs.rsp;
            /*i386_regs.xss = x86_64_regs.ss;*/
        }
    }
    if (get_regs_error < 0) {
        printf("get_regs failed, errno=%s", strerror(errno));
    }
}
void REG_setRegs(x86_reg reg, pid_t pid){
    if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)) {
        ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &x86_io);
    } else {
        ptrace(PTRACE_SETREGS, pid, NULL, &x86_64_regs);
    }
}

const int long_size = sizeof(long);
void REG_getData(pid_t pid, u_int64_t addr, char* str, int len){
    char *laddr;
    int i, j;
    union u {
        long val;
        char chars[long_size];
    }data;
    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * 4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, pid, addr + i * 4, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}
