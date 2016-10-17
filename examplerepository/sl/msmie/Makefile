# Multiprocessor Shared-Memory Information Exchange

TARGETS = msmie.vdm.tar.gz msmie.ps.gz 

BASEFILE = msmie

msmie.vdm.tar.gz: msmie.tex \
        msmie1.vdm msmie2.vdm msmie0.vdm alt_msmie.vdm

msmie.dvi: msmie.tex msmie0.vdm.tex msmie1.vdm.tex msmie2.vdm.tex alt_msmie.vdm.tex

msmie.ps.gz: msmie.ps

include ../Make.rules
