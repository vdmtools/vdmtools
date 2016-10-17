######################################################################
# Interpreter Window. 
#
# $Id: interp.tcl,v 1.4 1996/08/29 13:53:38 erik Exp $
######################################################################


proc interptool_open {} {

  set w .interptool
  
  if {[winfo exist $w]} {
    if {[wm state $w] != "normal"} {
      interptool maybesetwmgeo
    }
    wm deiconify $w
    raise $w
    return
  }
  
  toplevel $w
  
  # pack $w -fill both -expand 1
  
  # wm title $w [ windows`gettitle $w ]
}
  
  
