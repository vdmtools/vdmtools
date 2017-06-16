
include makeincl

#OPTIONS = $(SUITE) $(CODE) ${INCL} ${TESTLIB}
#EXE     = icode # included in makeincl
#MAIN    = icode.cc DelUserClass.cc # do


ifstmt-%: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

javac-ifstmt-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-ifstmt-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

