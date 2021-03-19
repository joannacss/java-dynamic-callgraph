#ifndef _JDYNCG_STACK_H
#define _JDYNCG_STACK_H


typedef struct {
    char * caller_name; char * caller_signature;
    char * callee_name; char * callee_signature;
    int frequency;
}JDynCallgraph;


typedef struct {
    JDynCallgraph * value;
    JDynCallgraph * next;
}JDynStack;





/** Initializes Stack */
JDynStack * top = NULL;


JDynStack * init_program_stack();

bool isEmpty();

JDynCallgraph * peek();

JDynCallgraph * pop();

void push(JDynCallgraph * value);


#endif