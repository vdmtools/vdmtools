
include makeincl

blockstmt-01 blockstmt-02 blockstmt-03: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-blockstmt-% : A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-blockstmt-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class




