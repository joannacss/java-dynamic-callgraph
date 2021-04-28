#include "callgraph.h"



CallGraph * new_callgraph(Edge * edge){
    CallGraph * c = (CallGraph*) malloc(sizeof(CallGraph));
    memset(c, 0, sizeof *c);
    c->edge = *edge;
    return c;
}

Edge * new_edge(StackNode * from, StackNode * to){
    Edge * edge = (Edge*) malloc(sizeof(Edge));
    memset(edge, 0, sizeof * edge);
    edge->caller_class_signature = from->class_signature;
    edge->caller_method_name = from->method_name;
    edge->caller_method_signature = from->method_signature;
    edge->callee_class_signature = to->class_signature;
    edge->callee_method_name = to->method_name;
    edge->callee_method_signature = to->method_signature;
    return edge;
}



void add_edge(CallGraph * cg, StackNode * from, StackNode * to){
    
    /*fprintf(stderr, "BEFORE COUNT=%d\n", HASH_COUNT(cg));
    Edge * edge = new_edge(from, to);
    CallGraph * itemToLookup = new_callgraph(edge);
    CallGraph * found_item;
    
    // HASH_FIND(hh, cg, &itemToLookup->edge, sizeof(Edge), found_item);
    
    
    // item found, increment the counter
    // if(found_item){
    //     found_item->count++;
    // }else{
        itemToLookup->count = 1;
        // item not found, add a new one (count = 1)
        HASH_ADD(hh, cg, edge, sizeof(Edge), itemToLookup);            
    // }
    fprintf(stderr, "AFTER COUNT=%d\n", HASH_COUNT(cg));*/

    fprintf(stderr, "%.*s.", (int) strlen(from->class_signature)-1, from->class_signature);
    fprintf(stderr, "%s%s", from->method_name, (from->method_signature == NULL ? "" : from->method_signature));
    fprintf(stderr, "\t");
    fprintf(stderr, "%.*s.", (int) strlen(to->class_signature)-1, to->class_signature);
    fprintf(stderr, "%s%s", to->method_name, (to->method_signature == NULL ? "" : to->method_signature));
    
}



void print_callgraph(CallGraph *cg){
    
    CallGraph *node, *tmp;
    HASH_ITER(hh, cg, node, tmp){
        fprintf(stderr, "%.*s.", (int) strlen(node->edge.caller_class_signature)-1, node->edge.caller_class_signature);
        fprintf(stderr, "%s%s", node->edge.caller_method_name, (node->edge.caller_method_signature == NULL ? "" : node->edge.caller_method_signature));
        fprintf(stderr, "\t");
        fprintf(stderr, "%.*s.", (int) strlen(node->edge.callee_class_signature)-1, node->edge.callee_class_signature);
        fprintf(stderr, "%s%s", node->edge.callee_method_name, (node->edge.callee_method_signature == NULL ? "" : node->edge.callee_method_signature));        
        fprintf(stderr, "%d\n", node->count);        
    }
}