
include makeincl

oldname-01 \
oldname-02 : A.h A.cc 
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

clean: 
	rm -f *~ core *.o 

