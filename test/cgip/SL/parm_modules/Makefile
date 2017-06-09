
include makeincl

SUITE = -I/home/hermes/cg/test/CORE/modules/

parm_modules-01: A.h A.cc Main.cc Main.h Sort.cc Sort.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc Main.cc Sort.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-02: A.h A.cc BASEDOM.cc BASEDOM.h 
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc BASEDOM.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-03: A.h A.cc MAIN.cc MAIN.h SORT.cc SORT.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc MAIN.cc SORT.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-04: A.h A.cc MAIN.cc MAIN.h TYPES.cc TYPES.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc MAIN.cc TYPES.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-05: A.h A.cc MAIN.cc MAIN.h VALUES.cc VALUES.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc MAIN.cc VALUES.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-06: A.h A.cc MAIN.cc MAIN.h VALUES.cc VALUES.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc MAIN.cc VALUES.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-07: A.h A.cc MAIN.cc MAIN.h TYPES.cc TYPES.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc MAIN.cc TYPES.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-08: A.h A.cc PAIRS.cc PAIRS.h 
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc PAIRS.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-09: A.h A.cc MAIN.cc MAIN.h SORT.cc SORT.h COMPARE.cc COMPARE.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc MAIN.cc SORT.cc COMPARE.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

parm_modules-10: A.h A.cc PAIRS.cc PAIRS.h RELATIONS.cc RELATIONS.h
	cp -f A.h icode.h
	${CC} -o $(EXE) A.cc PAIRS.cc RELATIONS.cc ${MAIN} -D$(MODE) $(SUITE) ${OPTIONS}

clean: 
	rm -f *~ core *.o

