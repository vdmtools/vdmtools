
include makeincl

numbers-% : Numbers.h Numbers.cc 
	cp -f Numbers.h icode.h
	${CC} -o $(EXE)  Numbers.cc ${MAIN} ${OPTIONS}

javac-numbers-% : Numbers.java
	${JAVAC} -classpath ${CLASSPATH} Numbers.java ${MAINJAVA}.java

java-numbers-% : Numbers.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

