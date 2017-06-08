#
# Makefile for the code generated NewQuadrilateral example.
#
# REMEMBER to change the macro TBDIR to fit your directory
# structure.
#
# Note that this version of the code generator must be used 
# with g++ version 2.7.2
#

CC      = g++
CCC     = g++
TBDIR   = /opt/vdm++_toolbox
INCL    = -I$(TBDIR)/cg/include
LIB     = -L$(TBDIR)/cg/lib -lvdm -lCG -lm

CFLAGS	= $(INCL)
CCFLAGS	= $(CFLAGS)
CXXFLAGS= $(CCFLAGS)

all: app

app: app.o WorkSpace.o Vector.o Mathematics.o Square.o \
           Quadrilateral.o  Parallelogram.o CGBase.o \
           Rhombus.o Rectangle.o
	${CC} -o app app.o WorkSpace.o Vector.o Mathematics.o \
                 Quadrilateral.o Parallelogram.o Square.o \
                 Rhombus.o Rectangle.o CGBase.o ${LIB}

app.o:           app.cc WorkSpace.h Quadrilateral.h \
                 Parallelogram.h Square.h 
Mathematics.o:   Mathematics.h Mathematics.cc vdm_Mathematics_implicit.cc
Vector.o:        Vector.h Vector.cc Mathematics.h 
Quadrilateral.o: Quadrilateral.h Quadrilateral.cc \
                 Vector.h vdm_Quadrilateral_prelim.cc
Parallelogram.o: Quadrilateral.h Parallelogram.h Parallelogram.cc 
Rectangle.o:     Rectangle.h Rectangle.cc Parallelogram.h 
Rhombus.o:       Rhombus.h Rhombus.cc Rectangle.h 
WorkSpace.o:     WorkSpace.h WorkSpace.cc Vector.h \
                 Quadrilateral.h vdm_Quadrilateral_prelim.cc
CGBase.o:        CGBase.cc CGBase.h

clean: 
	rm -f *.o app
