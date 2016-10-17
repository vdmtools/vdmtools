
TARGETS = trigno.vdm.tar.gz trigno.ps.gz

BASEFILE = trigno

trigno.vdm.tar.gz: cylinder.vdm cylio.vdm math.vdm Makefile.SunOS4 \
	Makefile.SunOS5 cylio.cc cylio.h main.cc tcfcylio.cc tcfmath.cc \
	trigno.tex

trigno.ps.gz: trigno.ps

trigno.ps: cylinder.vdm.tex cylio.vdm.tex math.vdm.tex

include ../Make.rules

