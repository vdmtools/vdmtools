
include makeincl

SUITE = -I/home/hermes/cg/test/PP/access/

access-01: A.h
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc  ${MAIN} ${SUITE} ${OPTIONS}

access-02: A.h B.h 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc  B.cc ${MAIN} ${SUITE} ${OPTIONS}

access-03: A.h B.h
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc  B.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-access-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-access-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

