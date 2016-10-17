
include makeincl

SUITE = -I/home/hermes/cg/test/PP/depend/

depend-01 depend-02 depend-04: A.h A.cc B.h B.cc
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc ${MAIN} ${SUITE} ${OPTIONS}

depend-03 \
depend-07 \
depend-08 \
depend-09 \
depend-10 \
depend-11 \
depend-13: A.h A.cc B.h B.cc C.h C.cc
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc C.cc ${MAIN} ${SUITE} ${OPTIONS}

depend-05: A.cc B.cc Math.cc IA.cc ID.cc Itest.cc Itest2.cc Itest3.cc AA.cc
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc Math.cc IA.cc ID.cc Itest.cc Itest2.cc Itest3.cc AA.cc ${MAIN} ${SUITE} ${OPTIONS}

depend-06: A.cc B.cc C.cc MATH.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc C.cc MATH.cc ${MAIN} ${SUITE} ${OPTIONS}

depend-12: A.cc B.cc C.cc D.cc E.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc B.cc C.cc D.cc E.cc ${MAIN} ${SUITE} ${OPTIONS}

javac-depend-01 javac-depend-02 javac-depend-04: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-depend-01 java-depend-02 java-depend-04: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-depend-03 \
javac-depend-07 \
javac-depend-08 \
javac-depend-09 \
javac-depend-10 \
javac-depend-11 \
javac-depend-13: A.java B.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java ${MAINJAVA}.java

java-depend-03 \
java-depend-07 \
java-depend-08 \
java-depend-09 \
java-depend-10 \
java-depend-11 \
java-depend-13: A.class B.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-depend-05: A.java B.java vdm_Math.java IA.java ID.java Itest.java Itest2.java Itest3.java AA.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java vdm_Math.java IA.java ID.java Itest.java Itest2.java Itest3.java AA.java ${MAINJAVA}.java

java-depend-05 : A.class B.class vdm_Math.class IA.class ID.class AA.class Itest.class Itest2.class Itest3.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-depend-06: A.java B.java C.java MATH.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java MATH.java ${MAINJAVA}.java

java-depend-06 : A.class B.class C.class MATH.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-depend-12: A.java B.java C.java D.java E.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java D.java E.java ${MAINJAVA}.java

java-depend-12 : A.class B.class C.class D.class E.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

copyinterfacefile: 
	cp -f /home/jojo/toolbox/cgtest/PP/depend/javainterfaces javainterfaces

clean: 
	rm -f *~ core *.o *.class javainterfaces

