BASEDIR=$(TBCODE)
ifndef CORBADIR
  CORBADIR=/usr/local/omniORB
endif
OMNIIDL = $(CORBADIR)/bin/omniidl

IDLCCFLAGS = -bpython
IDLDIR=$(BASEDIR)/api/corba
IDLSRC_FILES=corba_api.idl metaiv_idl.idl internal_api.idl
IDLSRC=$(patsubst %,$(IDLDIR)/%, $(IDLSRC_FILES))
IDL_TARGET=$(IDLSRC_FILES:%.idl=%_idl.py) 

all: $(IDL_TARGET) 

%_idl.py: $(IDLDIR)/%.idl
	$(OMNIIDL)  $(IDLCCFLAGS)  $<

clean:
	rm -f  $(IDL_TARGET) $(IDLSRC_FILES:%.idl=%_idl.pyc) 
	rm -fR ToolboxAPI ToolboxAPI__POA
	rm -fR VDM VDM__POA

