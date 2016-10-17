
include makeincl

SUITE = -I/home/hermes/cg/test/PP/types/

types-01: superA.h superA.cc A.h A.cc B.h B.cc subA.cc subA.h
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc subA.cc superA.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-types-01: A.java B.java superA.java subA.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java superA.java subA.java ${MAINJAVA}.java

java-types-01: A.class B.class superA.java subA.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

types-02: A.h A.cc B.h B.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-types-02: A.java B.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-types-02: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

types-03: A.h A.cc B.h B.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-types-03: A.java B.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-types-03: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

types-04: A.h A.cc B.h B.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-types-04: A.java B.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-types-04: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

