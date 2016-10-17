
include makeincl

setrange-% : A.h A.cc 
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-setrange-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-setrange-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

