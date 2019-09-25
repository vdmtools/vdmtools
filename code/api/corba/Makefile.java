ifndef JDKHOME
ifeq ($(strip $(OSTYPE)),win32)
JDKHOME = /usr/java/default
else
ifeq ($(strip $(OSTYPE)),Darwin)
JDKHOME = /usr
else
JDKHOME = /usr/java/current
endif
endif 
endif

ifeq ($(strip $(OSTYPE)),win32)
PLATFORMFLAGS = -i $(VPATH)
else
PLATFORMFLAGS=
endif

IDLJFLAGS = -pkgPrefix ToolboxAPI jp.vdmtools.api.corba -pkgPrefix VDM jp.vdmtools.api.corba -td $(DESTPATH)javaapi $(PLATFORMFLAGS)

ifeq ($(strip $(OSTYPE)),Darwin)
JAVAC     = $(JDKHOME)/bin/javac -J-Dfile.encoding=UTF-8
JAR       = $(JDKHOME)/bin/jar -J-Dfile.encoding=UTF-8
JAVADOC   = $(JDKHOME)/bin/javadoc -locale en_US -J-Dfile.encoding=UTF-8
IDLJ      = LANG=C $(JDKHOME)/bin/idlj -J-Dfile.encoding=UTF-8
else
JAVAC     = $(JDKHOME)/bin/javac
JAR       = $(JDKHOME)/bin/jar
JAVADOC   = $(JDKHOME)/bin/javadoc -locale en_US
IDLJ      = LANG=C $(JDKHOME)/bin/idlj
endif

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
SOURCEPATH="$(shell cygpath -w baselink)"/api/corba/
DESTPATH="$(shell cygpath -w $(shell pwd))"/
endif

#JAVA_VERSION=$(shell java -version 2>&1 | head -1 | awk -F '.' '{print $$2}' )
JAVA_VERSION_MAJOR=$(shell java -version 2>&1 | head -1 | awk -F '["|.]' '{print $$2}')
JAVA_VERSION_MINOR=$(shell java -version 2>&1 | head -1 | awk -F '["|.]' '{print $$3}')
ifeq ($(JAVA_VERSION_MAJOR),1)
JAVA_VERSION=$(JAVA_VERSION_MINOR)
else
JAVA_VERSION=$(JAVA_VERSION_MAJOR)
endif

ifeq ($(strip $(OSTYPE)),win32)
JAVACFLAGS = -source $(JAVA_VERSION) -classpath ".;javaapi"
TOOLBOX_CLIENT = $(VPATH)/ToolboxClient.java
else
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
JAVACFLAGS = -source $(JAVA_VERSION) -classpath ".;javaapi"
TOOLBOX_CLIENT = $(VPATH)/ToolboxClient.java
else
ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.5)
#JAVACFLAGS = -source 5 -target 5 -classpath .:./javaapi
JAVACFLAGS = -classpath .:./javaapi
TOOLBOX_CLIENT = ToolboxClient.java
else
JAVACFLAGS = -source $(JAVA_VERSION) -classpath .:./javaapi
TOOLBOX_CLIENT = ToolboxClient.java
endif
else
JAVACFLAGS = -source $(JAVA_VERSION) -classpath .:./javaapi
TOOLBOX_CLIENT = ToolboxClient.java
endif
endif
endif


PACKAGEDIR=jp/vdmtools/api/corba
DOCDIR=javaapi-doc
APIDIR=javaapi

all:  ToolboxAPI.jar

clean:
	rm -fr javaapi
	rm -f ToolboxAPI.jar
	rm -f metaiv_idl.stamp corba_api.stamp toolboxclient.stamp

$(DOCDIR): FORCE 
	[ -d $@ ] || mkdir -p $@

$(APIDIR): FORCE 
	[ -d $@ ] || mkdir -p $@

FORCE:

ToolboxAPIDoc: javaapi/$(PACKAGEDIR) $(DOCDIR) ToolboxAPI.jar
	$(JAVADOC) -version -d $(DOCDIR) \
	$</ToolboxAPI/*.java \
	$</ToolboxAPI/VDMCodeGeneratorPackage/*.java \
	$</VDM/*.java \
	$(SOURCEPATH)$(TOOLBOX_CLIENT)

%.stamp: %.idl
ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
	cd $(SOURCEPATH);$(IDLJ) $(IDLJFLAGS) $<
else
	$(IDLJ) $(IDLJFLAGS) $<
endif
	touch $@

toolboxclient.stamp: ToolboxClient.java $(APIDIR)
	$(JAVAC) $(JAVACFLAGS) -d javaapi $(SOURCEPATH)$<
	touch $@

ToolboxAPI.jar: metaiv_idl.stamp corba_api.stamp toolboxclient.stamp
	$(JAVAC) $(JAVACFLAGS) javaapi/$(PACKAGEDIR)/ToolboxAPI/*.java \
	                       javaapi/$(PACKAGEDIR)/ToolboxAPI/VDMCodeGeneratorPackage/*.java \
	                       javaapi/$(PACKAGEDIR)/VDM/*.java
	cp ToolboxClient.java javaapi/jp/vdmtools/api/.
	cd javaapi; $(JAR) cvf ../ToolboxAPI.jar jp


