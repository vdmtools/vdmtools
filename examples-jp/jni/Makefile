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


all: jnienv.so Hello.jar

ifeq ($(strip $(OSTYPE)),Darwin)
MAJOR_DARWIN_VERSION=$(shell uname -r | awk -F '.' '{print $$1}')
CCPATH = /usr/bin/

ifeq ($(strip $(MAJER_DARWIN_VERSION)),8) # 10.4
CC      = $(CCPATH)g++-4.0
CXX     = $(CCPATH)g++-4.0
CFLAGS	= -g -I$(INCDIR) -I/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers
LIBFLAGS = -L. -L$(LIBDIR) -lvdm_darwin -liconv -framework JavaVM -Wl,-syslibroot,$(SDK)
else
ifeq ($(strip $(MAJER_DARWIN_VERSION)),9) # 10.5
CC      = $(CCPATH)g++-4.2
CXX     = $(CCPATH)g++-4.2
CFLAGS	= -g -I$(INCDIR) -I/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers
LIBFLAGS = -L. -L$(LIBDIR) -lvdm_darwin -liconv -framework JavaVM -Wl,-syslibroot,$(SDK)
else
ifeq ($(strip $(MAJER_DARWIN_VERSION)),10) # 10.6
CC      = $(CCPATH)g++
CXX     = $(CCPATH)g++
CFLAGS	= -g -I$(INCDIR) -I/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers
LIBFLAGS = -L. -L$(LIBDIR) -lvdm_darwin -liconv -framework JavaVM -Wl,-syslibroot,$(SDK)
else #10.7~
JDK_HOME = $(shell $(shell readlink $(shell which javac) | xargs dirname)/java_home)
CFLAGS	= -g -I$(INCDIR) -I$(JDK_HOME)/include -I$(JDK_HOME)/include/darwin
JNI_LIB_DIR=$(JDK_HOME)/jre/lib/server
CC      = $(CCPATH)clang++
CXX     = $(CCPATH)clang++
CFLAGS	+= -Wno-return-type-c-linkage
LIBFLAGS = -L. -L$(LIBDIR) -lvdm_darwin -liconv -Wl,-syslibroot,$(SDK) -L$(JNI_LIB_DIR) -ljvm
endif #10.6
endif #10.5
endif #10.4

CCFLAGS	= $(CFLAGS)
CXXFLAGS= $(CCFLAGS)

jnienv.so: jnienv.cc export
	${CXX} $(CXXFLAGS) $(ARCH) -dynamiclib -fPIC -o $@ $< $(LIBFLAGS)

JAVACCFLAGS = -J-Dfile.encoding=UTF-8 -encoding UTF8
JARFLAGS = -J-Dfile.encoding=UTF-8

export:
	echo "#!/bin/bash" > exp.sh
	echo "export DYLD_LIBRARY_PATH=$(JNI_LIB_DIR):$$DYLD_LIBRARY_PATH" >> exp.sh
	chmod 755 exp.sh
endif

ifeq ($(strip $(OSTYPE)),Linux)
CCPATH = /usr/bin/
CC      = $(CCPATH)gcc
CCC     = $(CCPATH)g++

CPUTYPE = $(shell uname -m)
ifeq ($(strip $(CPUTYPE)),x86_64)
JVMDIR=amd64/server
else
JVMDIR=i386/client
endif

# for Ubuntu 10.04 - 12.04, Debian 6
#JDK_HOME = /usr/lib/jvm/java-1.6.0-openjdk
# for Fedora 17
#JDK_HOME = /usr/lib/jvm/java-1.7.0-openjdk.x86_64

JDK_HOME = $(shell readlink -f $(shell which javac) | xargs dirname | xargs dirname)
JNI_INC_DIR = $(JDK_HOME)/include

JNI_LIB_DIR = $(JDK_HOME)/jre/lib/$(JVMDIR)

CFLAGS	= -g -I$(INCDIR) -I$(JNI_INC_DIR) -I$(JNI_INC_DIR)/linux
CCFLAGS	= $(CFLAGS)
CXXFLAGS= $(CCFLAGS)

LIBFLAGS = -L. -L$(LIBDIR) -L$(JNI_LIB_DIR) -lvdm_shared -ljvm

jnienv.so: jnienv.cc
	${CXX} $(CXXFLAGS) -shared -fPIC -o $@ $< $(LIBFLAGS)
	make export

export:
	echo "#!/bin/bash" > exp.sh
	echo "export LD_LIBRARY_PATH=$(JNI_LIB_DIR):$(LD_LIBRARY_PATH)" >> exp.sh
	chmod 755 exp.sh

JAVACCFLAGS=
endif

ifeq ($(strip $(OSTYPE)),CYGWIN)
#TBDIRWIN=$(shell cygpath -w /cygdrive/c/cygwin$(VDMTBDIR))
TBDIRWIN="C:/Program Files/The VDM++ Toolbox v9.0.3"
WININCDIR = $(TBDIRWIN)/cg/include
#WININCDIR = .
WINLIBDIR = $(TBDIRWIN)/cg/lib
#WINLIBDIR = .
VDMLIB = $(WINLIBDIR)/vdm.lib

JAVA = $(shell java -version >& javav_tmp;cat javav_tmp | head -1)
JAVAV = $(word 3, $(strip $(JAVA)))

#JDK_HOME ?= "C:/Program Files/Java/jdk1.7.0_21"
JDK_HOME ?= "C:/Program Files/Java/jdk"$(JAVAV)
JNI_INC_DIR = $(JDK_HOME)/include
JNI_LIB_DIR = $(JDK_HOME)/lib
DLL_CC = cl.exe
DLL_LINK = link.exe /DLL /nologo
DLLFLAGS = /D "_USRDLL"
DLL_LFLAGS   = /incremental:no /NODEFAULTLIB /MANIFEST
DLL_CFLAGS = /nologo /c /Zi /MD /W0 /EHsc /TP -D_UNICODE /I$(WININCDIR) /I$(JNI_INC_DIR) /I$(JNI_INC_DIR)/win32
DLLWINLIBS = msvcrt.lib msvcprt.lib \
             kernel32.lib user32.lib advapi32.lib delayimp.lib oldnames.lib \
	     $(JNI_LIB_DIR)/jvm.lib

MT=mt.exe
jnienv.obj: %.obj: %.cc
	$(DLL_CC) $(DLL_CFLAGS) $(DLLFLAGS) $(DLL_INCPATH) /Fo"$@" $<

jnienv.so: %.so: %.obj
	#$(DLL_LINK) $(DLL_LFLAGS) /out:"$@" $(DLL_LPATH) vdm.lib $(DLLWINLIBS) $<
	$(DLL_LINK) $(DLL_LFLAGS) /out:"$@" $(DLL_LPATH) $(VDMLIB) $(DLLWINLIBS) $<
	$(MT) -manifest $@.manifest -outputresource:$@

JAVACCFLAGS = -J-Dfile.encoding=UTF-8 -encoding UTF8
endif

%.class: %.java
	javac $(JAVACCFLAGS) -d . -classpath . $^

JAVASRC = SomeException.java Hello.java
JAVACLS = $(patsubst %.java, %.class, $(JAVASRC))

Hello.jar: $(JAVASRC)
	echo $(JAVACLS)
	make $(JAVACLS)
	jar $(JARFLAGS) cvf ./Hello.jar ./jni

clean:
	rm -f *.o *.obj *.idb *.pdb *.class *.jar javav_tmp
	rm -rf jni *.dSYM
	rm -f jnienv.so jnienv.lib jnienv.exp jnienv.so.manifest

