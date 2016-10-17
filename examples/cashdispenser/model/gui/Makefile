# Makefile for CashDispenser system 
#
OSTYPE=$(shell uname)
OSTYPE2=$(word 1, $(subst _, ,$(strip $(OSTYPE))))
ifeq ($(strip $(OSTYPE2)),CYGWIN)
#TBHOME?=/cygdrive/c/Program\ Files/The\ VDM++\ Toolbox\ v6.8.7
#TBHOME?=/cygdrive/c/Program\ Files/The\ VDM++\ Toolbox\ v6.8.7-Academic
TBHOME?=../../../..
JDKHOME?=/cygdrive/c/Program\ Files/Java/jdk1.5.0_06
#JDKHOME?=/cygdrive/c/j2sdk1.4.2_10
LN=cp
else
TBHOME?=../../../..
JDKHOME?=/usr
LN=ln -s
endif

JAVAC=$(JDKHOME)/bin/javac -source 1.4
JAR=$(JDKHOME)/bin/jar

APILIB=ToolboxAPI.jar

#CP=.:..:$(PACKAGEDIR)
JFLAGS= -d $(PACKAGEDIR) -sourcepath . -classpath $(PACKAGEDIR)
DOCDIR=doc
PACKAGEDIR = classes
PPJAVAFILES = Stmt.java ToolboxInterface.java Till.java CashDispenser.java 
PPCLASSES = Stmt$$1.class \
            Stmt$$Transaction.class ToolboxInterface$$VDMTill.class \
            Till$$1.class CashDispenser$$1.class \
            $(foreach j, $(PPJAVAFILES), $(basename $(j)).class)
PPPACKAGE = $(foreach c, $(PPCLASSES), $(PACKAGEDIR)/$(c))

SLJAVAFILES = Stmt.java ToolboxInterface.java Till.java CashDispenser_SL.java
SLCLASSES = Stmt$$1.class \
            Stmt$$Transaction.class ToolboxInterface$$VDMTill.class \
            Till$$1.class CashDispenser_SL$$1.class \
	    $(foreach j, $(SLJAVAFILES), $(basename $(j)).class)
SLPACKAGE = $(foreach c, $(SLCLASSES), $(PACKAGEDIR)/$(c))

ALLJAVAFILES = Stmt.java ToolboxInterface.java Till.java \
           CashDispenser.java CashDispenser_SL.java 

HTMLFILES = $(foreach j, $(ALLJAVAFILES), $(basename $(j)).html)

all: ../cashdispenser.jar ../cashdispenser_SL.jar

%.html:	%.java
	javadoc -notree -noindex -nohelp -d $(DOCDIR) $<

docdir:
	rm -rf $(DOCDIR)
	mkdir $(DOCDIR)

doc:	docdir $(HTMLFILES)

$(PACKAGEDIR)/CashDispenser$$1.class \
$(PACKAGEDIR)/CashDispenser.class: CashDispenser.java classes/jp
	$(JAVAC) $(JFLAGS) $<
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) CashDispenser\$$1.class

$(PACKAGEDIR)/CashDispenser_SL$$1.class \
$(PACKAGEDIR)/CashDispenser_SL.class: CashDispenser_SL.java classes/jp
	$(JAVAC) $(JFLAGS) $<
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) CashDispenser_SL\$$1.class

$(PACKAGEDIR)/Till$$1.class \
$(PACKAGEDIR)/Till.class: Till.java classes/jp
	$(JAVAC) $(JFLAGS) $<
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) Till\$$1.class
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) Till\$$1.class

$(PACKAGEDIR)/Stmt$$1.class \
$(PACKAGEDIR)/Stmt.class: Stmt.java classes/jp
	$(JAVAC) $(JFLAGS) $<
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) Stmt\$$Transaction.class
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) Stmt\$$1.class
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) Stmt\$$Transaction.class
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) Stmt\$$1.class

$(PACKAGEDIR)/ToolboxInterface$$VDMTill.class \
$(PACKAGEDIR)/ToolboxInterface.class: ToolboxInterface.java classes/jp
	$(JAVAC) $(JFLAGS) $<
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) ToolboxInterface\$$VDMTill.class
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) ToolboxInterface\$$VDMTill.class

$(PACKAGEDIR)/%.class: %.java classes/jp
	$(JAVAC) $(JFLAGS) $<
	$(JAR) uvf ../cashdispenser.jar -C $(PACKAGEDIR) $(@F)
	$(JAR) uvf ../cashdispenser_SL.jar -C $(PACKAGEDIR) $(@F)

classes:
	mkdir classes

classes/jp: classes $(APILIB)
	rm -rf classes/jp
	cd classes; $(JAR) -xf ../$(APILIB); cd ..
	rm -f ../cashdispenser.jar
	rm -f ../cashdispenser_SL.jar
	cd classes; $(JAR) cvmf ../cashdispenser.mf ../../cashdispenser.jar jp; cd ..
	cd classes; $(JAR) cvmf ../cashdispenser_SL.mf ../../cashdispenser_SL.jar jp; cd ..

slclasses/jp: classes
	rm -rf classesjp/
	cd classes; $(JAR) -xf "$(APILIB)"; cd ..
	rm -f ../cashdispenser_SL.jar
	cd classes; $(JAR) cvmf ../cashdispenser_SL.mf ../../cashdispenser_SL.jar jp; cd ..

../cashdispenser.jar: classes/jp $(PPPACKAGE)

../cashdispenser_SL.jar: classes/jp $(SLPACKAGE)

$(APILIB): $(TBHOME)/api/corba/$(APILIB)
	$(LN) $(TBHOME)/api/corba/$@ .

clean:
	rm -rf classes/jp
	rm -rf classes/META-INF
	rm -f classes/*.class
	rm -f ../cashdispenser.jar
	rm -f ../cashdispenser_SL.jar
	rm -f $(APILIB)
	rm -rf classes
