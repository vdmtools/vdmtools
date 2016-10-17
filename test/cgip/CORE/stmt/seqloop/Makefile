
include makeincl

seqloop-%: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-seqloop-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-seqloop-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

