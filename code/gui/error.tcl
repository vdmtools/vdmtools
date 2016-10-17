
class ErrorLister {

  inherit FrameWin 

  variable Name
  variable WPath
  variable textw
  variable text
  variable Top
  
  constructor {n title} { FrameWin::constructor $n $title; } {
    set Name $n;
    proc ::$n`pack {w} " $this Pack \$w "
  }

  method Pack {w} {}
  method save {} {}
  method load {} {}
  method setstatus {value} {
    $text configure -state $value
  }
  method first {} {}
  method next {} {}
  method prev {} {}
  method last {} {}
  method cb`first {} {}
  method cb`next {} {}
  method cb`prev {} {}
  method cb`last {} {}
  method savepos {i fname line col} {}
  method getfile {i} {}
  method getline {i} {}
  method getcolumn {i} {}
  method geterrors {i} {}
  method Format {i} {}
  method errtag {i} {}
  method goto {file line col} {}
  method showMsg {num} {}
  method select {t} {}
  method addinfo {msg} {}
  method AddErr {errno fname line col msg} {}
  method clearAll {} {}
  method allDone {} {}
  method print {} {}
  method pipe {} {}
}

proc errortool_open {} {
  set w .errortool

  if {[winfo exist $w]} {
    if {[wm state $w] != "normal"} {
      errortool maybesetwmgeo
    }
    wm deiconify $w
    raise $w
    return
  }
  
  toplevel $w
  # pack $w -fill both -expand 1
  
  wm title $w [ windows`gettitle $w ]
}

############################################################
# This function pack the error frame
# w - the window to pack in
############################################################
body ErrorLister::Pack {w} {
  global SN BITMAPDIR
  set Top $w
  
  help`add $w errorPage`error_lister
  general`packLabel $w error_lister
  
  set left [frame $w.left]
  set right [frame $w.right]
  pack $left -side left -anchor n
  pack $right -fill both -expand 1 -side left
  #pack $left -side top -anchor n
  #pack $right -fill both -expand 1 -side left

  ### the command buttons.
  foreach elm {
    {line1 {first last} }
    {line2 {prev next} }
  } {
    set line [lindex $elm 0]
    set buts [lindex $elm 1]
    pack [frame $left.$line]
    foreach but $buts {
      button $left.$line.$but -bitmap bm`$but -command "$this $but"
      pack $left.$line.$but -side left
      help`add $left.$line.$but errorPage`error`$but
    }
  }
  tixScrolledText $right.text -scrollbar auto
  pack $right.text -fill both -expand 1
  set text [$right.text subwidget text]

  setSN $Name`text $text
  
  AddFocusWin $text
  
  bind $text <Down> " $this next; break; " 
  bind $text <Up> " $this prev; break; " 
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
body ErrorLister::save {} {
  general`saveText $text
}
body ErrorLister::load {} {
  general`loadText $text
}

############################################################
# Navigating in the Error window. 
############################################################


body ErrorLister::first {} {
  tkif_SetEditorName error_lister
  tkif_executeCommand "first" {}
}

body ErrorLister::next {} { 
  tkif_SetEditorName error_lister
  tkif_executeCommand "next" {}
}

body ErrorLister::prev {} { 
  tkif_SetEditorName error_lister
  tkif_executeCommand "previous" {}
}

body ErrorLister::last {} { 
  tkif_SetEditorName error_lister
  tkif_executeCommand "last" {}
}

body ErrorLister::cb`first {} { 
  #puts "cb`first"
}
body ErrorLister::cb`next {} { 
  #puts "cb`next"
}
body ErrorLister::cb`prev {} { 
  #puts "cb`prev"
}
body ErrorLister::cb`last {} { 
  #puts "cb`last" 
}



body ErrorLister::errtag {i} {
  set tag "error$i" 
  return $tag
}

body ErrorLister::goto {file line col} {
  # puts "goto $file $line $col"
  error_editor goto $file $line $col 
  emacs`goto $file $line $col
}

body ErrorLister::showMsg {num} {
  select $num
}

body ErrorLister::select {errorno} {
  if {[llength [ $text tag ranges Chosen ] ] >0} {
    $text tag configure Chosen \
        -underline false \
        -background [ $text cget -background ]
    $text tag delete Chosen
  }
  set t [ errtag $errorno ]
  set t1 "$t.first linestart"
  set t2 "$t.first lineend"
  $text tag add Chosen $t1 $t2
  if {[ Config isMonochome ]} {
    $text tag configure Chosen -underline true
  } else {
    $text tag configure Chosen -underline true
    #$text tag configure Chosen -background gray80
  }
  $text see "$t.last linestart"
  $text see "$t.first linestart"
  # puts "Select: $t"

#  error_editor goto [getfile $errorno ] \
#      [ getline $errorno ] \
#      [ expr [getcolumn $errorno ] ]
}

body ErrorLister::addinfo {msg} {
  # puts stderr "ErrorLister::addinfo '$msg'"
  setstatus normal
  $text insert insert $msg
  setstatus disabled
}

body ErrorLister::AddErr {errno fname line col msg} {
  
  if {$errno == 1} {
    errortool_open
  }

  setstatus normal
  
  # puts "Adderr $errno $fname $line $col"
  savepos $errno $fname $line $col
  set start [ $text index insert ]
  $text insert insert "$msg"
  set end [ $text index insert ]
  set errtag [ errtag $errno ]
  $text tag add $errtag $start $end
  $text tag bind $errtag <ButtonPress-1> "tkif_SetMsgPtr $errno; "
  # puts [bindtags $text]
  $text see $end

  setstatus disabled
  
  update idletasks
}

# Called from tk_if.cc

body ErrorLister::clearAll {} {
  global error`list error`count error`data SN
  
#  $text configure -state normal
  setstatus normal
  $text delete 1.0 end
  setstatus disabled
  set ErrorCount 0
  error_editor reset
}

body ErrorLister::allDone {} {
  first
#  $SN(error`text) configure -state disabled
}

############################################################
# These two function print/pipe the content of the
# 'error' buffer
############################################################
body ErrorLister::print {} {
  general`printText $Name
}
body ErrorLister::pipe {} {
  general`pipeText $Name
}

######################################################################

proc error_lister`print {} {
  error_lister print
}

proc error_lister`pipe {} {
  error_lister pipe
}
