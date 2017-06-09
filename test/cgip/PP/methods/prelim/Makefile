
include makeincl

SUITE = -I/home/hermes/cg/test/PP/methods/prelim/

prelim-02: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc   -DDEF_A_USERIMPL -DDEF_B_USERIMPL -DPRELIM02 ${MAIN} ${SUITE} ${OPTIONS}

prelim-01: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc  -DDEF_A_USERIMPL -DPRELIM01 ${MAIN} ${SUITE} ${OPTIONS}

prelim-03: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc  -DDEF_B_USERIMPL  -DPRELIM03 ${MAIN} ${SUITE} ${OPTIONS}

prelim-04: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc   -DDEF_A_USERIMPL -DDEF_B_USERIMPL  -DPRELIM04 ${MAIN} ${SUITE} ${OPTIONS}

prelim-05: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc -DDEF_A_USERIMPL -DPRELIM05 ${MAIN} ${SUITE} ${OPTIONS}

javac-prelim-01: A.java B.java
	cp -f external_A.java external_A.java.default
	cp -f $(BASEDIR)/../cgtestcases/PP/methods/prelim/external_A.java external_A.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-prelim-01: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}


javac-prelim-02: A.java B.java
	cp -f external_B.java external_B.java.default
	cp -f $(BASEDIR)/../cgtestcases/PP/methods/prelim/external_B.java external_B.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-prelim-02: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-prelim-03: A.java B.java
	cp -f external_B.java external_B.java.default
	cp -f $(BASEDIR)/../cgtestcases/PP/methods/prelim/external_B.java external_B.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-prelim-03: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-prelim-04 \
javac-prelim-05: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-prelim-04 \
java-prelim-05: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class external_A.java external_B.java

