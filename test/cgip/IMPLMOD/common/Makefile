include makeincl
FPIC = -fpic

ifeq ($(OSTYPE),OSF1)
LD=$(CCC) -hidden -shared -o
FPIC =
endif

implmod-01: libmath.so
implmod-02: libmath.so
implmod-03: libmath.so
implmod-04: libmath.so
implmod-05: tcfmath2.o
	${LD} libmath2.so tcfmath2.o $(TESTLIB) 
implmod-06: libmath.so
implmod-07: inout1.o tcfinout1.o
	${LD} libio07.so inout1.o tcfinout1.o ${TESTLIB}

implmod-08: inout2.o tcfinout2.o
	${LD} libio08.so inout2.o tcfinout2.o ${TESTLIB}

implmod-09: inout1.o tcfinout3.o
	${LD} libio09.so inout1.o tcfinout3.o ${TESTLIB}

implmod-10: inout1.o tcfinout3.o libmath.so
	${LD} libio10.so inout1.o tcfinout3.o ${TESTLIB}

implmod-11: inout1.o tcfinout3.o libmath.so
	${LD} libio11.so inout1.o tcfinout3.o ${TESTLIB}

implmod-12: inout1.o tcfinout3.o libmath.so
	${LD} libio12.so inout1.o tcfinout3.o ${TESTLIB}

implmod-13: libmath.so


implmod-14: inout1.o tcfinout3.o libmath.so
	${LD} libio14.so inout1.o tcfinout3.o ${TESTLIB}

implmod-15: inout1.o tcfinout3.o libmath.so
	${LD} libio15.so inout1.o tcfinout3.o ${TESTLIB}

implmod-16: inout2.o tcfinout2.o
	${LD} libio16.so inout2.o tcfinout2.o ${TESTLIB}

implmod-17:
#       no shared library defined	

implmod-18:
#       no shared library defined	
implmod-19:
#       no shared library defined	

libmath.so: tcfmath.o
	${LD} libmath.so tcfmath.o ${TESTLIB} 

tcfcylio.cc: cylio.h
tcfinout1.cc: inout1.h
tcfinout2.cc: inout2.h
tcfinout3.cc: inout1.h

## object files
%.o : %.cc
	${CC} -c  -o $@ $< ${INCL}
SOURCE_DIR = .
## source files
%.cc : $(SOURCE_DIR)/%.cc
	cp $< .
%.h : $(SOURCE_DIR)/%.h
	cp $< .


clean:
	-rm -f libio07.so libio09.so libio11.so libio14.so libio16.so 
	-rm -f libmath2.so libio08.so libio10.so libio12.so libio15.so 
	-rm -f inout1.o tcfinout1.o tcfinout3.o tcfmath2.o # libmath.so
	-rm -f inout2.o tcfinout2.o tcfmath.o


