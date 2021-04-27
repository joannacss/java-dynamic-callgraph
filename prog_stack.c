#include "prog_stack.h"

Stack * init_program_stack(){
    Stack * s = (Stack*) malloc(sizeof(Stack));
    s->top = NULL;
    return s;
}

Stack * new_node(char * class_signature, char * method_name, char * method_signature){
    StackNode * node = (StackNode*) malloc(sizeof(StackNode));

    node->class_signature = class_signature;
    node->method_name = method_name;
    node->method_signature = method_signature;
    node->next = NULL;

    return node;
}


bool isEmpty(Stack *s) {
   return s->top == NULL;
}

StackNode * peek(Stack *s){
    return isEmpty(s) ? NULL : s->top->value;
}

StackNode * pop(Stack *s){
    if(!isEmpty(s)){
        StackNode * current_top = s->top;
        s->top = s->top->next;
        return current_top;
    }
    return NULL;
}


void push(Stack * s, StackNode * new_node){
    new_node->next = s->top;
    s->top = new_node;
}



