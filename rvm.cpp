#include "rvm.h"
#include <map>
#include <list>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;


/*
    Initialize the library with the specified directory as backing store.
    Initialize Data Structures needed to maintain persistent virtual memory.
    Create an Alarm which runs through the ReDo Logs.
*/
rvm_t rvm_init(const char *directory){
    struct stat status;
    
    rvm_t rvm;
    strcpy(rvm.directory,directory);
    
    if(stat(directory,&status) < 0){
        char dir[1000];
        strcpy(dir, "mkdir ");
        strcat(dir,directory);
        system(dir);
    }
    else printf("\nDirectory already exists");
    
    /* Initialize the time interval for Log Truncation */
    long int period = 100000;
    struct itimerval it;
    struct sigaction act, oact;
    long sec = period/1000000;
    long usec = period - (sec*1000000);
    it.it_interval.tv_sec = sec;
    it.it_interval.tv_usec = usec;
    it.it_value.tv_sec = sec;
    it.it_value.tv_usec = usec;

    act.sa_handler = rvm_truncate_log(rvm);
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction (SIGPROF, &act, &oact);

printf("\n Starting timer \n");

    /* Start timer */
    setitimer (ITIMER_PROF, &it, NULL);

printf("Timer started \n");
         
    return rvm;
}


//map a segment from disk into memory. If the segment does not already exist, then create it and give it size size_to_create. If the segment exists but is shorter than size_to_create, then extend it until it is long enough. It is an error to try to map the same segment twice.
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create){
    rvm_truncate_log(rvm);

    char temp[1000];
    struct stat status;
    char *name;
    void **addr1;
    strcpy(name,segname);

    strcpy(temp,rvm.directory);
    strcat(temp,"/");
    strcat(temp,segname);
    if(stat(temp,&status) < 0){
        //doesn't exist
        printf("\n in here");printf("\n in here");
        void *addr = malloc(size_to_create);
        *addr1 = addr;
        segname_struct segn;
        segn.size = size_to_create;
        strcpy(segn.name,segname);
        
        rvm.list_of_segments.insert(pair<void**,segname_struct>(addr1,segn));
        rvm.list_of_segname_segbase.insert(pair<char*,void**>(name,addr1));

        stat(temp,&status);
        
        int fd = open(temp, O_RDWR|O_CREAT|O_EXCL, 777); //Create and open a file with file name temp
        if(fd < 0){
            printf("\nError in creating and opening file. Returning\n");
            return NULL;
        }
        
        if(lseek(fd, size_to_create, SEEK_SET) < 0){    //Expand size of file to "size_to_create" bytes
            close(fd);
            printf("\nError in taking cursor to end of file! Returning\n");
            return NULL;
        }
        
        return addr;
                        
    }else{
          printf("\n in here1");printf("\n in here1");
          map<char*,void**>::iterator it = rvm.list_of_segname_segbase.end();
          //make sure it is not already mapped.
          if(rvm.list_of_segname_segbase.find(name) != it){
              printf("\nError! Segment is already mapped. Returning\n");
              return NULL;
          }
          
          void *addr = malloc(size_to_create);
          *addr1 = addr;
          segname_struct segn;
          segn.size = size_to_create;
          strcpy(segn.name,segname);

          rvm.list_of_segments.insert(pair<void**,segname_struct>(addr1,segn));
          rvm.list_of_segname_segbase.insert(pair<char*,void**>(name,addr1));

          //stat(temp,&status);
          
          int fd = open(temp, O_RDWR);
          if(fd < 0){
            printf("\nError in opening file. Returning\n");
            return NULL;
          }
          
          if(lseek(fd, size_to_create, SEEK_END) < 0){
            close(fd);
            printf("\nError in taking cursor to end of file! Returning\n");
            return NULL;
          }
          return addr;
    }
}				
			

// - unmap a segment from memory
void rvm_unmap(rvm_t rvm, void *segbase){
    //map<char*,void*>::iterator it = rvm.list_of_segments.end();

}


// - destroy a segment completely, erasing its backing store. This function should not be called on a segment that is currently mapped.
void rvm_destroy(rvm_t rvm, const char *segname){


}

/*
    Begin a transaction that will modify the segments listed in segbases.
    If any of the specified segments is already being modified by a
    transaction, then the call should fail and return (trans_t) -1
*/
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases){
    
}


// - declare that the library is about to modify a specified range of memory in the specified segment. The segment must be one of the segments specified in the call to rvm_begin_trans. Your library needs to ensure that the old memory has been saved, in case an abort is executed. It is legal to call rvm_about_to_modify multiple times on the same memory area.
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size){

}


// - commit all changes that have been made within the specified transaction. When the call returns, then enough information should have been saved to disk so that, even if the program crashes, the changes will be seen by the program when it restarts.
void rvm_commit_trans(trans_t tid){

}

// - undo all changes that have happened within the specified transaction.
void rvm_abort_trans(trans_t tid){


}
		
// - play through any committed or aborted items in the log file(s) and shrink the log file(s) as much as possible.
void rvm_truncate_log(rvm_t rvm){
    /*
    Block SIG_BLOCK signal while Truncating Logs
    */
    sigset_t new, old;
    sigemptyset (&new);
    sigaddset (&new, SIGPROF);
    sigprocmask(SIG_BLOCK, &new, &old);

// Truncate Logs

    /* Unblock SIG_BLOCK signal */	
    sigprocmask(SIG_SETMASK, &old, NULL);				
}
