# Makefile for CashDispenser model - concurrency version
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

VDMLIB=VDM.jar
APILIB=javaapi.jar

PACKAGEDIR = classes
JAVAFILES = Account.java Card.java Cardholder.java CentralResource.java \
            Channel.java Clock.java Letter.java Letterbox.java \
            LocalResource.java LocalTill.java MessageBuffer.java \
            Till.java Timer.java TillGUI.java CashDispenser.java \
            ToolboxInterface.java Stmt.java
CLASSES = $(basename $(JAVAFILES))
ALL = $(foreach c, $(CLASSES), $(PACKAGEDIR)/$(c).class)
CLASSESALL = $(foreach c, $(CLASSES), $(c).class)

QUOTEDIR = $(PACKAGEDIR)
QUOTEFILES = quotes/Fail.java quotes/PostStmt.java quotes/GetBalance.java \
             quotes/ResetTries.java quotes/IncTries.java quotes/Retained.java \
             quotes/IsLegalCard.java quotes/TriesExceeded.java \
             quotes/PinNotOk.java quotes/Withdrawal.java quotes/PinOk.java
QUOTECLASSES = $(basename $(QUOTEFILES))
QUOTEALL = $(foreach c, $(QUOTECLASSES), $(QUOTEDIR)/$(c).class)

JARFILE = cashdispenser_concur

all: $(JARFILE)
classes: $(ALL)

$(PACKAGEDIR)/%.class: %.java
	$(JAVAC) -d $(PACKAGEDIR) -classpath .:..:$(PACKAGEDIR) $<

$(QUOTEDIR)/%.class: %.java
	$(JAVAC) -d $(QUOTEDIR) -sourcepath . -classpath $(PACKAGEDIR) $<

%.class: %.java
	$(JAVAC) -g $<

classes/jp: $(VDMLIB) $(APILIB)
	rm -rf classes/jp
	cd classes; $(JAR) -xf ../$(VDMLIB)
	cd classes; $(JAR) -xf ../$(APILIB)

cashdispenser_concur: classes/jp $(ALL) $(QUOTEALL)
	cd classes; \
        $(JAR) cmf ../cashdispenser_concur.mf \
                  ../../cashdispenser_concur.jar *.class jp quotes

$(VDMLIB): $(TBHOME)/javacg/$(VDMLIB)
	$(LN) $(TBHOME)/javacg/$@ .

$(APILIB): $(TBHOME)/javacg/$(APILIB)
	$(LN) $(TBHOME)/javacg/$@ .

clean:
	rm -rf classes/jp
	rm -rf classes/META-INF
	rm -f classes/*.class
	rm -rf $(VDMLIB) $(APILIB)

