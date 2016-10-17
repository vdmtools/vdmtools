
include makeincl

instance-%: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-instance-%: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-instance-% : A.class ${MAINJAVA}.class B.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class
