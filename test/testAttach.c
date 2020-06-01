//
// Created by loyx on 2020/6/1.
//
#include <stdio.h>
#include <unistd.h>

struct stc1{
    int a1,a2;
};
struct stc2{
    int b1;
    struct stc1 sub_stc1;
};

int main(){
    setvbuf(stdout, NULL, _IONBF, 0);
    struct stc2 a = {.b1 = 1, .sub_stc1 = {2, 3}};
    int cnt = 0;
    while (1){
        sleep(1);
        cnt++;
        printf("\r%d", cnt);
    }
}
