#include <jvmti.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// https://www.oracle.com/technical-resources/articles/javase/jvmti.html
// https://docs.oracle.com/javase/8/docs/platform/jvmti/jvmti.html#jmethodID
// https://stackoverflow.com/questions/52185905/is-there-a-way-to-know-maximally-reached-jvm-call-stack-depth-for-a-particular-p
static volatile int max_depth = 0;
FILE *output_file = NULL;

#define TRACK_OPAQUE_CODE 0


static int adjust_stack_depth(jvmtiEnv *jvmti, int delta) {
    intptr_t depth = 0;
    (*jvmti)->GetThreadLocalStorage(jvmti, NULL, (void**)&depth);
    (*jvmti)->SetThreadLocalStorage(jvmti, NULL, (const void*)(depth + delta));

    return (int)depth;
}


void print_trace(bool entry, jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method){

    char * method_name, * signature, *generic_signature;
    char * class_signature, *class_generic_signature;

    jvmtiThreadInfo thread_info;
    jclass klass;
    (*jvmti)->GetMethodName(jvmti, method, &method_name, &signature, &generic_signature);
    (*jvmti)->GetThreadInfo(jvmti, thread, &thread_info);
    (*jvmti)->GetMethodDeclaringClass(jvmti, method, &klass);
    (*jvmti)->GetClassSignature(jvmti, klass,&class_signature,&class_generic_signature);

    jboolean isSynthetic, isNative;
    (*jvmti)->IsMethodNative(jvmti, method, &isNative);    
    (*jvmti)->IsMethodSynthetic(jvmti, method, &isSynthetic);    
    // method is native or synthetic, and we disabled tracking opaque code
    // early return so that we don't track the edge
    if((isSynthetic || isNative) && !(TRACK_OPAQUE_CODE))
        return;

    fprintf(output_file, "%s", (entry ? ">" : "<"));
    fprintf(output_file, "[%s] ",thread_info.name);
    fprintf(output_file, "%.*s.", (int) strlen(class_signature)-1, class_signature);
    fprintf(output_file, "%s%s\n", method_name, (signature==NULL?"":signature));
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
    // initializes output
    output_file = fopen("trace.txt", "w");
    //if file pointer is null, print the statement
    if (output_file == NULL){
        printf("Sorry. . The file you are trying to open do not exist . . .");
        return -1;
    }
    
    // initializes JVM agent
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
}
