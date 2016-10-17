
include makeincl

union-%: A.h A.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-union-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-union-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

