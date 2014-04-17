#ifndef __LIBRVM__
#define __LIBRVM__

#include "rvm_internal.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <list>

using namespace std;

long int trans_count;

typedef struct Segment_Name{
    int size;           // Size of Segment
    char *name;         // Name of Segment
}segname_struct;

typedef long int trans_t;

typedef struct Change{
    long int offset;    // Offset within the Segment
    int size_modified;  // Size of segment modified
}change;


typedef struct Transaction{
    trans_t tid;        // Transaction ID
    char *redo_log;     // Redo File Name
    char *undo_log;     // Undo Directory Name
    map<char *,list<change> > modified_segments;  //segname, list_of_changes
    map<void *, char *> list_of_segments;         //segbase, segname
}txn;

map<trans_t,txn> list_of_transactions;      // Map of TID and TXN

typedef struct RVM_T{
    char *directory;    // Directory where all files are stored
    map<void *, segname_struct> list_of_segments;   // <Segbase, segname_struct>
    map<char *,void *> list_of_segname_segbase;     // <Segname, Segbase>
}rvm_t;

/*
Initialize all RVM_T data structures and start Alarm to run through Redo Logs
*/
rvm_t rvm_init(const char *directory);

/*
    Map a segment from disk into memory. 
    If the segment does not already exist, then create it and give it
    size size_to_create.
    If the segment exists but is shorter than size_to_create, then
    extend it until it is long enough.
    It is an error to try to map the same segment twice.
*/
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create);

/*
    Unmap a Segment from Memory
*/
void rvm_unmap(rvm_t rvm, void *segbase);

/*
    Destroy a Segment completely, erasing it's backing store.
*/
void rvm_destroy(rvm_t rvm, const char *segname);

/*
    Begin a transaction that will modify the segments listed in segbases.
    If any of the specified segments is already being modified by a
    transaction, then the call should fail and return (trans_t) -1
*/
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases);

/*
    Declare that the library is about to modify a specified range
    of memory in the specified segment. The segment must be one of
    the segments specified in the call to rvm_begin_trans.
*/
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size);

/*
    Commit all changes that have been made within the specified transaction
*/
void rvm_commit_trans(trans_t tid);

/*
    Undo all changes that have happened within the specified transaction
*/
void rvm_abort_trans(trans_t tid);

/*
    Play through any committed or aborted items in the log file(s)
    and shrink the log file(s) as much as possible.
*/
void rvm_truncate_log(rvm_t rvm);

#endif
