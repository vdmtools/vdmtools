######################################################################
#
#

proc general`clearwindows {} {
  debug_editor reset
  error_editor reset
  breakpoint`reset
  error_lister clearAll
}


############################################################
# This is the general save/load functions for frames which
# only contain a text widget
# window - then name of the window
# return - nothing
############################################################
proc general`saveText {window} {
  global $window`text $window`insert $window`tags $window`tagdef SN
  global $window`tagbind $window`textState
  global $window`misc 
  set text $SN($window`text)
  set $window`text [$text get 1.0 end-1c]
  set $window`insert [$text index insert]
  foreach tag [$text tag names] {
    set $window`tags($tag) [$text tag ranges $tag]
    set $window`tagdef($tag) {}
    set $window`tagbind($tag) {}

    ### saving the tag definitions
    foreach opt [$text tag configure $tag] {
      set name [lindex $opt 0]
      set value [lindex $opt 4]
      if {$value != {}} {
	lappend $window`tagdef($tag) $name $value
      }
    }

    ### saving the tag bindings
    foreach bind [$text tag bind $tag] {
      lappend $window`tagbind($tag) [list $bind [$text tag bind $tag $bind]]
    }
  }
  ### saving the state of the widget
  set $window`textState [$text cget -state]
  ### save insert mark and first visible line.
  array set $window`misc {}
  set $window`misc(insert) [ $text index insert ]
  set $window`misc(yview) [ lindex  [ $text yview ] 0 ]
}

proc general`loadText {window} {
  global $window`text $window`insert $window`tags $window`tagdef SN
  global $window`tagbind $window`textState
  global $window`misc 

  set text $SN($window`text)
  $text insert end [set $window`text]
  $text yview [set $window`insert]
  focus $text
  foreach tag [array names $window`tags] {

    ### loading the tags
    if {[set $window`tags($tag)] != ""} {
      eval "$text tag add \"$tag\" [set $window`tags($tag)]"
    }

    ### loading the definitions
    eval "$text tag configure \"$tag\" [set $window`tagdef($tag)]"

    ### loading the bindings
    foreach elm [set $window`tagbind($tag)] {
      set bind [lindex $elm 0]
      set sequence [lindex $elm 1]
      $text tag bind $tag $bind $sequence
    }
  }
  ### setting the state
  $text configure -state [set $window`textState]
  ### set mark and scroll to first line
  $text mark set insert [set $window`misc(insert)]
  $text see insert
  ### $text yview moveto [set $window`misc(yview)]
  
}

############################################################
# This function create the label/ function line over
# each frame (modules,errors, function trace etc.)
############################################################
proc general`packLabel {w window} {
  global BITMAPDIR 
  pack [frame $w.label] -fill x
  setSN $window`labelTop $w.label

  set tool [ windows`gettoolroot $window ]
  ### the close button
  #-#button $w.label.close -bitmap bm`close -relief flat \
  #-#    -activeforeground white -activebackground black\
  #-#    -command "$window close"
  #-#pack $w.label.close -side left -ipadx 3 -fill both
  #-#help`add $w.label.close icons`close

  ### the "create toplevel" button
  if {[$window isToplevel]} {
    set arrow moveDown
    set func insertToplevel
  } else {
    set arrow moveUp
    set func createToplevel
  }
  
  #-#button $w.label.up -bitmap bm`$arrow -relief flat \
  #-#    -activeforeground white -activebackground black\
  #-#    -command "$tool $func $window"
  #-#pack $w.label.up -side left -ipadx 3 -fill both
  #-#help`add $w.label.up icons`$arrow

  ### the "move around" button
  #-#if {![$window isToplevel]} {
  #-#  button $w.label.move -bitmap bm`moveAround -relief flat \
  #-#      -activeforeground white -activebackground black\
  #-#      -command "$tool move $window "
  #-#  pack $w.label.move -side left -ipadx 3 -fill both
  #-#  help`add $w.label.move icons`move
  #-#}
  
  ### the clear button
  if {$window == "log" || $window == "command" || $window == "umllog"} {
    button $w.label.clear -bitmap bm`clear -relief flat \
	-activeforeground white -activebackground black\
	-command "$window`clear"
    pack $w.label.clear -side left -ipadx 3 -fill both 
    help`add $w.label.clear icons`clear
  }
  if {! [Config isWinOS]} {
    ### the print button
    if {$window != "modules" && $window != "super_classes" &&
        $window != "sub_classes" && $window != "uses" && $window != "used_by" &&
        $window != "dependencynamemess" && $window != "inhtree" && $window != "breakpoint"} {
      button $w.label.print -bitmap bm`print -relief flat \
          -activeforeground white -activebackground black\
          -command "$window`print"
      pack $w.label.print -side left -ipadx 3 -fill both
      help`add $w.label.print icons`print
    }
  }

  if {! [Config isWinOS]} {
    ### the pipe button
    if {$window != "modules" && $window != "super_classes" &&
      $window != "sub_classes" && $window != "uses" && $window != "used_by" &&
      $window != "dependencynamemess" && $window != "inhtree" && $window != "breakpoint"} {
      button $w.label.pipe -bitmap bm`pipe -relief flat \
          -activeforeground white -activebackground black\
          -command "$window`pipe"
      pack $w.label.pipe -side left -ipadx 3 -fill both
      help`add $w.label.pipe icons`pipe
    }
  }

  ### the edit button
  if {$window == "debug_editor" || $window == "error_editor" || $window == "modules"} {
    button $w.label.edit -bitmap bm`edit -relief flat \
	-activeforeground white -activebackground black\
	-command "$window`edit"
    pack $w.label.edit -side left -ipadx 3 -fill both
    help`add $w.label.edit icons`edit
    setSN $window`editButton $w.label.edit 
  }

  ### The delete break point button
  if {$window == "breakpoint"} {
    button $w.label.delete -bitmap bm`clear -relief flat \
	-activeforeground white -activebackground black\
	-command "breakpoint`delete"
    pack $w.label.delete -side left -ipadx 3 -fill both
    help`add $w.label.delete icons`delete
    setSN $window`editButton $w.label.delete 
  }

  ### The enable/disable break point button
  if {$window == "breakpoint"} {
    button $w.label.pm -bitmap bm`plusminus -relief flat \
	-activeforeground white -activebackground black\
	-command "breakpoint`toggle"
    pack $w.label.pm -side left -ipadx 3 -fill both
    help`add $w.label.pm icons`pm
    setSN $window`editButton $w.label.pm 
  }

   # Do not put the window title into the frame if we have a toplevel window.
  if {[$window isToplevel ]} {
    set ftext ""
  } else {
    set ftext [ $window getTitle ]
  }
  if {$window == "modules"} {
    button $w.label.packages \
        -relief flat \
        -image imbm`hier_off \
        -command "modules`toggleHierarchy $w.label.packages"
    # $w.label.packages select    
    setSN $window`toggleHierarchy $w.label.packages
    help`add $w.label.packages icons`toggleHierarchy
    pack $w.label.packages -side left 
  }

  
  label $w.label.text -text $ftext 
  # -font {-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*}
  setSN $window`label $w.label.text
  
  # make a space on one cm to the left of the text
  label $w.label.space  -bitmap bm`space -width 1c
  setSN $window`space $w.label.space
  pack $w.label.space -side left -fill y 
  pack $w.label.text  -fill both -anchor w -side left

  # Extra Information
  label $w.label.extraInfo
  pack $w.label.extraInfo -fill both -anchor w -side left -padx 0.5c
  setSN $window`extraInfo $w.label.extraInfo
}

proc general`ChangeLabelState {window state} {

  global SN
  set w $SN($window`top)
  ### the clear button
  if {$window == "log" || $window == "command" || $window == "umllog"} {
    $w.label.clear configure -state $state
  }
  if {! [Config isWinOS]} {
    ### the print button
    if {$window != "modules" && $window != "super_classes" &&
        $window != "sub_classes" && $window != "uses" && $window != "used_by" &&
        $window != "dependencynamemess" && $window != "inhtree" && $window != "breakpoint"} {
      $w.label.print configure -state $state
    }

    ### the pipe button
    if {$window != "modules" && $window != "super_classes" &&
      $window != "sub_classes" && $window != "uses" && $window != "used_by" &&
      $window != "dependencynamemess" && $window != "inhtree" && $window != "breakpoint"} {
        $w.label.pipe configure -state $state
    }
  }

  ### the edit button
  if {$window == "debug_editor" || $window == "error_editor" || $window == "modules"} {
    $w.label.edit configure -state $state
  }

  ### The delete break point button
  if {$window == "breakpoint"} {
    $w.label.delete configure -state $state
  }

  ### The enable/disable break point button
  if {$window == "breakpoint"} {
    $w.label.pm configure -state $state
  }
}



############################################################
# This function hilight the frame 'window' if 'state'
# is 'on' otherwise it dehelight the frame
############################################################
proc general`hilight {window state} {
  global SN
  if {$state == "on"} {
    set bg black
    set fg [. cget -bg]
  } else {
    set bg [. cget -bg]
    set fg black
  }

  set w $SN($window`labelTop)

  $w.close configure -bg $bg -fg $fg
  $w.up configure -bg $bg -fg $fg
  if {![$window isToplevel]} {
    $w.move configure -bg $bg -fg $fg
  }
  if {$window == "log" || $window == "command"} {
    $w.clear configure -bg $bg -fg $fg
  }
  if {$window != "modules"} {
    $w.print configure -bg $bg -fg $fg
  }
  if {$window != "modules"} {
    $w.pipe configure -bg $bg -fg $fg
  }
  if {$window == "editor" || $window == "modules"} {
    $w.edit configure -bg $bg -fg $fg
  }
  $w.text configure -bg $bg -fg $fg
  $w.space configure -bg $bg -fg $fg
}
############################################################
# This is a print function for the frames which
# only contains a text widget
############################################################
proc general`printText window {
  global SN options`misc
  set text $SN($window`text)

  windows`Busy on 
  set selection [selection own]
  set range [$text tag ranges sel]
  if {$selection == $text && [llength $range]==2} {
    set content [eval $text get $range]
  } else {
    set content [$text get 1.0 end]
  }
  set cmd [functions`expandCommand $window [set options`misc(print)]]
  
  if {[catch {set OUT [open "|$cmd" w]} err]} {
    runtime`userError $err ""
  } else {
    if {[catch {
      puts $OUT $content
      close $OUT
    } err]}  {
      runtime`userError $err ""
    }
  }
  windows`Busy off 
}

############################################################
# This is a pipe function for frames which
# only contain a text widget
############################################################
proc general`pipeText window {
  global SN options`misc pipe`state $window`pipeCmd
  set text $SN($window`text)

  ### getting the text which have to be piped
  set selection [selection own]
  set range [$text tag ranges sel]
  if {$selection == $text && [llength $range]==2} {
    set content [eval $text get $range]
  } else {
    set content [$text get 1.0 end]
  }

  ### creating the window which ask for the pipe command
  toplevel .pipe
  tixLabelEntry .pipe.cmd -label "Command to pipe to" \
      -options "entry.textVariable $window`pipeCmd"
  bind [.pipe.cmd subwidget entry] <Return> ".pipe.buts.ok invoke"
    
  
  label .pipe.help -wraplength 20c -justify left -text "In the command, the following substitutions occur:\n%f - the name of the frame, or the name of the file (for editor window)\n%d - The current date\n%t - The current time"


  ### the Ok and Cancel button
  frame .pipe.buts
  button .pipe.buts.ok -text Ok -command {
    global pipe`state
    set pipe`state ok
    destroy .pipe
  }
  button .pipe.buts.cancel -text Cancel -command {
    global pipe`state
    set pipe`state cancel
    destroy .pipe
  }

  ### packing the toplevel window, and waiting for it to exit
  pack .pipe.buts.ok .pipe.buts.cancel -side left
  pack .pipe.cmd .pipe.help .pipe.buts -pady 10 -fill x -anchor w
  focus [.pipe.cmd subwidget entry]
  tkwait window .pipe

  ### if the cancel button was pressed, then exit
  if {${pipe`state} == "cancel"} {
    return
  }

  ### expand the command, and evaluate it
  set cmd [functions`expandCommand $window [set $window`pipeCmd]]
  if {[catch {set OUT [open "|$cmd" w]} err]} {
    runtime`userError $err ""
  } else {
    puts $OUT $content
    close $OUT
  }
}

############################################################
# This will clear the text widget in 'win'
# return - 1 - if the window has been cleared
#          0 - if the clear has been canceled
############################################################
proc general`clear win {
  global SN 
  set answer [tk_dialog .dialog "Clear frame" "Clear the window titled [ $win getTitle ] ?" questhead 0 OK Cancel Help]

  if {$answer == 0} {
    set text $SN($win`text)
    $text delete 1.0 end
    return 1
  } elseif {$answer == 2} {
    help`gotoTag window`clear
  }
  return 0
}

# Tix does not like / in file names on Win32
proc general`winfixslash {filename} {
  if {[Config isWinOS]} {
      regsub -all -- {/} $filename {\\} filename
  }
  return $filename
}

proc general`backslashprotectlist {l} {
  #if {[Config isWinOS]} {
    set L ""
    # error "hey"
    #foreach f $l {
      set f $l
      regsub -all -- {\\} $f {/} f
      # puts "protect: '$l:$f'"
      return $f
    #}
    return $L
  #} else {
  #  return $l
  #}
}

proc general`exec {cmd} {
  if {[catch {} err]} {
    runtime`userError $err ""
  }
}

proc general`startEditorAtPos {file line} {
  global options`editor

  set Filespec [set options`editor(fileSpec)]
  set Editor [set options`editor(name)]
  set Prefix [set options`editor(prefix)]
  set Postfix [set options`editor(postfix)]

  set spec [set options`editor(fileSpecWithLineNo)]
  set spec_list [split $spec " "]
  set args ""
  foreach s $spec_list {
    # Substitute the line no first. Otherwise if the file name
    # contains a %l and fileSpecWithLineNo don't contain a %l
    # we will mess up the file name. 
    
    regsub {%l} $s $line s
    regsub {%f} $s $file s
    lappend args $s
  }
  set execcmds [concat [list $Editor] $args]
  # puts $execcmds
  eval exec $execcmds & 
}

proc general`startEditor {files} {
  global options`editor
  # puts "general`startEditor $files"

  set Filespec [set options`editor(fileSpec)]
  set Editor [set options`editor(name)]
  
  set spec_list [split $Filespec " "]
  if {[set options`editor(multipleFiles)] == 1} {
    set args ""
    foreach file $files {
      foreach s $spec_list {
        regsub {%f} $s $file s
        lappend args $s
      }
    }
    set execcmds [concat [list $Editor] $args]
    eval exec $execcmds &
  } else {
    foreach file $files {
      set args ""
      foreach s $spec_list {
        regsub {%f} $s $file s
        lappend args $s
      }
      set execcmds [concat [list $Editor] $args]
      eval exec $execcmds &
    }
  }
}


proc general`initMeter { title lbltxt } {
  if {![winfo exist .vppmeter]} {
    set w .vppmeter
    toplevel $w
    wm title $w $title
    if {[string length $lbltxt] > 32} {
      set lbltxt [string range $lbltxt 0 32]
    }
    # Create the Meter and the Label
    #
    label $w.lbl -text $lbltxt
    tixMeter $w.met -value 0 -text 0% -width 200

    pack $w.lbl -side top -padx 10 -pady 5 -anchor c
    pack $w.met -side top -padx 30 -pady 5 -anchor c
    update
  }
}


proc general`setMeter { val txt } {
  if {($val <= 1.0) && ($val >= 0)} {
    if {[string length $txt] > 32} {
      set txt [string range $txt 0 32]
    }
    .vppmeter.lbl config -text $txt
    set text [expr int($val*100.0)]%
    .vppmeter.met config -value $val -text $text
    update
  }
}


proc general`destroyMeter {} {
  if {[winfo exist .vppmeter]} {
    destroy .vppmeter
  }
}

