
include makeincl

values-01 \
values-02 \
values-03 : VALUE1.o VALUE2.o B.o icode.o 
	cp -f B.h icode.h
	${CC} -o $(EXE) VALUE1.o VALUE2.o B.o  icode.o  $(CODE) ${TESTLIB} ${LIB2} 

icode.o: A.h icode.cc init.cc
	cp -f B.h icode.h
	${CC} -c -g $(MAIN) $(SUITE) ${INCL} $(CODE) 

DelUserClass.o: DelUserClass.cc
	${CC} -c DelUserClass.cc $(SUITE) $(CODE) ${INCL} 


VALUE1.o: VALUE1.h VALUE1.cc
	${CC} -c -g VALUE1.cc $(SUITE) $(CODE) ${INCL}   

B.o: B.h B.cc
	${CC} -c B.cc -g $(SUITE) $(CODE) ${INCL} 

VALUE2.o: VALUE2.h VALUE2.cc
	${CC} -c VALUE2.cc -g $(SUITE) $(CODE) ${INCL} 

clean: 
	rm -f *~ core *.o

