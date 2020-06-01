//
// Created by loyx on 2020/5/31.
//

#ifndef DEBUGER_BPTABLE_H
#define DEBUGER_BPTABLE_H

typedef struct breakList_ *breakList;

long TAB_newBreakpoint(breakList breakpointList, long origin);
long TAB_getOrigin(breakList breakpointList, long breakpoint);
breakList TAB_emptyBreakList();

#endif //DEBUGER_BPTABLE_H
