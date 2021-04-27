#ifndef _JDYNCG_STACK_H
#define _JDYNCG_STACK_H


typedef struct {
	char * class_signature;
    char * method_name;
    char * method_signature;
    struct StackNode * next;
}StackNode;


typedef struct {
    StackNode * top;
}Stack;



Stack * init_program_stack();


Stack * new_node(char * method_name, char * method_signature);

bool isEmpty(Stack * s);

StackNode * peek(Stack * s);

StackNode * pop(Stack * s);

void push(Stack * s, StackNode * new_node);


#endif