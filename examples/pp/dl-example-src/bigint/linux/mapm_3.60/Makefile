#
# Generic Unix makefile for MAPM library version >= 3.50
#
# Run this makefile from the source subdirectory with
#   make -f makefile.unx
#
# Michael Ring
# ringx004@tc.umn.edu
#
# 9 July 2000

OSTYPE=$(shell uname)
OSVERSION=$(shell uname -r)
TARGETDIR=../../../../dl-example/mapm_3.60

ifeq ($(strip $(OSTYPE)),Darwin)
OSV = $(shell uname -r)
OSMV = $(word 1, $(subst ., ,$(strip $(OSV))))
CCPATH = /usr/bin/
ifeq ($(strip $(OSMV)),12) # 10.8
CC      = $(CCPATH)clang
SDK =
LIBNAME = lib_mapm_darwin.a
else
ifeq ($(strip $(OSMV)),11) # 10.7
CC      = $(CCPATH)clang
SDK =
LIBNAME = lib_mapm_darwin.a
else
ifeq ($(strip $(OSMV)),10) # 10.6
CC      = $(CCPATH)gcc
SDK =
LIBNAME = lib_mapm_darwin.a
else
ifeq ($(strip $(OSMV)),9) # 10.5
CC      = $(CCPATH)gcc-4.2 -arch i386 -arch x86_64 -arch ppc
SDK =
LIBNAME = lib_mapm_darwin.a
else
ifeq ($(strip $(OSMV)),8) # 10.4
CC      = $(CCPATH)gcc-4.0 -arch i386 -arch x86_64 -arch ppc
SDK =
LIBNAME = lib_mapm_darwin.a
else
CC      = $(CCPATH)gcc-3.3
SDK = -B/Developer/SDKs/MacOSX10.3.9.sdk
LIBNAME = lib_mapm_darwin_ppcg3.a
endif
endif
endif
endif
endif

#ifdef G3
#CC = gcc-3.3
#SDK = -B/Developer/SDKs/MacOSX10.3.9.sdk
#LIBNAME = lib_mapm_darwin_ppcg3.a
#else
#CC = gcc-4.0 -arch i386 -arch x86_64 -arch ppc
#SDK =
#LIBNAME = lib_mapm_darwin.a
#endif
CFLAGS = $(SDK) -Wall -Wno-unused -O2 -fPIC
endif

ifeq ($(strip $(OSTYPE)),Linux)
CPUTYPE = $(shell uname -m)
CC = gcc
CFLAGS = -Wall -Wno-unused -O2 -fPIC
ifeq ($(strip $(CPUTYPE)),x86_64)
LIBNAME = lib_mapm_linux_x86_64.a
else
LIBNAME = lib_mapm.a
endif
endif

ifeq ($(strip $(OSTYPE)),SunOS)
CC = gcc
CFLAGS = -Wall -Wno-unused -O2 -fPIC
LIBNAME = lib_mapm_solaris.a
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
CC = gcc
CFLAGS = -Wall -Wno-unused -O2 -fPIC
LIBNAME = lib_mapm_freebsd.a
endif

SHELL = /bin/sh
LFLAGS = -s

OBJS =	mapmhasn.o \
	mapmhsin.o \
	mapm_pow.o \
	mapm_log.o \
	mapm_exp.o \
	mapmasin.o \
	mapm_sin.o \
	mapm5sin.o \
	mapmrsin.o \
	mapmsqrt.o \
	mapmcbrt.o \
	mapmgues.o \
	mapmfact.o \
	mapmipwr.o \
	mapm_rnd.o \
	mapmstck.o \
	mapm_div.o \
	mapm_mul.o \
	mapmfmul.o \
	mapm_fft.o \
	mapm_add.o \
	mapmistr.o \
	mapm_set.o \
	mapmutil.o \
	mapmcnst.o

all: $(LIBNAME) calc primenum validate

calc: $(LIBNAME) calc.o
	$(CC) $(LFLAGS) -o calc calc.o $(LIBNAME) -lm
	rm -f calc.o

primenum: $(LIBNAME) primenum.o
	$(CC) $(LFLAGS) -o primenum primenum.o $(LIBNAME) -lm
	rm -f primenum.o

validate: $(LIBNAME) validate.o
	$(CC) $(LFLAGS) -o validate validate.o $(LIBNAME) -lm
	rm -f validate.o

$(LIBNAME): $(OBJS)
	rm -f $(LIBNAME)
	ar rc $(LIBNAME) $(OBJS)
ifeq ($(strip $(OSTYPE)),Darwin)
	ranlib $(LIBNAME)
endif
	rm -f $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

install: $(LIBNAME)
	cp $^ $(TARGETDIR)/.

clean: 
	rm -f *.o 
	rm -f *.a 
	rm -f calc primenum validate

