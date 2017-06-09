
include makeincl

fctcall-mods-01 \
fctcall-mods-02: icode.o DelUserClass.o ApplyMod.o DefMod.o
	${CC} -o $(EXE) icode.o DelUserClass.o ApplyMod.o DefMod.o $(TESTLIB) ${OPTIONS}

icode.o: ApplyMod.h icode.cc init.cc
	cp -f ApplyMod.h icode.h
	${CC} -c $(MAIN) $(SUITE) ${INCL} $(CODE) ${OPTIONS}

DelUserClass.o: DelUserClass.cc
	${CC} -c DelUserClass.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

ApplyMod.o: ApplyMod.h ApplyMod.cc
	${CC} -c ApplyMod.cc $(SUITE) $(CODE) ${INCL}   ${OPTIONS}

DefMod.o: DefMod.h DefMod.cc
	${CC} -c DefMod.cc $(SUITE) $(CODE) ${INCL} ${OPTIONS}

clean: 
	rm -f *~ core *.o

