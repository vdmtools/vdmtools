
include makeincl

exception-%: A.h
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc A_anonym.cc ${MAIN} ${OPTIONS}

javac-exception-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-exception-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

