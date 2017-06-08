OSTYPE=$(shell uname)

VDMLIB = $(HOME)/vdmwork

ifeq ($(strip $(OSTYPE)),Darwin)
OSV = $(shell uname -r)
OSMV = $(word 1, $(subst ., ,$(strip $(OSV))))
CC      = /usr/bin/clang++
CXX     = /usr/bin/clang++
LIB     = -L. -L$(VDMLIB) -lvdm -lm -liconv
endif

ifeq ($(strip $(OSTYPE)),Linux)
CPUTYPE = $(shell uname -m)
CC      = /usr/bin/g++
CXX     = /usr/bin/g++
LIB     = -L. -L$(VDMLIB) -lvdm_shared -lm
endif

INCL = -I$(VDMLIB)

all: libcylio.so libmath.so

%.so:
ifeq ($(strip $(OSTYPE)),Darwin)
	$(CXX) -dynamiclib -fPIC -o $@ $^ $(LIB)
else
	$(CXX) -shared -fPIC -o $@ $^ $(LIB)
endif

libcylio.so: cylio.o tcfcylio.o 

cylio.o: cylio.cc
	${CXX} $(INCL) -c -fPIC -o $@ $< ${INCL}

tcfcylio.o: tcfcylio.cc
	${CXX} $(INCL) -c -fPIC -o $@ $< ${INCL}

libmath.so: tcfmath.o

tcfmath.o: tcfmath.cc
	${CXX} $(INCL) -c -fPIC -o $@  $<  ${INCL}

clean:
	rm -rf *.o *.so

