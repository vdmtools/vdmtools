HOME = /Users/sahara
VPPHOME = $(HOME)/bin.Darwin.vdmpp-6.8
VPPDE = $(VPPHOME)/bin/vppde
SPECFILES = Calendar.vpp Character.vpp CommonDefinition.vpp \
	Date.vpp DoubleListQueue.vpp Function.vpp Hashtable.vpp \
	Integer.vpp JapaneseCalendar.vpp Map.vpp Number.vpp \
	Object.vpp Product.vpp Queue.vpp Real.vpp SBCalendar.vpp \
	Sequence.vpp Set.vpp String.vpp Term.vpp Time.vpp UniqueSequence.vpp \
	test/AllT.vpp test/CalendarT.vpp test/DateT.vpp \
	test/DoubleListQueueT.vpp test/FunctionT.vpp \
	test/HashtableT.vpp test/IntegerT.vpp test/MapT.vpp \
	test/NumberT.vpp test/ProductT.vpp test/QueueT.vpp \
	test/RealT.vpp test/SBCalendarT.vpp test/SequenceT.vpp \
	test/SetT.vpp test/StringT.vpp test/TermT.vpp \
	test/TestCase.vpp test/TestDriver.vpp test/TestLogger.vpp \
	test/TimeT.vpp test/UniqueSequenceT.vpp \
	$(VPPHOME)/stdlib/io.vpp

all: $(SPECFILES)
	$(VPPDE) -p $(SPECFILES)
	$(VPPDE) -t $(SPECFILES)

test:  $(SPECFILES)
	 $(VPPDE) -i -R vdm.tc all.arg $(SPECFILES)
pp:$(SPECFILES)
	 $(VPPDE) -l -r -N $(SPECFILES)
