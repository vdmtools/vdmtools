
include makeincl

values-05: A.h A.cc B.h B.cc
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

values-%: A.h A.cc
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-values-05: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java  

java-values-05 : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-values-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java  

java-values-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

