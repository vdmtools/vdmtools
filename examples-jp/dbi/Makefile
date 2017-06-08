UNAME = $(shell uname)

ifeq ($(word 1, $(subst _, ,$(strip $(UNAME)))),CYGWIN)
OSTYPE=CYGWIN
else
OSTYPE=$(UNAME)
endif

VDMTBDIR ?= $(HOME)/vdmwork

GCC     = $(CC)
CXX     = $(CCC)

INCDIR    = $(VDMTBDIR)
LIBDIR    = $(VDMTBDIR)

CFLAGS	= -g -I$(INCDIR)
CCFLAGS	= $(CFLAGS)
CXXFLAGS= $(CCFLAGS)

all: dbi.so

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

LIBFLAGS = -L. -L$(LIBDIR) -lvdm_darwin -liconv -liodbc -Wl,-syslibroot,$(SDK) -Wno-return-type-c-linkage

dbi.so: dbi.cc
	${CXX} $(CXXFLAGS) $(ARCH) -dynamiclib -fPIC -o $@ $< $(LIBFLAGS)
endif

ifeq ($(strip $(OSTYPE)),Linux)
CCPATH = /usr/bin/
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++

LIBFLAGS = -L. -L$(LIBDIR) -lvdm_shared -lodbc

dbi.so: dbi.cc
	${CXX} $(CXXFLAGS) -shared -fPIC -o $@ $< $(LIBFLAGS)
endif

#ifeq ($(strip $(OSTYPE)),CYGWIN_NT-5.1)
ifeq ($(strip $(OSTYPE)),CYGWIN)
TBDIRWIN=$(shell cygpath -w /cygdrive/c/cygwin$(VDMTBDIR))
#WININCDIR = $(TBDIRWIN)/cg/include
WININCDIR = .
#WINLIBDIR = $(TBDIRWIN)/cg/lib
WINLIBDIR = .
VDMLIB = $(WINLIBDIR)/vdm.lib

DLL_CC = cl.exe
DLL_LINK = link.exe /DLL /nologo
DLLFLAGS = /D "_USRDLL"
DLL_LFLAGS   = /incremental:no /NODEFAULTLIB /MANIFEST
DLL_CFLAGS = /nologo /c /Zi /MD /W0 /EHsc /TP -D_UNICODE /I$(WININCDIR)
DLLWINLIBS = msvcrt.lib msvcprt.lib \
             kernel32.lib user32.lib advapi32.lib delayimp.lib oldnames.lib \
             odbc32.lib odbccp32.lib

MT=mt.exe
dbi.obj: %.obj: %.cc
	$(DLL_CC) $(DLL_CFLAGS) $(DLLFLAGS) $(DLL_INCPATH) /Fo"$@" $<

dbi.so: %.so: %.obj
	#$(DLL_LINK) $(DLL_LFLAGS) /out:"$@" $(DLL_LPATH) vdm.lib $(DLLWINLIBS) $<
	$(DLL_LINK) $(DLL_LFLAGS) /out:"$@" $(DLL_LPATH) $(VDMLIB) $(DLLWINLIBS) $<
	$(MT) -manifest $@.manifest -outputresource:$@
endif

clean:
	rm -rf *.o *.dSYM *.obj *.idb *.pdb *.class
	rm -rf dbi.so dbi.lib dbi.exp dbi.so.manifest

