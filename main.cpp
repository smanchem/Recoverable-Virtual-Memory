#include "rvm.cpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(){

printf("\ninside main");
rvm_t rvm = rvm_init("Dir");
char *p = (char *)rvm_map(rvm, "seg1", 1000);
return 0;
}


