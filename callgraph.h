#ifndef _JDYNCG_CALLGRAPH_H
#define _JDYNCG_CALLGRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include "prog_stack.h"
#include "lib/uthash.h"

typedef struct CGNode CGNode;
typedef struct CallGraph CallGraph;
typedef struct Edge Edge;

struct CGNode{
    char * class_signature;
    char * method_name;
    char * method_signature;    
};


struct Edge{
    CGNode * from;
    CGNode * to;    
};



struct CallGraph{
    Edge edge; /* key */
    int count; /* number of times it occurred at runtime */
    UT_hash_handle hh; /* makes this data structure hashable */
};



CallGraph * init_callgraph();


CGNode * new_cg_node(StackNode * stack_node);

Edge * new_edge(CGNode * from, CGNode * to);

void add_edge(volatile CallGraph * cg, CGNode * from, CGNode * to);


void print_cg_node(CGNode * node);

void print_callgraph(volatile CallGraph *cg);
#endif