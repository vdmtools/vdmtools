# makefile 3.1 5/28/93

# HOST should be the name of your machine
HOST = SunSS1p

# CC should be the name of your favorite C compiler
CC = gcc

# set your compiler flag(s) here
CBFLAG1 = -O2
CBFLAG2 =
CBFLAG3 =
# the following compiler flags go at the end of the command line; use for
# flags that are required by the compiler to be at the end of the line
CEFLAG1 = -lm
CEFLAG2 = 

# you shouldn't have to change anything below this line
CBFLAGS = ${CBFLAG1} ${CBFLAG2} ${CBFLAG3}
CEFLAGS = ${CEFLAG1} ${CEFLAG2}

# where the results of the test script will be written
RESULTFILE = results.${HOST}.${CC}${CBFLAG1}${CBFLAG2}${CBFLAG3}${CEFLAG1}${CEFLAG2}

# a standard resultfile
RESULTSTD = results.SunSS1p.gcc.O2-lm.orig

INCS =  mrandom.h bentley.h pcrand.h ran0.h ran1.h ran2.h ultra.h xsq.h

SRCS = mrtest.c mrandom.c bentley.c pcrand.c ran0.c ran1.c ran2.c\
	ultra.c xsq.c

OBJS = mrandom.o bentley.o pcrand.o ran0.o ran1.o ran2.o ultra.o xsq.o

all:
	make mrandom.a
	make mrtest

mrandom.a: ${OBJS}
	ar r mrandom.a ${OBJS}
	ranlib mrandom.a
	chmod a+rwx mrandom.a

mrtest	: mrtest.c mrandom.a
	${CC} ${CBFLAGS} mrtest.c -o mrtest mrandom.a ${CEFLAGS}

mrtestv	: mrandom.a mrtest.c
	${CC} ${CBFLAGS} -DVECTOR mrtest.c -o mrtestv mrandom.a \
	  ${CEFLAGS}

mrtestv.s: ${OBJS} mrtest.c
	${CC} ${CBFLAGS} -S -DVECTOR mrtest.c \
	  -o mrtestv.s mrandom.a ${CEFLAGS}

mrtestg	: ${OBJS} mrtest.c
	${CC} ${CFLAGS} -g mrtest.c \
	  -o mrtestg mrandom.a ${CEFLAGS}

test	:
# force recompilation, just in case CFLAGS or CC have changed.
	/bin/rm -f ${OBJS} mrandom.a
# The following command should take a few minutes...
	./script > ${RESULTFILE}
#
# Since mrtest's output should be machine- and compiler independent,
# the following diff should show only differences in timing and
# in compiler invocation lines.  If you see anything else, please
# contact cthombor@mars.d.umn.edu.
# 
	-diff ${RESULTFILE} ${RESULTSTD}
#
# End of test.

clean	:
	rm -f *.o *.s
	rm -f mrtest mrtestv mrtestg mrtestv.s mrandom.a
	rm -f ${RESULTFILE} RNGstatefile

realclean	:
	make clean
	rm -f mrtest mrandom.a

