
include makeincl

isofclass-01: A.h A.cc B.h B.cc C.h C.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc ${MAIN} ${OPTIONS}

isofclass-02: A.h A.cc B.h B.cc B.h B.cc C.h C.cc D.h D.cc E.h E.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc D.cc E.cc ${MAIN} ${OPTIONS}

javac-isofclass-01: A.java B.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java ${MAINJAVA}.java

java-isofclass-01: A.class B.class C.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-isofclass-02: A.java B.java C.java D.java E.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java D.java E.java ${MAINJAVA}.java

java-isofclass-02: A.class B.class C.java D.java E.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

