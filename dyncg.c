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



/**
 * Every JVMTI interface returns an error code, which should be checked to avoid
 * any cascading errors down the line. The function GetErrorName() returns the
 * actual enumeration constant name, making the error messages much easier to
 * understand.
 */
static void check_jvmti_error(jvmtiEnv *jvmti, jvmtiError error, const char *msg) {
    if (error != JVMTI_ERROR_NONE) {
        char *name = NULL;
        (*jvmti)->GetErrorName(jvmti, error, &name);
        fprintf(stderr, "ERROR: JVMTI: %d(%s): %s\n", error, (name == NULL ? "Unknown" : name), (msg == NULL ? "" : msg));
    }
}

/**
 * Only a single MethodEntry/MethodExit callback may be active at any one time.
 */
static jrawMonitorID callback_lock;

static void enter_critical_section(jvmtiEnv *jvmti, jrawMonitorID lock) {
    jvmtiError error = (*jvmti)->RawMonitorEnter(jvmti, lock);
    check_jvmti_error(jvmti, error, "Cannot enter with raw monitor");
}

static void exit_critical_section(jvmtiEnv *jvmti, jrawMonitorID lock) {
    jvmtiError error = (*jvmti)->RawMonitorExit(jvmti, lock);
    check_jvmti_error(jvmti, error, "Cannot exit with raw monitor");
}



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
    if((isSynthetic || isNative) && !(TRACK_OPAQUE_CODE))
        return; // early return so that we don't track the edge

    fprintf(output_file, "%s[%s] %.*s.%s%s\n", 
        (entry ? ">" : "<"),
        thread_info.name,
        (int) strlen(class_signature) - 1, class_signature,
        method_name, 
        (signature == NULL ? "" : signature)
    );

}

void JNICALL MethodEntry(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method) {
    enter_critical_section(jvmti, callback_lock); {
        adjust_stack_depth(jvmti, +1);
        print_trace(true, jvmti, jni, thread, method);
    } exit_critical_section(jvmti, callback_lock);
}

void JNICALL MethodExit(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method,
                        jboolean was_popped_by_exception, jvalue return_value) {
    enter_critical_section(jvmti, callback_lock); {
        int depth = adjust_stack_depth(jvmti, -1);
        if (depth > max_depth) {
            max_depth = depth;  // TODO: replace with atomic CAS to avoid race condition
        }
        print_trace(false, jvmti, jni, thread, method);
    } exit_critical_section(jvmti, callback_lock);
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
