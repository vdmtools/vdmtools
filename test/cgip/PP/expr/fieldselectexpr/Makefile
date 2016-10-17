
include makeincl

fieldselectexpr-%: A.h A.cc B.h B.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc B.cc  ${MAIN} ${OPTIONS}

javac-fieldselectexpr-%: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-fieldselectexpr-%: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

