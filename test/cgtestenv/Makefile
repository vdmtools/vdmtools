TESTARG=$(BASEDIR)/testarg
TESTARGDYN=testarg.dyn
TESTVAL=$(BASEDIR)/testval
COMP=$(BASEDIR)/comp.vdm.sh
COLLECT=$(BASEDIR)/collect
MODE=VDMPP

testspec.dyn.arg:$(TESTARGDYN).incl vdm.ast vdmtest.arg
	-rm -f testspec.dyn.arg 
	$(COLLECT) $(TESTARGDYN).incl testspec.dyn.arg

testspec.arg: $(TESTARG).incl vdm.ast
	-rm -f testspec.arg testval.ast
	$(COLLECT) $(TESTARG).incl testspec.arg -D$(MODE)
	$(COLLECT) $(TESTVAL).incl testval.ast -D$(MODE) 

comp: cres ires
	-rm -f comp.vdm
	$(COMP) comp.vdm

debug.arg: debug  vdm.ast vdmtest.arg
	-rm -f debug.arg
	$(COLLECT) debug debug.arg

debugpp.arg: debugpp  vdm.ast vdmtest.arg
	-rm -f debugpp.arg
	$(COLLECT) debugpp debugpp.arg

priority.vdm:	$(BASEDIR)/priority.vdm.arg priority.arg
	-rm -f priority.vdm
	$(COLLECT) $(BASEDIR)/priority.vdm.arg priority.vdm

clean:
	rm -f testspec.arg m4* .core .vdmtest *~
	rm -f *-??.arg *-??.arg.err *-??.arg.res
	rm -f cres *-??.vdm ires trace 
