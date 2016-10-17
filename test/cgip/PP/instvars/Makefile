
include makeincl

SUITE = -I/home/hermes/cg/test/PP/instvars/

instvars-01 \
instvars-02 \
instvars-03 \
instvars-04 \
instvars-05 \
instvars-06 \
instvars-07 \
instvars-08 \
instvars-09 \
instvars-10 \
instvars-11 \
instvars-13 \
instvars-14 \
instvars-15: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${SUITE} ${OPTIONS}


instvars-12: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-instvars-%: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-instvars-% : A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

