
TARGETS = graph-ed.vdm.gz graph-ed.ps.gz

BASEFILE = graph-ed

# Specification of a graph editor
graph-ed.vdm.gz: graph-ed.vdm

graph-ed.ps.gz: graph-ed.ps
	gzip -c < graph-ed.ps >$@

include ../Make.rules
