
include makeincl

instvars-01: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-instvars-01: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-instvars-01: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

