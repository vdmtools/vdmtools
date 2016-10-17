
include makeincl

assignstmt-01 assignstmt-02 assignstmt-03 assignstmt-04 \
assignstmt-05 assignstmt-06 assignstmt-%: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-assignstmt-01 javac-assignstmt-02 javac-assignstmt-03 javac-assignstmt-04 \
javac-assignstmt-05 javac-assignstmt-06 javac-assignstmt-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java
        

java-assignstmt-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class








