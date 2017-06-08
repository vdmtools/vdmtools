HOME = /Users/sahara
VPPHOME = $(HOME)/vpp
VPPDE = $(VPPHOME)/bin/vppde
#PDFLATEX=altjpdflatex
#PDFLATEX=pdfelatex
#PDFLATEX=pdflatex
PDFLATEX=dotexshop
BIBTEX=jbibtex

SPECFILES = Calendar.vpp Character.vpp CommonDefinition.vpp \
	Date.vpp DoubleListQueue.vpp Function.vpp Hashtable.vpp \
	Integer.vpp JapaneseCalendar.vpp Map.vpp Number.vpp \
	Object.vpp Product.vpp Queue.vpp Real.vpp SBCalendar.vpp \
	Sequence.vpp Set.vpp String.vpp Term.vpp Time.vpp UniqueSequence.vpp \
	AllT.vpp CalendarT.vpp DateT.vpp \
	DoubleListQueueT.vpp FunctionT.vpp \
	HashtableT.vpp IntegerT.vpp MapT.vpp \
	NumberT.vpp ProductT.vpp QueueT.vpp \
	RealT.vpp SBCalendarT.vpp SequenceT.vpp \
	SetT.vpp StringT.vpp TermT.vpp \
	TestCase.vpp TestDriver.vpp TestLogger.vpp \
	TimeT.vpp UniqueSequenceT.vpp \
	$(VPPHOME)/stdlib/io.vpp

%.pdf: %.tex 
	rm -f $*.aux $*.out $*.toc
	$(PDFLATEX) $<
#	$(BIBTEX) $(<:.tex=)
#	$(PDFLATEX) $<
	$(PDFLATEX) $<

all: $(SPECFILES)
	$(VPPDE) -p $(SPECFILES)
	$(VPPDE) -t $(SPECFILES)

test:  $(SPECFILES)
	 $(VPPDE) -i -R vdm.tc all.arg $(SPECFILES)
	 
pp:$(SPECFILES)
	 $(VPPDE) -l -r -N $(SPECFILES)
	 
All-PP.pdf: All-PP.tex	 
Gofo-PP.pdf: Gofo-PP.tex

clean: 
	rm All-PP.pdf 
