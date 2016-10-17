
include makeincl

SUITE = -I/home/cg/test/SL/state/

rename-01: A.h A.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} $(SUITE) ${OPTIONS}

clean: 
	rm -f *~ core *.o

