#include "rvm.cpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


int main(){

printf("inside main\n");
printf("again\n");
rvm_t rvm = rvm_init("Dir");
//char *p = reinterpret_cast<char*>(rvm_map(rvm, "seg1", 1000));
char * p = (char *) malloc (1000*sizeof(char));
p = (char *)rvm_map(rvm, "seg1", 1000);
int *i;
printf(" map actually returns: %p\n", p);
printf("\nCalling unmap");
rvm_unmap(rvm,p);
rvm_destroy(rvm,"seg1");

return 0;
}


