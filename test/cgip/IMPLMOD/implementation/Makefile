include makeincl
FPIC=-fpic
ifeq ($(OSTYPE),OSF1)
LD=$(CCC) -shared -hidden -o
FPIC =
endif

imod-01: imod-01.o
	${LD} libimod-01.so imod-01.o ${TESTLIB}
imod-02: imod-02.o
	${LD} libimod-02.so imod-02.o ${TESTLIB}
imod-03: imod-03.o
	${LD} libimod-03.so imod-03.o ${TESTLIB}
imod-04: imod-04.o
	${LD} libimod-04.so imod-04.o ${TESTLIB}

clean:
	#rm imod-01.o imod-02.o imod-03.o imod-04.o
	#rm libimod-01.so libimod-02.so libimod-03.so libimod-04.so

## object files
%.o : %.cc
	${CC} -c $(FPIC) -o $@ $< ${INCL} 
## source files
%.cc : $(SOURCE_DIR)/%.cc
	cp $< .
