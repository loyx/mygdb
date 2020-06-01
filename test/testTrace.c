//
// Created by loyx on 2020/6/1.
//

struct stc1{
    int a1,a2;
};
struct stc2{
    int b1;
    struct stc1 sub_stc1;
};

int main(){
    struct stc2 a = {.b1 = 1, .sub_stc1 = {2, 3}};
    return 0;
}