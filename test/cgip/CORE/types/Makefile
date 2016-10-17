
include makeincl

types-% : A.h A.cc
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-types-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java  

java-types-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

