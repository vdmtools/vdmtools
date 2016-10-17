BASEDIR=../../../..

PACKAGEDIR=jp/vdmtools/api
PACKAGE   =jp.vdmtools.api
DOCDIR=apidoc

CLASSESDIR=$(BASEDIR)/classes
SUBDIR=$(CLASSESDIR)/$(PACKAGEDIR)

CLASSES=$(basename $(wildcard *.java))
ALL = $(foreach c, $(CLASSES), $(SUBDIR)/$(c).class)

JARNAME = ToolboxAPI.jar
JARFILE = $(CLASSESDIR)/$(JARNAME)

JAVAC=javac -classpath $(BASEDIR) -deprecation -d $(CLASSESDIR) 

all: $(CLASSESDIR) $(ALL)

jar: $(JARFILE)

$(JARFILE): all
	cd $(CLASSESDIR) ; jar cvf $(JARNAME) $(PACKAGEDIR)

$(SUBDIR)/%.class: %.java
	echo $@
	$(JAVAC) $<

ToolboxClient.java: corba

corba: idl/api.idl
	-rm corba
	idltojava -fno-cpp -fno-server -p $(PACKAGE).corba -j $(BASEDIR) idl/api.idl
	make -C corba -f ../Makefile BASEDIR=$(BASEDIR)/.. PACKAGEDIR=$(PACKAGEDIR)/corba
	make -C corba/VDM -f ../../Makefile BASEDIR=$(BASEDIR)/../.. PACKAGEDIR=$(PACKAGEDIR)/corba/VDM
$(CLASSESDIR):
	mkdir $(CLASSESDIR)

api.idl: 
	echo run 'make preprocess' under Unix

preprocess: idl/corba_api.idl idl/metaiv_idl.idl
	cd idl ; ln -s corba_api.idl corba_api.c
	cd idl ; gcc -E -P corba_api.c > api.idl
	cd idl ; rm corba_api.c

run: $(JARFILE)
	java -classpath $(JARFILE) $(PACKAGE).ToolboxClient

javadoc: $(ALL) $(DOCDIR)
	javadoc -classpath ../../../.. -version -d $(DOCDIR) $(PACKAGE) $(PACKAGE).corba $(PACKAGE).corba.VDM

$(DOCDIR):
	mkdir $(DOCDIR)

clean:
	rm -rf $(CLASSESDIR)/$(PACKAGEDIR) $(JARFILE) $(DOCDIR) corba idl/api.idl
