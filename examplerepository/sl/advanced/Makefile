
TARGETS = advanced.vdm.tar.gz advanced.ps.gz

BASEFILE = advanced

advanced.vdm.tar.gz: GuiDB.vdm MapDB.vdm gui.vdm getdata.tcl getkey.tcl \
	gui_func.cc menu.tcl my_gui.cc showdata.tcl showdef.tcl showinfo.tcl \
	showmsg.tcl demo.sc Makefile.SunOS4 gui_func.h info.tex tcl_init.cc \
	tcl_init.h advanced.tex 

gui.vdm.tex: gui.vdm

MapDB.vdm.tex: MapDB.vdm

GuiDB.vdm.tex: GuiDB.vdm

advanced.ps:  advanced.tex gui.vdm.tex MapDB.vdm.tex GuiDB.vdm.tex

advanced.ps.gz: advanced.ps

include ../Make.rules

