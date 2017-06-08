#
# This makefile is used to compile clients that uses the CORBA
# interface of the VDM Toolbox.  
# This file should be used with the MAKE tool on Win32 only.
# 
# Copyright IFAD 1998.  
#


# Edit this variable to reflect the location of your installation
OMNIDIR = I:/share/lib/omniORB_2.6.1
TBDIR="C:/Program Files/The IFAD VDM++ Toolbox V6.2"
CORBADIR=../corba

VDMAPI = $(CORBADIR)/metaiv_idlSK.obj $(CORBADIR)/corba_apiSK.obj $(CORBADIR)/corba_client.obj
VDMLIB = vdm.lib
WIN32LIB = user32.lib

CORBA_CPPFLAGS = -D__WIN32__ -D__x86__ -D__NT__ -D__OSVERSION__=4
INCDIR = -I. -I$(OMNIDIR)/include -I$(TBDIR)/cg/include -I$(TBDIR)/api/corba -I$(CORBADIR)
CORBA_LIB = wsock32.lib advapi32.lib \
	    $(TBDIR)/cg/lib/vdm.lib \
            $(OMNIDIR)/lib/x86_win32/omniORB260_rtd.lib \
            $(OMNIDIR)/lib/x86_win32/omnithread2_rtd.lib
CXX = cl /nologo 
CXXFLAGS = -MT -GX $(DEBUG) $(CORBA_CPPFLAGS)

#DEBUG = -Z7 -MTd
#DEBUGLINK = -debug
DEBUG = 
DEBUGLINK = 

COMPILEcc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(INCDIR) -c
##CXXLINKOPTIONS = $(DEBUGLINK) /RELEASE /INCREMENTAL:NO /PDB:NONE /NOLOGO \
##	     -align:0x1000 -subsystem:CONSOLE
CCOUT = -Fo
CCIN = -Tp
LNKOUT = -Fe
##LINK = link $(CXXLINKOPTIONS)


OMNIIDL2 = $(OMNIDIR)/bin/x86_win32/omniidl2
MEX = cmd /c mex

.SUFFIXES: .cc .obj

all: saferinterfpp saferinterf

allW95: saferinterfpp95 saferinterf95

saferinterfpp: saferinterfpp.cpp 
	cd ../corba; make -f Client.mk corba_apiSK.obj metaiv_idlSK.obj corba_client.obj
	$(MEX) -v -g -f ../mexopts-pp-NT4.bat saferinterfpp.cpp $(VDMAPI) $(VDMLIB) $(WIN32LIB)
	mv -f saferinterfpp.dll ../../matlab/gui

saferinterf: saferinterf.cpp
	cd ../corba; make -f Client.mk corba_apiSK.obj metaiv_idlSK.obj corba_client.obj
	$(MEX) -v -g -f ../mexopts-sl-NT4.bat saferinterf.cpp $(VDMAPI) $(VDMLIB) $(WIN32LIB)
	mv -f saferinterf.dll ../../matlab/gui

saferinterfpp95: saferinterfpp.cpp
	cd ../corba; make -f Client.mk corba_apiSK.obj metaiv_idlSK.obj corba_client.obj
	$(MEX) -v -g -f ../mexopts-pp-W95.bat saferinterfpp.cpp $(VDMAPI) $(VDMLIB) $(WIN32LIB)
	mv -f saferinterfpp.dll ../../matlab/gui

saferinterf95: saferinterf.cpp
	cd ../corba; make -f Client.mk corba_apiSK.obj metaiv_idlSK.obj corba_client.obj
	$(MEX) -v -g -f ../mexopts-sl-W95.bat saferinterf.cpp $(VDMAPI) $(VDMLIB) $(WIN32LIB)
	mv -f saferinterf.dll ../../matlab/gui



clean:
	rm -f *.obj *.pdb
	rm -f vc60.pdb
	cd ../corba make -f Client.mk clean

