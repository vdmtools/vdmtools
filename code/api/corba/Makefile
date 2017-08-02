#
# This makefile is used to compile clients that uses the CORBA
# interface of the VDM Toolbox under Linux.  
# Clients using this makefile *must* use omniORB 4.0 or later
# implementations. Consult the user manual for further details.
#
# 
# Copyright Kyushu University
#

# NOTE:

# The variable OMNIDIR must point to the installation directory of
# omniORB, and TBDIR must point to the installation directory of the
# VDM Toolbox

OSTYPE=$(shell uname)

# Edit these variables to reflect the location of your installation
TBDIR = ../..
TBINC = $(TBDIR)/cg/include
TBLIB = $(TBDIR)/cg/lib
 
ifeq ($(strip $(OSTYPE)),Darwin)
OMNIDIR ?= /usr/local/omniORB
OSV = $(shell uname -r)
OSMV = $(word 1, $(subst ., ,$(strip $(OSV))))
CCPATH = /usr/bin/
ifeq ($(strip $(OSMV)),12) # 10.8
CCC     = $(CCPATH)clang++
else
ifeq ($(strip $(OSMV)),11) # 10.7
CCC     = $(CCPATH)clang++
else
ifeq ($(strip $(OSMV)),10) # 10.6
CCC     = $(CCPATH)g++
else
MACOSX_DEPLOYMENT_ARCH?=$(shell uname -p) # i386 or powerpc
ifeq ($(strip $(MACOSX_DEPLOYMENT_ARCH)),i386)
ARCH ?= -arch i386
else
ARCH ?= -arch ppc
endif
ifeq ($(strip $(OSMV)),9) # 10.5
CCC     = $(CCPATH)g++-4.2
else
CCC     = $(CCPATH)g++
endif
endif
endif
endif

CXX = $(CCC) $(ARCH)
CORBA_CPPFLAGS =
CORBA_LIB = -lomniORB4 -lomnithread -lvdm -lpthread -lomniDynamic4 -L$(OMNIDIR)/lib/$(OMNIARCH) -L$(TBLIB) -liconv
endif

ifeq ($(strip $(OSTYPE)),Linux)
OMNIDIR ?= /usr/local/omniORB
CXX = /usr/bin/g++
CORBA_CPPFLAGS =
CORBA_LIB = -lomniORB4 -lomnithread -lvdm -lpthread -lomniDynamic4 -L$(OMNIDIR)/lib/$(OMNIARCH) -L$(TBLIB)
endif

ifeq ($(strip $(OSTYPE)),SunOS)
OMNIDIR ?= /usr/local/omniORB
CXX = /usr/sfw/bin/g++
CORBA_CPPFLAGS =
CORBA_LIB = -lomniORB4 -lomnithread -lvdm -lpthread -lomniDynamic4 -L$(OMNIDIR)/lib/$(OMNIARCH) -L$(TBLIB) -lposix4 -lsocket
endif

ifeq ($(strip $(OSTYPE)),FreeBSD)
OMNIDIR = /usr/local
CXX = /usr/bin/g++
CORBA_CPPFLAGS =
CORBA_LIB = -lomniORB4 -lomnithread -lvdm -lpthread -lomniDynamic4 -L$(OMNIDIR)/lib/$(OMNIARCH) -L$(TBLIB) -liconv
endif

OSTYPE2=$(word 1, $(subst _, ,$(strip $(OSTYPE))))
ifeq ($(strip $(OSTYPE2)),CYGWIN)
all: winmake client_example.jar
else
all:  client_example-pp client_example-sl client_example.jar
endif

ifndef JDKHOME
ifdef JAVA_HOME
JDKHOME ?= $(JAVA_HOME)
JAR = $(JDKHOME)/bin/jar
JAVAC = $(JDKHOME)/bin/javac
else
JAR = jar
JAVAC = javac
endif
else
JAR = $(JDKHOME)/bin/jar
JAVAC = $(JDKHOME)/bin/javac
endif

INCDIR = -I. -I$(OMNIDIR)/include -I$(TBINC)  #-I~/toolbox/code/lib/

CXXFLAGS = -Wall -Wno-unused $(DEBUG) $(CORBA_CPPFLAGS)
DEBUG = -g -DDEBUG_ECHO
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(INCDIR) -c

CCOUT = -o
CCIN = 
LNKOUT = -o
LINK = $(CXX)
OMNIIDL = $(OMNIDIR)/bin/omniidl
IDLCCFLAGS = -bcxx -Wbs=SK.cc

winmake:
	make -f Makefile.win32

%-sl.o: %.cc
	$(COMPILE.cc) -DVDMSL $(CCIN)$< $(CCOUT) $@

%-pp.o: %.cc
	$(COMPILE.cc) -DVDMPP $(CCIN)$< $(CCOUT) $@

corba_apiSK.cc: corba_api.idl metaiv_idlSK.o
	$(OMNIIDL)  $(IDLCCFLAGS) $<

metaiv_idlSK.cc: metaiv_idl.idl
	$(OMNIIDL)  $(IDLCCFLAGS) $<

client_example-%.o: corba_apiSK.cc metaiv_idlSK.cc corba_client.h


client_example-%: client_example-%.o corba_apiSK.o metaiv_idlSK.o corba_client.o
	$(CXX) $(LNKOUT) $@ $(CXXFLAGS) $+ $(CORBA_LIB)

ifeq ($(strip $(OSTYPE)),Darwin)
client_example.jar: client_example.class ToolboxAPI.jar
	$(JAR) -J-Dfile.encoding=UTF-8 cvf ./client_example.jar ./client_example.class
else
client_example.jar: client_example.class ToolboxAPI.jar
	$(JAR) cvf ./client_example.jar ./client_example.class
endif

ifeq ($(strip $(OSTYPE)),Darwin)
client_example.class: client_example.java ToolboxAPI.jar
	$(JAVAC) -J-Dfile.encoding=UTF-8 -encoding UTF8 -classpath ./ToolboxAPI.jar client_example.java
else
client_example.class: client_example.java ToolboxAPI.jar
	$(JAVAC) -classpath ./ToolboxAPI.jar client_example.java
endif

clean:
	rm -f *.o client_example-pp client_example-sl
	rm -f metaiv_idl.hh metaiv_idlSK.cc
	rm -f corba_api.hh corba_apiSK.cc
	rm -f client_example.class client_example.jar
	rm -f *.obj *.exe *.cpp

