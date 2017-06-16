
include makeincl

SUITE = -I/home/cg/test/CORE/functions/

functions-%: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

javac-functions-02 \
javac-functions-14: A.java
	cp -f external_A.java external_A.java.default
	cp -f $(BASEDIR)/../cgtest/CORE/functions/external_A.java external_A.java 
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java  

javac-functions-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java  

java-functions-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class external_A.java

