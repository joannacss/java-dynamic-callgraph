# JDynCg: Java Dynamic Call Graph extractor
**JDynCg** can be used to compute dynamic call graphs for Java programs. It is an JVMTI agent that captures the execution traces to build the call graph.

*(This is a working progress; at this moment the code computes an execution trace which can be parsed into a call graph).*


## Requirements
- GCC 
- `JAVA_HOME` is set


## Compile


**MacOS**

`gcc -fPIC -shared -I $JAVA_HOME/include -I $JAVA_HOME/include/darwin -o libdyncg.so dyncg.c`



**Windows**


`gcc -fPIC -shared -I $JAVA_HOME\include -I $JAVA_HOME\include\win32 -o libdyncg.dll dyncg.c`

or

`cl /I<jre_path>\include /MD /FetiSample.dll tiSample.c /link /DLL`



## Running the Agent


`java -agentpath:/path/to/libdyncg.dll fully.qualified.ClassName`



## Output

The agent output the execution trace with the following format:

`> [thread_name] class_signature.method_signature`
`< [thread_name] class_signature.method_signature`

Lines preceded with `>`, indicate method entries (start of execution) and `<` indicates a method exit (end of execution).

An execution trace example:

```
>[main] Ljava/io/FileInputStream.read()I
>[main] Ljava/io/FileInputStream.read0()I
<[main] Ljava/io/FileInputStream.read0()I
<[main] Ljava/io/FileInputStream.read()I
```


