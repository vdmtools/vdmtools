## This Makefile has been generated automatically
##
LIBDIR  = /usr/local/package/toolbox/cg/lib
SOURCES = /usr/local/package/toolbox/cg/include

CXX      = g++ -g $(OPTIMIZE)
CXXFLAGS = $(INCL)
INCL    = -I$(SOURCES)
LIB     = -L$(LIBDIR) -lvdm -lCG -liostream -lm

all: libdynsem.a

# Modules :  AS REP GLOBAL SEM STATE MOD DEF EXPR STMT PAT AUX FREE OLD SETTINGS
libdynsem.a(AS.o): AS.cc AS.h REP.h
libdynsem.a(REP.o): REP.cc REP.h AS.h
libdynsem.a(GLOBAL.o): GLOBAL.cc GLOBAL.h AS.h REP.h SEM.h
libdynsem.a(SEM.o): SEM.cc SEM.h AS.h REP.h
libdynsem.a(STATE.o): STATE.cc STATE.h AS.h GLOBAL.h REP.h SEM.h
libdynsem.a(MOD.o): MOD.cc MOD.h AS.h GLOBAL.h REP.h SEM.h
libdynsem.a(DEF.o): DEF.cc DEF.h AS.h GLOBAL.h REP.h SEM.h
libdynsem.a(EXPR.o): EXPR.cc EXPR.h AS.h GLOBAL.h REP.h SEM.h
libdynsem.a(STMT.o): STMT.cc STMT.h AS.h GLOBAL.h REP.h SEM.h
libdynsem.a(PAT.o): PAT.cc PAT.h AS.h GLOBAL.h REP.h SEM.h
libdynsem.a(AUX.o): AUX.cc AUX.h AS.h GLOBAL.h SEM.h REP.h
libdynsem.a(FREE.o): FREE.cc FREE.h AS.h REP.h SEM.h
libdynsem.a(OLD.o): OLD.cc OLD.h AS.h REP.h
libdynsem.a(SETTINGS.o): SETTINGS.cc SETTINGS.h

libdynsem.a: libdynsem.a(AS.o) libdynsem.a(REP.o) libdynsem.a(GLOBAL.o) libdynsem.a(SEM.o) libdynsem.a(STATE.o) libdynsem.a(MOD.o) libdynsem.a(DEF.o) libdynsem.a(EXPR.o) libdynsem.a(STMT.o) libdynsem.a(PAT.o) libdynsem.a(AUX.o) libdynsem.a(FREE.o) libdynsem.a(OLD.o) libdynsem.a(SETTINGS.o)
	-ranlib $@



clean: rm -f *~ *.o
