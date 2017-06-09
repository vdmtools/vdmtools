
include makeincl

destroy-%: B.h B.cc A.h A.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-destroy-%: B.java
	${JAVAC} -classpath ${CLASSPATH} B.java ${MAINJAVA}.java

java-destroy-% : B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

