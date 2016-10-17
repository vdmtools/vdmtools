
include makeincl

error-% : A.h A.cc globalA.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc globalA.cc ${MAIN} ${OPTIONS}

javac-error-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-error-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class


