##---------------------------------------------------------------
##                    Make file for Linux
##---------------------------------------------------------------

VDMLIB  = /home/paulm/toolbox/work/linux
INCL    = -I/home/paulm/toolbox/work/linux -I.

CXXFLAGS = $(INCL) 
CC      = g++ 
LIB     =  -L$(VDMLIB) -lvdm -lm 

%.so: %.cc
	${CC} $(INCL) -shared -fpic $(EXCEPTION) -o $@ $^ ${LIB}

clean:
	rm -f *.o *.so
