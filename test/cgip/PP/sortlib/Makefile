
include makeincl

SUITE = -I/home/jeppe/toolbox/testdir/PP/sortlib

sortlib-01: 
	cp -f MainSort.h icode.h
	${CC} -g -o $(EXE)  MainSort.cc Sorter.cc ExplSort.cc MergeSort.cc DoSort.cc SortMachine.cc ImplSort.cc ${MAIN} ${SUITE} ${OPTIONS}

sortlib-02: 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc Sorter.cc ExplSort.cc MergeSort.cc DoSorting.cc SortMachine.cc ImplSort.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-sortlib-01:
	${JAVAC} -classpath ${CLASSPATH} MainSort.java Sorter.java ExplSort.java MergeSort.java DoSort.java SortMachine.java ImplSort.java ${MAINJAVA}.java

java-sortlib-01: MainSort.class Sorter.class ExplSort.class MergeSort.class DoSort.class SortMachine.class ImplSort.java ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

java-sortlib-02: A.class Sorter.class ExplSort.class ImplSort.class MergeSort.class SortMachine.class DoSorting.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA} 

javac-sortlib-02: A.java Sorter.java ExplSort.java ImplSort.java MergeSort.java SortMachine.java DoSorting.java
	${JAVAC} -classpath ${CLASSPATH} A.java Sorter.java ExplSort.java ImplSort.java MergeSort.java SortMachine.java DoSorting.java ${MAINJAVA}.java


clean: 
	rm -f *~ core *.o

