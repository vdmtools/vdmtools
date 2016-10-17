
TARGETS = loose.vdm.tar.gz loose.ps.gz 

BASEFILE = loose

# Looseness Analysis Tool for a VDM-SL Subset.
loose.vdm.tar.gz: loose.tex \
        as.vdm expr.vdm pat.vdm env.vdm auxil.vdm

loose.dvi: loose.tex as.vdm expr.vdm.tex pat.vdm.tex env.vdm.tex auxil.vdm.tex

loose.ps.gz: loose.ps

include ../Make.rules
