
include makeincl

casesexpr-%: A.h A.cc 
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-casesexpr-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-casesexpr-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

