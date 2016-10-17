
include makeincl

duration-01: A.h A.cc 
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-duration-%: A.java 
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-duration-%: A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}


clean: 
	rm -f *~ core *.o

