
include makeincl

new-06 : A.h A.cc B.h B.cc E.h E.cc D.h D.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc D.cc E.cc ${MAIN} ${OPTIONS}

new-08 : A.h A.cc B.h B.cc C.h C.cc D.h D.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc D.cc C.cc ${MAIN} ${OPTIONS}

new-09 new-10: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc ${MAIN} ${OPTIONS}

new-%: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-new-06: A.java B.java E.java D.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java E.java D.java ${MAINJAVA}.java

java-new-06: A.class B.class E.java D.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}


javac-new-08: A.java B.java C.java D.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java D.java C.java ${MAINJAVA}.java

java-new-08: A.class B.class C.java D.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-new-%: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-new-%: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}


clean: 
	rm -f *~ core *.o *.class

