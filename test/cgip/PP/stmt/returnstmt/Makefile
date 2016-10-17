
include makeincl

returnstmt-01 returnstmt-07: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

returnstmt-02: A.h A.cc B.h B.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-returnstmt-07 \
javac-returnstmt-01: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-returnstmt-07 \
java-returnstmt-01: A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-returnstmt-02: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-returnstmt-02: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

