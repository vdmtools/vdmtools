##---------------------------------------------------------------
##                    Make file for Linux
##---------------------------------------------------------------

MAPMDIR = ./mapm_3.60
VDMLIB  = /local2/paulm/jfits
INCL    = -I/local2/paulm/jfits -I${MAPMDIR}
VPPDE   = vppde -c

CXXFLAGS = $(INCL) 
CC      = g++ 
LIB     =  -L$(VDMLIB) -lvdm -lm 

CGSOURCES = A.cc A_anonym.cc BankAccount.cc BankAccount_anonym.cc \
            BigInt.cc BigInt_anonym.cc CGBase.cc 
CGFLAGS = -DCG #-DDEBUG

A.cc: A_userdef.h
BankAccount.cc: BankAccount_userdef.h

cgex:	cgex.cc ${CGSOURCES} bigint_cg.o ${MAPMDIR}/lib_mapm.a
	${CC} $(INCL) ${CGFLAGS} -o $@ $^ ${LIB}

%_userdef.h:
	touch $@

$(CGSOURCES): bigint.vpp
	$(VPPDE) $<


bigint_dl.so: bigint_dl.o ${MAPMDIR}/lib_mapm.a
bigint_dl.o: bigint_dl.cc

bigint_cg.o: bigint_dl.cc
	${CC} $(INCL) ${CGFLAGS} -c -o $@ $^
%.so:
	${CC} -shared -fpic $(EXCEPTION) -v -o $@ $^ ${LIB}

clean:
	rm -f bigint_dl.so bigint_dl.o bigint_cg.o
	rm -f cgex
	rm -f ${CGSOURCES}
	rm -f A.h A_anonym.h A_userdef.h
	rm -f BankAccount.h BankAccount_anonym.h BankAccount_userdef.h
	rm -f BigInt.h BigInt_anonym.h 
	rm -f CGBase.h
