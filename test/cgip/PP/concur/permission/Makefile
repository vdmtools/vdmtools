
include makeincl

SUITE = -I/home/jeppe/toolbox/testdir/PP/lib

Finished.java MainThread.java WatchDog.java threadmain.java:
	cp -f $(BASEDIR)/$@ .

%.class: %.java
	${JAVAC} -classpath ${CLASSPATH} $< 

javac-permis-%: C.java T.java Finished.java MainThread.java WatchDog.java threadmain.java
	${JAVAC} -classpath ${CLASSPATH} C.java T.java Finished.java MainThread.java WatchDog.java threadmain.java

java-permis-%: C.class T.class Finished.class threadmain.class
	${JAVA} -classpath ${CLASSPATH} threadmain

clean: 
	rm -f *~ core *.o *.class

