#include "rvm.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

using namespace std;

rvm_t rvm_global;
/*
    Initialize the library with the specified directory as backing store.
    Initialize Data Structures needed to maintain persistent virtual memory.
    Create an Alarm which runs through the ReDo Logs.
*/
rvm_t rvm_init(const char *directory){
    rvm_t rvm;
    struct stat status;
    
    rvm.directory = (char *) directory;
    if(stat(directory,&status) < 0){
        char dir[1000] = "mkdir ";
        strcat(dir,directory);
        system(dir);
	cout << "Created New Directory " << endl;
    }
    else 
	printf("\nDirectory already exists \n");
	rvm_global = rvm;
    return rvm;
}


//map a segment from disk into memory. If the segment does not already exist, then create it and give it size size_to_create. If the segment exists but is shorter than size_to_create, then extend it until it is long enough. It is an error to try to map the same segment twice.
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create){
    rvm = rvm_global;
    rvm_truncate_log(rvm);
    char temp[100];
    strcpy (temp, rvm.directory);
    struct stat status;
    char *name = (char *)segname;
    void **addr1;
    strcat(temp,"/");
    strcat(temp,segname);
    
    /* Check if the Segment being created already exists */
    if(stat(temp,&status) < 0){
        //doesn't exist
	// ALlocate memory to the segment
        void *addr = (void *)malloc(size_to_create*sizeof(char));
        segname_struct segn;
        segn.size = size_to_create;
	segn.name = (char *)segname;

	// Insert into List of Segments and Segname-Segbase in RVM       
        rvm.list_of_segments.insert(pair<void*,segname_struct>(addr,segn));
        rvm.list_of_segname_segbase.insert(pair<char*,void*>(name,addr));

	fstream fs;
	fs.open(temp, fstream::in|fstream::out|fstream::app);
	fs.close();
        rvm_global = rvm;
	cout<<"map returns "<<addr<<endl;
        return addr;
                        
    }else{
          map<char*,void*>::iterator it = rvm.list_of_segname_segbase.end();
          //make sure it is not already mapped.
          if(rvm.list_of_segname_segbase.find(name) != it){
              printf("\nError! Segment is already mapped. Returning\n");
              return NULL;
          }

	  // Allocate memory to the segment
          void *addr = (void *)malloc(size_to_create*sizeof(char));
          segname_struct segn;
          segn.size = size_to_create;
          segn.name = (char *) malloc(sizeof(char));
	  segn.name = (char *)segname;
	  // Insert into List of Segments and Segname-Segbase in RVM
          rvm.list_of_segments.insert(pair<void*,segname_struct>(addr,segn));
          rvm.list_of_segname_segbase.insert(pair<char*,void*>(name,addr));

	  // Verify if File is present and can be opened correctly
	  fstream fs;
          fs.open(temp, fstream::in|fstream::out|fstream::app);
	
	  if(!fs.is_open()){
		printf("\nError in opening file\n");
		return NULL;
	  }
	  fs.close();
	  cout<<"\nmap returns "<<addr<<endl;
          rvm_global = rvm;
          return  addr;
    }
}				
			

// - unmap a segment from memory
void rvm_unmap(rvm_t rvm, void *segbase){

    // Check if any segment is mapped to address segbase. If not, nothing to delete
    if (rvm.list_of_segments.find(segbase) != rvm.list_of_segments.end()) {
	rvm.list_of_segname_segbase.erase(rvm.list_of_segments.find(segbase)->second.name);
	rvm.list_of_segments.erase(segbase);
    } else {
	cout << "Trying to unmap a non-existent mapping" << endl;
    }

    rvm_global = rvm;

	return;
}

// - destroy a segment completely, erasing its backing store. This function should not be called on a segment that is currently mapped.
void rvm_destroy(rvm_t rvm, const char *segname){
	rvm = rvm_global;
	char *name;
	name = (char *)segname;
	
	// Make sure the segment to be destroyed is currently not mapped to any address before destroying it from disk	
	if(rvm.list_of_segname_segbase.find(name) == rvm.list_of_segname_segbase.end()) {
		char temp[100];
		strcpy(temp,"rm ");
		strcat(temp, rvm.directory);
		strcat(temp,"/");
		strcat(temp,name);
		system(temp);
	}
	else{
		cout<<"Error! Trying to destroy a segment that is mapped!" <<endl;
	}
	rvm_global = rvm;
}

// - begin a transaction that will modify the segments listed in segbases. If any of the specified segments is already being modified by a transaction, then the call should fail and return (trans_t) -1. Note that trant_t needs to be able to be typecasted to an integer type.
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

				
}
