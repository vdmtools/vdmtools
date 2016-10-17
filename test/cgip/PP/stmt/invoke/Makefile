
include makeincl

invoke-01 \
invoke-02 \
invoke-03 \
invoke-04 \
invoke-09 \
invoke-10 \
invoke-12 \
invoke-13 \
invoke-14 \
invoke-15 \
invoke-16 \
invoke-17 : A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-invoke-01 \
javac-invoke-02 \
javac-invoke-03 \
javac-invoke-04 \
javac-invoke-09 \
javac-invoke-10 \
javac-invoke-12 \
javac-invoke-13 \
javac-invoke-14 \
javac-invoke-15 \
javac-invoke-16 \
javac-invoke-17: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-invoke-01 \
java-invoke-02 \
java-invoke-03 \
java-invoke-04 \
java-invoke-09 \
java-invoke-10 \
java-invoke-12 \
java-invoke-13 \
java-invoke-14 \
java-invoke-15 \
java-invoke-16 \
java-invoke-17: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

invoke-11 \
invoke-05 \
invoke-06 \
invoke-07 \
invoke-08 : A.h A.cc B.h B.cc T20XXX.h T20XXX.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc T20XXX.cc ${MAIN} ${OPTIONS}


javac-invoke-11 \
javac-invoke-05 \
javac-invoke-06 \
javac-invoke-07 \
javac-invoke-08: A.java B.java T20XXX.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java T20XXX.java ${MAINJAVA}.java


java-invoke-11 \
java-invoke-05 \
java-invoke-06 \
java-invoke-07 \
java-invoke-08: A.class B.class T20XXX.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

invoke-18 \
invoke-19 : A.h A.cc B.h B.cc C.h C.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc ${MAIN} ${OPTIONS}

javac-invoke-18 \
javac-invoke-19: A.java B.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java ${MAINJAVA}.java

java-invoke-18 \
java-invoke-19: A.class B.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

invoke-20 : B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  B.cc ${MAIN} ${OPTIONS}

javac-invoke-20: B.java
	${JAVAC} -classpath ${CLASSPATH} B.java ${MAINJAVA}.java

java-invoke-20: B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class

