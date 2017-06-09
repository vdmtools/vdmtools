
include makeincl

call-04 \
call-01 : A.h A.cc B.cc B.h
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc B.cc ${MAIN} ${OPTIONS}

call-02 \
call-03 : A.h A.cc 
	cp -f A.h icode.h
	${CC} -o $(EXE)  A.cc ${MAIN} ${OPTIONS}

clean: 
	rm -f *~ core *.o 

