
include makeincl

functions-%:	A.cc B.cc
		cp -f B.h icode.h
		${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}


javac-functions-%: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-functions-% : A.class B.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

