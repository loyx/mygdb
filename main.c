#include <stdio.h>
#include <stdbool.h>
#include "debuger.h"

extern bool ASLR;

int main(int argc, char** argv) {

    if (argc < 2 || argc > 4){
        printf("usage: mgdb [-d file| -t pid] [-a]\n");
        return -1;
    }
    if (argc == 4)
        ASLR = true;

    switch (argv[1][1]) {
        case 'd':
            startTrace(argv[2]);
            break;
        case 't':
            startAttach(argv[2]);
            break;
    }

    return 0;
}
