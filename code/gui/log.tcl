############################################################
# This function pack the log frame
# w - the window to pack in
############################################################
proc log`pack w {
  setSN log`top $w
  help`add $w mainPage`log
  general`packLabel $w log
  set text [tixScrolledText $w.text -scrollbar auto]
  pack $text -fill both -expand 1
  setSN log`text [$text subwidget text]
  set text [$text subwidget text]
  AddFocusWin $text
  log`init
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc log`save {} {
  general`saveText log
}
proc log`load {} {
  general`loadText log
}

proc mainlog`addmsg {msg} {
  log`add $msg
}

proc errorlog`addmsg {msg} {
  log`add $msg
}

proc debuglog`addmsg {msg} {
  commandlog`addmsg $msg
}

proc commandlog`addmsg {mesg} {
  global SN log`text

  log`setstatus normal

  set text $SN(command`text)
  set dodisable 0
  if {[$text cget -state] == "disabled"} {
    $text configure -state normal
    set dodisable 1
  }
  $text insert end $mesg
  $text yview [$text index end]
  update idletasks
  
  log`setstatus disabled
  if {$dodisable} {
    $text configure -state disabled
  }
}
proc log`add mesg {
  global SN log`text

  log`setstatus normal

  set text $SN(log`text)
  set dodisable 0
  if {[$text cget -state] == "disabled"} {
    $text configure -state normal
  }
  $text insert end $mesg
  $text yview [$text index end]
  update idletasks

  log`setstatus disabled
  if {$dodisable} {
    $text configure -state disabled
  }
}

proc log`erradd mesg {
  global SN log`text

  log`setstatus normal

  set text $SN(log`text)
  $text insert end $mesg ErrTag
  $text yview [$text index end]
  update idletasks
  # $text tag configure ErrTag -background red

  log`setstatus disabled
}

############################################################
# These two function print/pipe the content of the
# 'log' buffer
############################################################
proc log`print {} {
  general`printText log
}
proc log`pipe {} {
  general`pipeText log
}

############################################################
# These two function are used to modify read-only status 
############################################################

proc log`setstatus {value} {
  global SN
  $SN(log`text) configure -state $value
}

proc log`init {} {
  log`setstatus disabled
}

############################################################
# This functions clear the log window
############################################################
proc log`clear {} {
  log`setstatus normal
  general`clear log
  log`setstatus disabled
}
