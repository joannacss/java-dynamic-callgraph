#ifndef _JDYNCG_STACK_H
#define _JDYNCG_STACK_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct StackNode StackNode;

struct StackNode{
	char * class_signature;
    char * method_name;
    char * method_signature;
    StackNode * next;
};


typedef struct Stack Stack;

struct Stack{
    StackNode * top;
    int count;
};



Stack * init_program_stack();


StackNode * new_stack_node(char * class_signature, char * method_name, char * method_signature);

bool isEmpty(volatile Stack * s);

StackNode * peek(volatile Stack * s);

StackNode * pop(volatile Stack * s);

void push(volatile  Stack * s, StackNode * new_node);


#endif