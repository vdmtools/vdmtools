include makeincl

math-01: A.h A.cc MATH.h MATH.cc
	cp -f A.h icode.h
	${CC} -o $(EXE)  $(SUITE) A.cc MATH.cc ${MAIN} ${OPTIONS}

clean:
	rm -f *~ core *.o *.class
