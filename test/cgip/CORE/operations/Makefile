
include makeincl

SUITE = -I/home/cg/test/SL/operations

operations-%: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} $(SUITE) ${OPTIONS}

javac-operations-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java  

java-operations-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

