# Makefile for KLV system 
#
# It is supposed to be execute on Windows under the cygwin32. To run
# it on a UNIX platform the //hermes should be removed in the VDMLIB
# and APILIB.

PACKAGEDIR = classes

CLASSES = $(basename $(wildcard *.java))
ALL = $(foreach c, $(CLASSES), $(PACKAGEDIR)/$(c).class)
CLASSESALL = $(foreach c, $(CLASSES), $(c).class)

JARFILE = klvjar

all: $(JARFILE)
classes: $(ALL)

$(PACKAGEDIR)/%.class: %.java
	javac -d $(PACKAGEDIR) -classpath $(PACKAGEDIR) $<

VDMLIB=//hermes/local/share/opt/simu/vdm/VDM.jar
APILIB=//hermes/local/share/opt/simu/vdm/ToolboxAPI.jar

classes/dk: $(VDMLIB) $(APILIB)
	rm -rf classes/dk
	cd classes; jar -xf "$(VDMLIB)"
	cd classes; jar -xf "$(APILIB)"

klvjar: classes/dk $(ALL)
	rm -f ../klv.jar
	cd classes; jar cmf ../klv.mf ../../klv.jar $(CLASSESALL) dk

clean:
	rm -rf classes/dk
	rm -rf classes/META-INF
	rm -f classes/*.class
	rm ../klv.jar
