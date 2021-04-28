CC=gcc
CFLAGS=-fPIC -I ${JAVA_HOME}/include -I ${JAVA_HOME}/include/darwin

compile: 
	${CC} ${CFLAGS} -shared -o libdyncg.so *.c

clean:
	rm *.so
# gcc -fPIC -shared -I $JAVA_HOME/include -I $JAVA_HOME/include/darwin -o libdyncg.so dyncg.c

