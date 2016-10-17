
include makeincl

modules-01 \
modules-02 \
modules-03 \
modules-04: A.o B.o C.o D.o E.o icode.o DelUserClass.o
	cp -f A.h icode.h
	${CC} -o $(EXE) A.o B.o C.o D.o E.o icode.o DelUserClass.o $(CODE) ${TESTLIB} ${TESTLIB2} ${OPTIONS}

modules-05 \
modules-06: A.o B.o C.o D.o E.o icode.o LAST.o
	cp -f A.h icode.h
	${CC} -o $(EXE) A.o B.o C.o D.o E.o icode.o LAST.o $(CODE) ${TESTLIB} ${TESTLIB2} ${OPTIONS}


icode.o: A.h icode.cc init.cc
	cp -f A.h icode.h
	${CC} -c $(MAIN) $(SUITE) ${INCL} $(CODE) ${OPTIONS}

DelUserClass.o: DelUserClass.cc
	${CC} -c DelUserClass.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

A.o: A.h A.cc
	${CC} -c A.cc $(SUITE) $(CODE) ${INCL}   ${OPTIONS}

B.o: B.h B.cc
	${CC} -c B.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

C.o: C.h C.cc
	${CC} -c C.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

D.o: D.h D.cc
	${CC} -c D.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

E.o: E.h E.cc
	${CC} -c E.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

LAST.o: LAST.h LAST.cc
	${CC} -c LAST.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

clean: 
	rm -f *~ core *.o

