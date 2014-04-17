#ifndef __LIBRVM__
#define __LIBRVM__

#include "rvm_internal.h"
#include <stdio.h>
#include <vector>
#include <map>

using namespace std;

long int trans_count;

typedef struct Segment_Name{
    int size;
    char *name;
}segname_struct;

typedef long int trans_t;

typedef struct Change{
    long int start_addr;
    int size_modified;
}change;


typedef struct Transaction{
    trans_t tid;
    char* redo_log;
    char* undo_log;
    map modified_segments<char*,list<change>>;  //segname, list_of_changes
    map list_of_segments<void*, char*>;         //segbase, segname
}txn;

map list_of_transactions<trans_t,txn>;

typedef struct RVM_T{
    char *directory;
    map list_of_segments<void*, segname_struct>;
    map list_of_segname_segbase<char*,void*>;
}rvm_t;


rvm_t rvm_init(const char *directory);
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);
void rvm_unmap(rvm_t rvm, void *segbase);
void rvm_destroy(rvm_t rvm, const char *segname);
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);
void rvm_commit_trans(trans_t tid);
void rvm_abort_trans(trans_t tid);
void rvm_truncate_log(rvm_t rvm);

#endif
