
include makeincl

recordexpr-01 \
recordexpr-02 \
recordexpr-03 : A.h A.cc B.h B.cc
	cp -f B.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

clean: 
	rm -f *~ core *.o 

