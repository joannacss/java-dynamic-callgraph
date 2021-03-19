#include "prog_stack.h"

JDynStack * init_program_stack(){
    return (JDynCallgraph*) malloc(sizeof(JDynCallgraph));
}


bool isEmpty() {
   return top == NULL;
}

JDynCallgraph * peek(){
    return isEmpty() ? NULL : top->value;
}

JDynCallgraph * pop(){
    if(!isEmpty()){
        JDynCallgraph current_value = top->value;
        top->value = current_value->next;
        if(top->value != NULL)
            top->next = top->value->next;
        return current_value;    
    }
    return NULL;
}

// void push(JDynCallgraph * value);




// JDynCallgraph peek() {
//    return top.value;
// }


// int pop() {
//    int data;
    
//    if(!isempty()) {
//       data = stack[top];
//       top = top - 1;   
//       return data;
//    } else {
//       printf("Could not retrieve data, Stack is empty.\n");
//    }
// }

// int push(int data) {

//    if(!isfull()) {
//       top = top + 1;   
//       stack[top] = data;
//    } else {
//       printf("Could not insert data, Stack is full.\n");
//    }
// }


