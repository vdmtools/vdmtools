
include makeincl

mapseqapply-%: A.h A.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-mapseqapply-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-mapseqapply-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

