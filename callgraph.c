#include "callgraph.h"



CallGraph * init_callgraph(){
    CallGraph * c = (CallGraph*) malloc(sizeof(CallGraph));
    memset(c, 0, sizeof (CallGraph));
    return c;
}


CGNode * new_cg_node(StackNode * stack_node){
    CGNode * cg_node = (CGNode*) malloc(sizeof(CGNode));
    memset(cg_node, 0, sizeof (CGNode));

    cg_node->class_signature = stack_node->class_signature;
    cg_node->method_name = stack_node->method_name;
    cg_node->method_signature = stack_node->method_signature;

    return cg_node;
}

Edge * new_edge(CGNode * from, CGNode * to){
    Edge * edge = (Edge*) malloc(sizeof(Edge));
    memset(edge, 0, sizeof (Edge));

    edge->from = from;
    edge->to = to;

    return edge;
}

void add_edge(volatile CallGraph * cg, CGNode * from, CGNode * to){
    
    Edge * edge = new_edge(from, to);

    CallGraph * found_item;
    
    HASH_FIND(hh, cg, edge, sizeof(Edge), found_item);
    
    fprintf(stderr, "FOUND %d\n", HASH_COUNT(cg));
    // item found, increment the counter
    if(found_item){
        found_item->count++;
    }else{

        found_item = init_callgraph();
        found_item->edge.from = edge->from;
        found_item->edge.to = edge->to;
        found_item->count = 1;
        // item not found, add a new one (count = 1)
        HASH_ADD(hh, cg, edge, sizeof(Edge), found_item);    
    }
    
}


void print_cg_node(CGNode * node){
    fprintf(stderr, "%.*s.", (int) strlen(node->class_signature)-1, node->class_signature);
    fprintf(stderr, "%s%s", node->method_name, (node->method_signature == NULL ? "" : node->method_signature));
}

void print_callgraph(volatile CallGraph *cg){
    
    CallGraph *current, *tmp;
    HASH_ITER(hh, cg, current, tmp){
        print_cg_node(current->edge.from);
        fprintf(stderr, "\t");
        print_cg_node(current->edge.to);
        fprintf(stderr, "%d\n", current->count);        
    }
}