
include makeincl

values-09 \
values-10 \
values-11 \
values-12: B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  B.cc ${MAIN} ${OPTIONS}

values-%: A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

javac-values-%: A.java
	${JAVAC} -classpath ${CLASSPATH} A.java ${MAINJAVA}.java

java-values-% : A.class ${MAINJAVA}.class
	${JAVA} -classpath ${CLASSPATH} ${MAINJAVA}

clean: 
	rm -f *~ core *.o *.class
