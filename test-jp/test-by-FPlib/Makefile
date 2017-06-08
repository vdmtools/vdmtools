###########################
# Auto VDM -> TeX report
###########################

## Definitions
#################

HOME = /Users/sahara
VPPHOME = $(HOME)/vdmpp
VPPDE = $(VPPHOME)/bin/vppde

#encode
#CODE =sjis
CODE =utf8

# TeX commands
TEXDIR = /usr/texbin
PLATEX = $(TEXDIR)/platex --kanji=$(C0DE)
#BIBTEX=jbibtex
BIBTEX=pbibtex --kanji=$(C0DE)
DVIPDFM = $(TEXDIR)/dvipdfmx
EBB = ebb

# VDMTools command line
VPPDE = vppde

# Output document name
OUTDOC = FPlib

# VPP files

VPPFILES = AllT.vpp		FHashtable.vpp		FNumberT.vpp		FSet.vpp \
	FBusinessTable.vpp	FHashtableT.vpp		FProduct.vpp		FSetT.vpp \
	FBusinessTableT.vpp	FInteger.vpp		FProductT.vpp		FString.vpp \
	FCalendar.vpp		FIntegerT.vpp		FQueue.vpp		FStringT.vpp \
	FCalendarT.vpp		FJapaneseCalendar.vpp	FQueueT.vpp		FTestDriver.vpp \
	FCharT.vpp		FJapaneseCalendarT.vpp	FReal.vpp		FTestLogger.vpp \
	FCharacter.vpp		FMap.vpp		FRealT.vpp		io.vpp \
	FFunction.vpp		FMapT.vpp		FSequence.vpp \
	FFunctionT.vpp		FNumber.vpp		FSequenceT.vpp


# Test script name
TESTSCRIPT = test.script
# Image directory
IMGDIR = image

## Make
#################

# make target
all: $(OUTDOC)

$(OUTDOC):  makevdmtex
	$(PLATEX) $(OUTDOC)
	$(BIBTEX) $(OUTDOC)
	makeindex  $(OUTDOC).idx
	$(PLATEX) $(OUTDOC)
	$(PLATEX) $(OUTDOC)
	$(DVIPDFM) $(OUTDOC)

# run VDMTools
makevdmtex:
	# Read & Syntax check
	LANG=ja_JP.$(CODE) $(VPPDE) -p -R vdm.tc $(VPPFILES)
	# Type check
	LANG=ja_JP.$(CODE) $(VPPDE) -t $(VPPFILES)
	# Run Interpreter with TESTSCRIPT
	#LANG=ja_JP.$(CODE) $(VPPDE) -i -D -I -P -Q -R vdm.tc $(TESTSCRIPT) $(VPPFILES)
	# Run Interpreter with "set exception" option
	LANG=ja_JP.$(CODE) $(VPPDE) -o setException.script -q 
	# Pretty print
	LANG=ja_JP.$(CODE) $(VPPDE)  -l -r $(VPPFILES)

# Clean files. But Output document is not deleted.
clean:
	rm -rf *.aux *.log *.bbl *.blg *.dvi *.toc
	rm -rf *.out *.vpp.tex *.vdmpp.tex *.idx vdm.tc
	rm -rf image/*.bb

# Clean output document
realclean: clean
	rm -rf $(OUTDOC).pdf