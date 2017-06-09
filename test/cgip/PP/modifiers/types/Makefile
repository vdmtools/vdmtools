
include makeincl

types-09 \
types-10 \
types-11 \
types-12 \
types-19 \
types-20 \
types-21 \
types-22 : B.cc B.h
	cp -f B.h icode.h
	${CC} -o $(EXE)  B.cc ${MAIN} ${OPTIONS}

types-%: A.h A.cc B.cc B.h
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-types-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-types-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class
