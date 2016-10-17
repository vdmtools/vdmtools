
include makeincl

SUITE = -I/home/hermes/cg/test/PP/stmt/specification/

specification-01 \
specification-02 \
specification-03: A.h A.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-specification-01 \
javac-specification-02 \
javac-specification-03: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-specification-01 \
java-specification-02 \
java-specification-03: A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

