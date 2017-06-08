
SPECS = ast.vpp basetools.vpp batcherr.vpp cgrepos.vpp \
        colleague.vpp errors.vpp errstate.vpp filestatus.vpp \
        interface.vpp mediator.vpp nonprinterr.vpp options.vpp \
        prompterr.vpp ptypes.vpp repository.vpp scripterr.vpp \
        statestore.vpp toolkit.vpp unit.vpp unitstatus.vpp \
	updateses.vpp noneses.vpp structses.vpp flatses.vpp \
	updateproj.vpp statusinfo.vpp repdb.vpp \
        depend.vpp umltool.vpp

PPALL := $(SPECS)

.PHONY: default
default: 
	echo Use make PP dot
	exit 1;

all: PP

PP:  .vdmde-specman-pp

dot: .vdmde-specman-pp

# ETN: use /bin/pwd below to get real path! The sed cmd may fail if spec
# is not in ones homedir!
USER=`whoami`
MPWD=$(shell /bin/pwd)
UNAME = $(shell uname)
REALOS = $(word 1, $(subst _, ,$(strip $(UNAME))))
ifeq ($(strip $(REALOS)),CYGWIN)
MPWD=$(shell /bin/pwd | cygpath -w -m -f -)
else
MPWD=$(shell /bin/pwd)
endif
READ_CMD = perl -le 'for (@ARGV) { print "r ${MPWD}/$$_"; }'

.vdmde-specman-pp: $(PPALL)
	$(READ_CMD) $^ > $@

linkall link: 
	echo $@ is no longer used. files should be found by make through its vpath directive
	exit 1;

