##---------------------------------------------------------------
##                    Make file for Linux
##---------------------------------------------------------------

OSTYPE=$(shell uname)

MAPMDIR = ./mapm_3.60
INCL    = -I../../cg/include -I${MAPMDIR}

VPPDE = ../../bin/vppde
CXXFLAGS = $(INCL) 
DEBUG =-g
VDMLIB  = ../../cg/lib
CGFLAGS = -DCG #-DDEBUG

ifeq ($(strip $(OSTYPE)),Darwin)
OSV = $(shell uname -r)
OSMV = $(word 1, $(subst ., ,$(strip $(OSV))))
CCPATH = /usr/bin/
ifeq ($(strip $(OSMV)),12) # 10.8
CC      = $(CCPATH)clang++
CXX     = $(CCPATH)clang++
else
ifeq ($(strip $(OSMV)),11) # 10.7
CC      = $(CCPATH)clang++
CXX     = $(CCPATH)clang++
else
ifeq ($(strip $(OSMV)),10) # 10.6
CC      = $(CCPATH)g++
CXX     = $(CCPATH)g++
else
ifeq ($(strip $(OSMV)),9) # 10.5
CC      = $(CCPATH)g++-4.2
CXX     = $(CCPATH)g++-4.2
else
ifeq ($(strip $(OSMV)),8) # 10.4
CC      = $(CCPATH)g++-4.0
CXX     = $(CCPATH)g++-4.0
else
CC      = $(CCPATH)g++
CXX     = $(CCPATH)g++
endif
endif
endif
endif
endif

LIB     = -L. -L$(VDMLIB) -lCG -lvdm -lm -liconv

#ifeq ($(strip $(GCC_VERSION)),-3.3)
#MAPMLIB = lib_mapm_darwin_ppcg3.a
#else
MAPMLIB = lib_mapm_darwin.a
#endif
endif

ifeq ($(strip $(OSTYPE)),Linux)
CPUTYPE = $(shell uname -m)
CC      = /usr/bin/g++
CXX	= /usr/bin/g++
LIB     = -L. -L$(VDMLIB) -lCG -lvdm -lm
ifeq ($(strip $(CPUTYPE)),x86_64)
MAPMLIB = lib_mapm_linux_x86_64.a
else
MAPMLIB = lib_mapm.a
endif
endif

ifeq ($(strip $(OSTYPE)),SunOS)
CC      = /usr/sfw/bin/g++
CXX	= /usr/sfw/bin/g++
LIB     = -L. -L$(VDMLIB) -L../../lib -lCG -lvdm -lm
MAPMLIB = lib_mapm_solaris.a
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
CC      = /usr/bin/g++
CXX	= /usr/bin/g++
LIB     = -L. -L$(VDMLIB) -L../../lib -lCG -lvdm -lm -L/usr/local/lib -liconv
MAPMLIB = lib_mapm_freebsd.a
endif

OSTYPE2=$(word 1, $(subst _, ,$(strip $(OSTYPE))))
ifeq ($(strip $(OSTYPE2)),CYGWIN)
all: winall 
else
all: cg.stamp
	$(MAKE) cgex bigint_dl.so
endif
	$(MAKE) CGEX

ifeq ($(strip $(OSTYPE2)),CYGWIN)
CPUTYPE = $(shell uname -m)
CC      = /usr/bin/g++
CXX	= /usr/bin/g++
LIB     = -L. -L$(VDMLIB) -L../../lib -lCG -lvdm -liconv
ifeq ($(strip $(CPUTYPE)),x86_64)
MAPMLIB = lib_mapm_cygwin_x86_64.a
else
MAPMLIB = lib_mapm_cygwin_i686.a
endif
endif

CGSOURCES = A.cc A_anonym.cc BankAccount.cc BankAccount_anonym.cc \
            BigInt.cc BigInt_anonym.cc CGBase.cc 
CGOBJS = $(CGSOURCES:%.cc=%.o)

winall:
	make -f Makefile.win32

cgex:	cgex.o $(CGOBJS) bigint_cg.o $(MAPMDIR)/$(MAPMLIB)
ifeq ($(strip $(OSTYPE)),Darwin)
	ranlib $(MAPMDIR)/$(MAPMLIB)
endif
	$(CC) $(DEBUG) $(INCL) $(CGFLAGS) -o $@ $^ $(LIB)

cgex.o: cgex.cc A_userdef.h BankAccount_userdef.h cg.stamp
	$(CC) $(DEBUG) $(INCL) $(CGFLAGS) -c -o $@ cgex.cc 

%.o: %.cc
	$(CC) $(DEBUG) $(CCFLAGS) $(INCL) -c -o $@ $^

%_shared.o: %.cc
	$(CC) $(DEBUG) -fPIC $(CCFLAGS) $(INCL) -c -o $@ $^

cg.stamp: bigint.vpp
	$(VPPDE) -c $^
	touch $@

jcg.stamp: bigint.vpp
	$(VPPDE) -j $^
	touch $@

%_userdef.h:
	touch $@

bigint_dl.so: bigint_dl.o ${MAPMDIR}/$(MAPMLIB)

bigint_dl.o: bigint_dl.cc
ifeq ($(strip $(OSTYPE2)),CYGWIN)
	$(CC) $(DEBUG) $(CCFLAGS) $(INCL) -c -o $@ $^
else
	$(CC) $(DEBUG) -fPIC $(CCFLAGS) $(INCL) -c -o $@ $^
endif

bigint_cg.o: bigint_dl.cc
	$(CC) $(DEBUG) $(INCL) $(CGFLAGS) -c -o $@ $^

%.so:
ifeq ($(strip $(OSTYPE)),Darwin)
	ranlib ${MAPMDIR}/$(MAPMLIB)
	$(CXX) -dynamiclib -fPIC -o $@ $^ $(LIB)
else
	$(CXX) -shared -fPIC -o $@ $^ $(LIB)
endif

GENJAVAFILES = A.java BankAccount.java BigInt.java

CGEX.class: CGEX.java $(GENJAVAFILES)
CGEX: jcg.stamp CGEX.class $(GENJAVAFILES:%.java=%.class)

ifeq ($(strip $(OSTYPE)),Darwin)
%.class : %.java
	javac -J-Dfile.encoding=UTF-8 -encoding UTF8 -classpath ../../javacg/VDM.jar:. $<
else
ifeq ($(strip $(OSTYPE2)),CYGWIN)
%.class : %.java
	javac -classpath "../../javacg/VDM.jar;." $<
else
%.class : %.java
	javac -classpath ../../javacg/VDM.jar:. $<
endif
endif

ifeq ($(strip $(OSTYPE)),Darwin)
jrun:	jcg.stamp CGEX
	java -Dfile.encoding=UTF-8 -classpath ../../javacg/VDM.jar:. CGEX
else
ifeq ($(strip $(OSTYPE2)),CYGWIN)
jrun:	jcg.stamp CGEX
	java -classpath "../../javacg/VDM.jar;." CGEX
else
jrun:	jcg.stamp CGEX
	java -classpath ../../javacg/VDM.jar:. CGEX
endif
endif

clean:
	rm -f bigint_dl.so bigint_dl.o bigint_cg.o
	rm -f cgex cgex.o cg.stamp $(CGOBJS) $(CGOBJS:%.o=%_shared.o)
	rm -f $(CGSOURCES)
	rm -f A.h A_anonym.h A_userdef.h
	rm -f BankAccount.h BankAccount_anonym.h BankAccount_userdef.h
	rm -f BigInt.h BigInt_anonym.h 
	rm -f CGBase.h
	rm -f *.obj *.cpp *.exe *.exp *.lib
	rm -f A.java BankAccount.java BigInt.java *.class *.java.bak jcg.stamp
