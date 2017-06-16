
include makeincl

type-01 \
type-02 \
type-03 : A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-type-01 \
javac-type-02 \
javac-type-03: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java


java-type-01 \
java-type-02 \
java-type-03: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

type-04 : A.h A.cc B.h B.cc C.h C.cc D.h D.cc
	cp -f B.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc C.cc D.cc ${MAIN} ${OPTIONS}

javac-type-04: A.java B.java C.java D.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java D.java ${MAINJAVA}.java

java-type-04: A.class B.class C.class D.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

