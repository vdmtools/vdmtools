
include makeincl

modules-01 \
modules-02 \
modules-03 \
modules-04: A.o B.o C.o D.o E.o icode.o DelUserClass.o
	cp -f A.h icode.h
	${CC} -o $(EXE) A.o B.o C.o D.o E.o icode.o DelUserClass.o $(CODE) ${TESTLIB} ${TESTLIB2} 

modules-05 \
modules-06: A.o B.o icode.o
	cp -f A.h icode.h
	${CC} -o $(EXE) A.o B.o icode.o  $(CODE) ${TESTLIB} ${TESTLIB2} 


icode.o: A.h icode.cc init.cc
	cp -f A.h icode.h
	${CC} -c $(MAIN) $(SUITE) ${INCL} $(CODE) 

DelUserClass.o: DelUserClass.cc
	${CC} -c DelUserClass.cc $(SUITE) $(CODE) ${INCL} 

A.o: A.h A.cc
	${CC} -c A.cc $(SUITE) $(CODE) ${INCL}   

B.o: B.h B.cc
	${CC} -c B.cc $(SUITE) $(CODE) ${INCL} 

C.o: C.h C.cc
	${CC} -c C.cc $(SUITE) $(CODE) ${INCL} 

D.o: D.h D.cc
	${CC} -c D.cc $(SUITE) $(CODE) ${INCL} 

E.o: E.h E.cc
	${CC} -c E.cc $(SUITE) $(CODE) ${INCL} 


clean: 
	rm -f *~ core *.o

