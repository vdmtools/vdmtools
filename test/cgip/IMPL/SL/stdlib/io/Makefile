include makeincl

io-%: A.h A.cc IO.h IO.cc IO_anonym.cc 
	cp -f A.h icode.h
	${CC} -o $(EXE)  $(SUITE) A.cc IO.cc IO_anonym.cc ${MAIN} ${OPTIONS}
