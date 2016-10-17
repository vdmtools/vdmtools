
include makeincl

rep-01 \
rep-02 \
rep-03 \
rep-04 \
rep-05 \
rep-06 \
rep-07 \
rep-12 : A.h A.cc B.h B.cc C.h C.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc ${MAIN} ${OPTIONS}

javac-rep-01 \
javac-rep-02 \
javac-rep-03 \
javac-rep-04 \
javac-rep-05 \
javac-rep-06 \
javac-rep-07 \
javac-rep-12: A.java B.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java ${MAINJAVA}.java

java-rep-01 \
java-rep-02 \
java-rep-03 \
java-rep-04 \
java-rep-05 \
java-rep-06 \
java-rep-07 \
java-rep-12: A.class B.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-08 \
rep-09 \
rep-10 \
rep-13 \
rep-14 \
rep-15 \
rep-16 \
rep-17 \
rep-18 \
rep-60: A.h A.cc B.h B.cc C.h C.cc D.h D.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc D.cc ${MAIN} ${OPTIONS}

rep-61: A.h A.cc A0.h A0.cc B.h B.cc 
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc A0.cc B.cc ${MAIN} ${OPTIONS}

javac-rep-08 \
javac-rep-09 \
javac-rep-10 \
javac-rep-13 \
javac-rep-14 \
javac-rep-15 \
javac-rep-16 \
javac-rep-17 \
javac-rep-18 \
javac-rep-60: A.java B.java C.java D.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java D.java ${MAINJAVA}.java

javac-rep-61: A.java B.java A0.java 
	${JAVAC} -classpath ${CLASSPATH} A.java B.java A0.java ${MAINJAVA}.java

javac-rep-64: A.java B.java A0.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java A0.java C.java ${MAINJAVA}.java

java-rep-08 \
java-rep-09 \
java-rep-10 \
java-rep-13 \
java-rep-14 \
java-rep-15 \
java-rep-16 \
java-rep-17 \
java-rep-18 \
java-rep-60: A.class B.class C.class D.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

java-rep-61: A.class A0.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

java-rep-64: A.class A0.class B.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}


rep-20 : B.h B.cc C.h C.cc D.h D.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  B.cc C.cc D.cc ${MAIN} ${OPTIONS}

javac-rep-20: B.java C.java D.java
	${JAVAC} -classpath ${CLASSPATH} B.java C.java D.java ${MAINJAVA}.java

java-rep-20: B.class C.class D.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-11 \
rep-62 : A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

rep-63 : A.h A.cc B.h B.cc E.h E.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc E.cc ${MAIN} ${OPTIONS}

rep-64 : A.h A.cc B.h B.cc A0.cc A0.h C.h C.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc A0.cc C.cc ${MAIN} ${OPTIONS}

javac-rep-11: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-rep-11: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

javac-rep-62: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-rep-62: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-59 \
rep-28 \
rep-30 \
rep-32 \
rep-34 rep-35 rep-36 \
rep-33 : A.h A.cc B.h B.cc C.h C.cc D.h D.cc E.cc E.h
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc D.cc E.cc ${MAIN} ${OPTIONS}

javac-rep-59 \
javac-rep-28 \
javac-rep-30 \
javac-rep-32 \
javac-rep-34 javac-rep-35 javac-rep-36 \
javac-rep-33: A.java B.java C.java D.java E.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java D.java E.java ${MAINJAVA}.java

javac-rep-63: A.java B.java E.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java E.java ${MAINJAVA}.java


java-rep-59 \
java-rep-28 \
java-rep-30 \
java-rep-32 \
java-rep-34 java-rep-35 java-rep-36 \
java-rep-33: A.class B.class C.class D.class E.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

java-rep-63: A.class B.class E.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}


rep-37 rep-38 rep-27 : B.h B.cc C.h C.cc D.h D.cc 
	cp -f B.h icode.h
	${CC} -o $(EXE)  B.cc C.cc D.cc ${MAIN} ${OPTIONS}

javac-rep-37 javac-rep-38 javac-rep-27: B.java C.java D.java
	${JAVAC} -classpath ${CLASSPATH} B.java C.java D.java ${MAINJAVA}.java

java-rep-37 java-rep-38 java-rep-27: B.class C.class D.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-39 : B.h B.cc D.h D.cc 
	cp -f B.h icode.h
	${CC} -o $(EXE)  B.cc D.cc ${MAIN} ${OPTIONS}

javac-rep-39: D.java B.java
	${JAVAC} -classpath ${CLASSPATH} D.java B.java ${MAINJAVA}.java

java-rep-39: D.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-40 \
rep-47 : B.h B.cc VALUE1.h VALUE1.cc VALUE2.h VALUE2.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  VALUE1.cc VALUE2.cc B.cc ${MAIN} ${OPTIONS}

javac-rep-40 javac-rep-47: B.java VALUE1.java VALUE2.java
	${JAVAC} -classpath ${CLASSPATH} B.java VALUE1.java VALUE2.java ${MAINJAVA}.java

java-rep-40 java-rep-47: B.class VALUE1.class VALUE2.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-42 : A.h A.cc B.h B.cc C.h C.cc 
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc  ${MAIN} ${OPTIONS}

javac-rep-42: A.java B.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java ${MAINJAVA}.java

java-rep-42: A.class B.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-41 \
rep-43 : TYPE1.h TYPE1.cc TYPE2.h TYPE2.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  TYPE1.cc TYPE2.cc B.cc  ${MAIN} ${OPTIONS}

javac-rep-41 javac-rep-43: TYPE1.java TYPE2.java
	${JAVAC} -classpath ${CLASSPATH} TYPE1.java TYPE2.java ${MAINJAVA}.java

java-rep-41 java-rep-43: TYPE1.class TYPE2.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-44 \
rep-45 \
rep-46 : TYPE1.h TYPE1.cc TYPE2.h TYPE2.cc VALUE1.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  TYPE1.cc TYPE2.cc B.cc VALUE1.cc ${MAIN} ${OPTIONS}

javac-rep-44 javac-rep-45 javac-rep-46: TYPE1.java TYPE2.java VALUE1.java B.java
	${JAVAC} -classpath ${CLASSPATH} TYPE1.java TYPE2.java VALUE1.java B.java ${MAINJAVA}.java

java-rep-44 java-rep-45 java-rep-46: TYPE1.class TYPE2.class VALUE1.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-48 \
rep-49 \
rep-50 \
rep-51 \
rep-52 \
rep-53 \
rep-54 \
rep-55 \
rep-65 \
rep-58: A.h A.cc B.h B.cc 
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-rep-48 \
javac-rep-49 \
javac-rep-50 \
javac-rep-51 \
javac-rep-52 \
javac-rep-53 \
javac-rep-54 \
javac-rep-55 \
javac-rep-65 \
javac-rep-58: A.java B.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java ${MAINJAVA}.java

java-rep-48 \
java-rep-49 \
java-rep-50 \
java-rep-51 \
java-rep-52 \
java-rep-53 \
java-rep-54 \
java-rep-55 \
java-rep-65 \
java-rep-58: A.class B.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-56: AA.h AA.cc B.h B.cc D.h D.cc A.h A.cc C.h C.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  AA.cc B.cc C.cc D.cc A.cc ${MAIN} ${OPTIONS}

javac-rep-56: A.java B.java D.java AA.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java D.java AA.java C.java ${MAINJAVA}.java

java-rep-56: A.class B.class D.class AA.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

rep-57: A.h A.cc B.h B.cc C.h C.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc C.cc ${MAIN} ${OPTIONS}

javac-rep-57: A.java B.java C.java
	${JAVAC} -classpath ${CLASSPATH} A.java B.java C.java ${MAINJAVA}.java

java-rep-57: A.class B.class C.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class
