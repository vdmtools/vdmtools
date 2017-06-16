
include makeincl

SUITE = -I/home/hermes/cg/test/PP/expr/oldname

oldname-01: A.h A.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-oldname-01: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-oldname-01: A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

