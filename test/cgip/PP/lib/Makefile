
include makeincl

SUITE = -I/home/jeppe/toolbox/testdir/PP/lib

lib-01: 
	cp -f MainSort.h icode.h
	${CC} -g -o $(EXE)  MainSort.cc Sorter.cc ExplSort.cc MergeSort.cc DoSorting.cc SortMachine.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-lib-01:
	${JAVAC} -classpath ${CLASSPATH} MainSort.java Sorter.java ExplSort.java MergeSort.java DoSorting.java SortMachine.java ${MAINJAVA}.java

java-lib-01: MainSort.class Sorter.class ExplSort.class MergeSort.class DoSorting.class SortMachine.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o

