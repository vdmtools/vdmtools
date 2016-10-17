##---------------------------------------------------------------
##                    Make file for Windows 32bit
##             This Makefile can only be used with GNU make
##---------------------------------------------------------------

CC       = cl.exe
MAPMDIR = ./mapm_3.60
TBDIR = D:/tbdev/jfits

CFLAGS   = /nologo /c /MT /W0 /GD /GX /D "WIN32" /TP
DLLFLAGS = /D "_USRDLL" 
INCPATH  = -I//hermes/paulm/toolbox/jfits/winnt \
           -I//hermes/paulm/manuals/doc/dlclass/example/win/mapm_3.60

LDFLAGS = "$(TBDIR)/CG.lib" "$(TBDIR)/vdm.lib" "$(MAPMDIR)/mapm.lib"

## DLL Specific binary, path and flag

LINK     = link.exe
LPATH    = /LIBPATH:D:/tbdev/jfits
LFLAGS   = /dll /incremental:no /DEFAULTLIB:vdm.lib

## CG Version Files

CGSOURCES = A.cpp A_anonym.cpp BankAccount.cpp BankAccount_anonym.cpp \
            BigInt.cpp BigInt_anonym.cpp CGBase.cpp 
CGOBJ = A.obj A_anonym.obj BankAccount.obj BankAccount_anonym.obj BigInt.obj \
        BigInt_anonym.obj CGBase.obj
CGFLAGS = -DCG #-DDEBUG

## CG Version specific rules

cgex.obj: A_userdef.h BankAccount_userdef.h 

cgex.exe: cgex.obj bigint_cg.obj $(CGOBJ)

bigint_cg.obj: bigint_dl.cpp
	$(CC) $(CFLAGS) -DCG $(INCPATH) bigint_dl.cpp -Fo"bigint_cg.obj"

## DLL specific  rules

bigint_dl.obj: bigint_dl.cpp
	$(CC) $(CFLAGS) $(DLLFLAGS) $(INCPATH) /Fo"$@" $<

bigint_dl.dll: bigint_dl.obj $(MAPMDIR)/mapm.lib

## Rules

%.dll:
	$(LINK) $(LPATH) $(LFLAGS) /out:"$@" $^

%.obj: %.cpp
	$(CC) $(CFLAGS) $(INCPATH) /Fo"$@" $<

%.exe: %.obj
	$(CC) $(LDFLAGS) /Fo"$@" $^

%.cpp: %.cc
	cp -f $^ $@

%_userdef.h:
	touch $@

clean:
	rm -f $(CGOBJ) bigint_cg.obj cgex.obj cgexe.exe
	rm -f bigint_dl.dll bigint_dl.obj
	rm -f *.cpp
	rm -f A.h A_anonym.h A_userdef.h
	rm -f BankAccount.h BankAccount_anonym.h BankAccount_userdef.h
	rm -f BigInt.h BigInt_anonym.h 
	rm -f CGBase.h
	rm -f cgex.exe cgex.lib cgex.exp
	rm -f bigint_dl.lib bigint_dl.exp




