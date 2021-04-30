#ifndef _JDYNCG_CALLGRAPH_H
#define _JDYNCG_CALLGRAPH_H

#include <stdlib.h>    /* malloc       */
#include <stddef.h>    /* offsetof     */
#include <stdio.h>     /* printf       */
#include <string.h>    /* memset       */
#include <pthread.h>

#include "prog_stack.h"
#include "lib/uthash.h"

typedef struct CallGraph CallGraph;
typedef struct Edge Edge;

struct Edge{
    char * caller_class_signature; char * caller_method_name; char * caller_method_signature;
    char * callee_class_signature; char * callee_method_name; char * callee_method_signature;    
};

struct CallGraph{
    /* key */
    Edge edge;
    int count; /* number of times it occurred at runtime */
    UT_hash_handle hh; /* makes this data structure hashable */
};



CallGraph * new_callgraph(Edge * edge);


Edge * new_edge(StackNode * from, StackNode * to);

CallGraph * add_edge(CallGraph * cg, StackNode * from, StackNode * to);



void print_callgraph(CallGraph *cg);
#endif