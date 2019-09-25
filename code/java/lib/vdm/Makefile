# $Id: Makefile,v 1.27 2006/01/05 07:53:56 vdmtools Exp $
# Makefile for Java VDM library

########################### Variable definitions ########################

# Binary locations
ifndef JAVACC
JAVACC  = /usr/local/javacc-4.2/bin/javacc
endif

ifndef JDKHOME
ifeq ($(strip $(OSTYPE)),win32)
JDKHOME=/usr/java/default
else
JDKHOME=/usr/java/current
endif
endif

#JAVA_VERSION=$(shell java -version 2>&1 | head -1 | awk -F '.' '{print $$2}' )
JAVA_VERSION_MAJOR=$(shell java -version 2>&1 | head -1 | awk -F '["|.]' '{print $$2}')
JAVA_VERSION_MINOR=$(shell java -version 2>&1 | head -1 | awk -F '["|.]' '{print $$3}')
ifeq ($(JAVA_VERSION_MAJOR),1)
JAVA_VERSION=$(JAVA_VERSION_MINOR)
else
JAVA_VERSION=$(JAVA_VERSION_MAJOR)
endif

ifeq ($(strip $(OSTYPE)),Darwin)
ifeq ($(strip $(MACOSX_DEPLOYMENT_TARGET)),10.5)
JAVAC   = $(JDKHOME)/bin/javac -J-Dfile.encoding=UTF-8 -source 5
else
JAVAC   = $(JDKHOME)/bin/javac -J-Dfile.encoding=UTF-8 -source $(JAVA_VERSION)
endif
JAR     = $(JDKHOME)/bin/jar -J-Dfile.encoding=UTF-8
JAVADOC = $(JDKHOME)/bin/javadoc -locale en_US -J-Dfile.encoding=UTF-8
else
JAVAC   = $(JDKHOME)/bin/javac -source $(JAVA_VERSION)
JAR     = $(JDKHOME)/bin/jar
JAVADOC = $(JDKHOME)/bin/javadoc -locale en_US
endif

# Normal library files
JAVAFILES = CGException.java VDMRunTimeException.java  VDMSpecException.java\
            INDEX_OUT_OF_RANGE.java NotSupportedConstructException.java \
	    RANDOM.java \
            Record.java Tuple.java  UTIL.java Token.java \
            EvaluatePP.java VDMThread.java PeriodicThread.java Sentinel.java \
	    CollectionFactory.java DefaultCollection.java

PACKAGEDIR = jp/vdmtools/VDM

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)
SOURCEPATH1="$(shell cygpath -w baselink)"/java/lib/vdm/
SOURCEPATH2="$(shell cygpath -w baselink)"/java/lib/vdmconc/
SOURCEPATH3="$(shell cygpath -w baselink)"/random/
FILES1=CGException.java VDMRunTimeException.java VDMSpecException.java \
	INDEX_OUT_OF_RANGE.java NotSupportedConstructException.java \
	Record.java Tuple.java  UTIL.java Token.java
FILES2=EvaluatePP.java VDMThread.java PeriodicThread.java Sentinel.java
FILES3=RANDOM.java
endif

# Quote values needed for IO library
QUOTEDIR = $(PACKAGEDIR)/quotes
QUOTEJAVA = start.java append.java
QUOTECLASSES = $(basename $(QUOTEJAVA))


DOCDIR=libdoc
CLASSES = $(basename $(JAVAFILES))
ALL = $(foreach c, $(CLASSES), $(PACKAGEDIR)/$(c).class) \
      $(foreach c, $(QUOTECLASSES), $(QUOTEDIR)/$(c).class) \
      $(PACKAGEDIR)/ValueParser/ValueParser.class
JARFILE = VDM.jar

# Defs for value parser
PARSER_DIR = java_valueparser
VPSTAMP=$(PARSER_DIR)/valueparser.stamp

########################### Rules ########################

$(JARFILE): $(VPSTAMP) $(ALL)
	cp $(JAVAFILES) $(PACKAGEDIR)/.
	cp $(PARSER_DIR)/*.java $(PACKAGEDIR)/ValueParser/.
	$(JAR) cvf $@ $(PACKAGEDIR)

$(ALL):	$(PACKAGEDIR) $(QUOTEDIR)


$(PACKAGEDIR): FORCE
	[ -d $@ ]   || mkdir -p $@

$(QUOTEDIR): FORCE
	[ -d $@ ]   || mkdir -p $@

$(DOCDIR): FORCE
	[ -d $@ ]   || mkdir -p $@

FORCE:

ifeq ($(strip $(OSTYPE)),CYGWIN_NT)

javadoc: $(FILES1) $(FILES2) $(FILES3)
	$(JAVADOC) -version -d $(DOCDIR) \
	$(FILES1:%.java=$(SOURCEPATH1)%.java) \
	$(FILES2:%.java=$(SOURCEPATH2)%.java) \
	$(FILES3:%.java=$(SOURCEPATH3)%.java)
else
javadoc: $(JAVAFILES) 
	$(JAVADOC) -version -d $(DOCDIR) $^
endif

$(PACKAGEDIR)/%.class: %.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH1)$<

$(PACKAGEDIR)/EvaluatePP.class: EvaluatePP.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH2)$<
$(PACKAGEDIR)/PeriodicThread.class: PeriodicThread.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH2)$<
$(PACKAGEDIR)/Sentinel.class: Sentinel.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH2)$<
$(PACKAGEDIR)/VDMThread.class: VDMThread.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH2)$<
$(PACKAGEDIR)/RANDOM.class: RANDOM.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH3)$<

$(QUOTEDIR)/%.class: %.java
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH1)$<

$(VPSTAMP): ValueParser.jj
	rm -rf $(PARSER_DIR)/*.java*
	$(JAVACC) -OUTPUT_DIRECTORY=$(PARSER_DIR) $(SOURCEPATH1)$<
	touch $(VPSTAMP)

$(PACKAGEDIR)/ValueParser/ValueParser.class: $(PARSER_DIR) $(VPSTAMP)
	$(JAVAC) -classpath . -d . $</*.java

ifeq ($(strip $(OSTYPE)),win32)
$(PACKAGEDIR)/Tuple.class: Tuple.java 
	$(JAVAC) -O -d . -sourcepath $(dir $<) -classpath $(dir $<) $<
$(PACKAGEDIR)/UTIL.class: UTIL.java 
	$(JAVAC) -O -d . -sourcepath $(dir $<) -classpath $(dir $<) $<
else
$(PACKAGEDIR)/Tuple.class\
$(PACKAGEDIR)/UTIL.class: UTIL.java Tuple.java 
	$(JAVAC) -O -classpath . -d . $(SOURCEPATH1)UTIL.java $(SOURCEPATH1)Tuple.java 
endif

javadist: $(JARFILE) $(DOCDIR) javadoc

cleanjavadist:
	rm -rf $(PACKAGEDIR)
	rm -rf $(JARFILE)
	rm -rf $(DOCDIR)
	rm -rf $(PARSER_DIR)



