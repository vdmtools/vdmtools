############################################################
# This function pack the trace frame
# w - the window to pack in
############################################################
proc debug_trace`pack w {
  global SN upDnIndicator levelCounter oldIndication
  setSN debug_trace`top $w
  help`add $w interpPage`debug_trace
  general`packLabel $w debug_trace
  set text [tixScrolledText $w.text -scrollbar auto]
  pack $text -fill both -expand 1
  setSN debug_trace`text [$text subwidget text]
  AddFocusWin [$text subwidget text]

  # Prevent a fast click on a collapsed header from being
  # cut&paste "selected". 
  bind $SN(debug_trace`text) <Double-Button-1> "break";
  bind $SN(debug_trace`text) <Up> "debug`down";
  bind $SN(debug_trace`text) <Down> "debug`up";

  if {[array exists upDnIndicator]} {
    unset upDnIndicator
  }
  set levelCounter 0
  set oldIndication 0

  debug_trace`init   
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc debug_trace`save {} {
  general`saveText debug_trace
}
proc debug_trace`load {} {
  general`loadText debug_trace
}

############################################################
# These two function print/pipe the content of the
# 'debug_trace' buffer
############################################################
proc debug_trace`print {} {
  general`printText debug_trace
}
proc debug_trace`pipe {} {
  general`pipeText debug_trace
}

proc debug_trace`setstatus {value} {
  global SN
  $SN(debug_trace`text) configure -state $value
}

proc debug_trace`init {} {
  debug_trace`setstatus disabled
}

  
proc debug_trace`collapsearg {funcid bt_index} {
  global SN

  debug_trace`setstatus normal

  set s [ $SN(debug_trace`text) index $funcid.first ]
  $SN(debug_trace`text) delete $funcid.first $funcid.last
  $SN(debug_trace`text) tag delete $funcid
  $SN(debug_trace`text) insert $s "..." [list $funcid]
  $SN(debug_trace`text) tag bind $funcid <ButtonPress-1> \
      "debug_trace`expandarg [list $funcid $bt_index]"

  debug_trace`setstatus disabled
  
}

proc debug_trace`expandarg {funcid bt_index} {
  global SN
  
  debug_trace`setstatus normal
  set s [ $SN(debug_trace`text) index $funcid.first ]
  $SN(debug_trace`text) delete $funcid.first $funcid.last
  $SN(debug_trace`text) tag delete $funcid
  set args [tkif_funcTrace $bt_index]
  $SN(debug_trace`text) insert $s $args [list $funcid]
  $SN(debug_trace`text) tag bind $funcid <ButtonPress-1> \
      "debug_trace`collapsearg [list $funcid $bt_index]"
  
  debug_trace`setstatus disabled
}

proc debug_trace`clear {} {
  global SN levelCounter upDnIndicator oldIndication
  debug_trace`setstatus normal
  $SN(debug_trace`text) delete 0.0 end
  foreach t [ $SN(debug_trace`text) tag names ] {
    $SN(debug_trace`text) tag delete $t
  }

  if {[array exists upDnIndicator]} {
    unset upDnIndicator
  }
  set levelCounter 0
  set oldIndication 0

  debug_trace`setstatus disabled
}

proc debug_trace`add {func bt_index has_args is_debugLine} {
  global SN levelCounter upDnIndicator

  debug_trace`setstatus normal
  set text $SN(debug_trace`text)

  if {$is_debugLine} {
    $text tag configure prompt -foreground [$text cget -background] -background black
  }
  set pointA [$text index insert]

  $SN(debug_trace`text) insert end "$func "
  set pointB [$text index insert]

  if {$has_args} {
    $SN(debug_trace`text) insert end "( "
    set s [ $SN(debug_trace`text) index insert ]
    $SN(debug_trace`text) insert end "..."
    set e [ $SN(debug_trace`text) index insert ]
    $SN(debug_trace`text) tag add $func $s $e
    $SN(debug_trace`text) tag bind $func <ButtonPress-1> \
        "debug_trace`expandarg [list $func $bt_index]"
    $SN(debug_trace`text) insert end " )"
  }
  
  if {$is_debugLine} {
    $text tag add prompt $pointA [$text index insert]
  }

  $SN(debug_trace`text) insert end "\n"
  $SN(debug_trace`text) see 1.0

  debug_trace`setstatus disabled

  ### set the up/down level indication point information
  incr levelCounter
  
  $text tag add indicator-$func $pointA $pointB
  set upDnIndicator($levelCounter) indicator-$func
  
}

proc debug_trace`gotoUpDownLevel {level} {
  global upDnIndicator oldIndication SN

  if {![array exists upDnIndicator]} {
    # This is the case of the stack is empty. It is empty when a debugging
    # has finished.
    return
  }
  set text $SN(debug_trace`text)
  debug_trace`setstatus normal
  
  ### remove old indication
  if {$oldIndication != 0} {
    set funcid $upDnIndicator($oldIndication)
    $text tag configure $funcid -foreground black -background [$text cget -background]
  } 

  ### Set new indication
  set oldIndication $level
  if {$level != 0} {
    set funcid $upDnIndicator($level)
    $text tag configure $funcid -foreground black -background green
    $text see $funcid.first
  }

  debug_trace`setstatus disabled
}

proc debug_trace`enableWindow {} {
  general`ChangeLabelState debug_trace normal
}

proc debug_trace`disableWindow {} {
  general`ChangeLabelState debug_trace disabled
}
