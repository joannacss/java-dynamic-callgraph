#include <jvmti.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "prog_stack.h"
#include "callgraph.h"


static volatile Stack * prog_stack = NULL;
static volatile CallGraph * cg = NULL;

static volatile int max_depth = 0;

static int adjust_stack_depth(jvmtiEnv *jvmti, int delta) {
    intptr_t depth = 0;
    (*jvmti)->GetThreadLocalStorage(jvmti, NULL, (void**)&depth);
    (*jvmti)->SetThreadLocalStorage(jvmti, NULL, (const void*)(depth + delta));

    return (int)depth;
}


static void print_trace(bool entry, jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method){

    char * method_name, * signature, *generic_signature;
    char * class_signature, *class_generic_signature;

    jvmtiThreadInfo thread_info;
    jclass klass;
    (*jvmti)->GetMethodName(jvmti, method, &method_name, &signature, &generic_signature);
    (*jvmti)->GetThreadInfo(jvmti, thread, &thread_info);
    (*jvmti)->GetMethodDeclaringClass(jvmti, method, &klass);
    (*jvmti)->GetClassSignature(jvmti, klass,&class_signature,&class_generic_signature);

    // add to the program stack
    StackNode * new_node = new_stack_node(class_signature,  method_name, signature);
    if(entry) {
        StackNode * from_node = peek(prog_stack);
        // adds an edge to the call graph
        if(from_node != NULL) {
            add_edge(cg, from_node, new_node);
            fprintf(stderr, "AFTER AFTER COUNT=%d\n", HASH_COUNT(cg));
        }
        // add to the program stack
        push(prog_stack, new_node);        
    }else{
        // removes from the program stack
        pop(prog_stack);
    }

    // prints execution trace
    printf("%s", (entry ? ">" : "<"));
    printf("[%s] ",thread_info.name);
    printf("%.*s.", (int) strlen(class_signature)-1, class_signature);
    printf("%s%s\n", method_name, (signature==NULL?"":signature));
}

void JNICALL MethodEntry(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method) {
    adjust_stack_depth(jvmti, +1);
    print_trace(true, jvmti, jni, thread, method);
}

void JNICALL MethodExit(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method,
                        jboolean was_popped_by_exception, jvalue return_value) {
    int depth = adjust_stack_depth(jvmti, -1);
    if (depth > max_depth) {
        max_depth = depth;  // TODO: replace with atomic CAS to avoid race condition
    }
    print_trace(false, jvmti, jni, thread, method);
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {

fprintf(stderr, "Agent_OnLoad");
    prog_stack = init_program_stack();
    

    jvmtiEnv* jvmti;
    (*vm)->GetEnv(vm, (void**)&jvmti, JVMTI_VERSION_1_0);

    jvmtiCapabilities capabilities = {0};
    capabilities.can_generate_method_entry_events = 1;
    capabilities.can_generate_method_exit_events = 1;
    (*jvmti)->AddCapabilities(jvmti, &capabilities);

    jvmtiEventCallbacks callbacks = {0};
    callbacks.MethodEntry = MethodEntry;
    callbacks.MethodExit = MethodExit;
    (*jvmti)->SetEventCallbacks(jvmti, &callbacks, sizeof(callbacks));

    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT, NULL);

    return 0;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm) {
    // print a runtime call graph here
    print_callgraph(cg);
}