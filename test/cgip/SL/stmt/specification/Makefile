
include makeincl

SUITE = -I/home/hermes/cg/test/PP/stmt/specification/

specification-01 \
specification-02 \
specification-03: A.h A.cc 
	cp -f A.h icode.h
	${CC} -g -o $(EXE)  A.cc ${MAIN} ${SUITE} ${OPTIONS}

clean: 
	rm -f *~ core *.o

