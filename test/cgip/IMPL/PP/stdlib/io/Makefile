include makeincl

io-01: A.h A.cc IO.h IO.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  $(SUITE) A.cc IO.cc ${MAIN} ${OPTIONS}
