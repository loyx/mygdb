//
// Created by loyx on 2020/5/31.
//

#include "BPtable.h"
#include <stdlib.h>
#include <assert.h>


static int break_count = 0;

struct breakList_{
    long origin, breakpoint;
    breakList tail;
};

static long newInt3();

breakList BreakList(long origin, long breakpoint, breakList tail){
    breakList list = (breakList)alloca(sizeof(*list));
    list->origin = origin;
    list->breakpoint = breakpoint;
    list->tail = tail;
    return list;
}

breakList TAB_emptyBreakList(){
    return BreakList(0, 0, NULL);
}

long TAB_newBreakpoint(breakList breakpointList, long origin){
    long break_int = newInt3();
    breakList newBreak = BreakList(origin, break_int, breakpointList->tail);
    breakpointList->tail = newBreak;
    return break_int;
}

static union {
    long code;
    struct {
        char int3;
        int index;
    }set;
}trans = {.set.int3 = 0xcc};

static long newInt3() {
    assert(trans.set.int3 == (char)0xcc);
    trans.set.index = break_count++;
    return trans.code;
}

long TAB_getOrigin(breakList breakpointList, long breakpoint){
    breakList iter;
    for (iter = breakpointList->tail; iter; iter = iter->tail){
        if (iter->breakpoint == breakpoint)
            return iter->origin;
    }
    assert(0);
}
