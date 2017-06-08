############################################################
# This function pack the breakpoint frame
# w - the window to pack in
############################################################
proc breakpoint`pack w {
  setSN breakpoint`top $w
  help`add $w interpPage`breakpoint
  general`packLabel $w breakpoint


  listbox $w.lb -yscrollcommand  "$w.scroll set" -selectmode extended
  scrollbar $w.scroll -command "$w.lb yview"
  pack $w.scroll -side right -fill y
  pack $w.lb -fill both -expand 1

  setSN breakpoint`lb $w.lb
  AddFocusWin $w.lb
  breakpoint`init 
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc breakpoint`save {} {
  general`saveText breakpoint
}
proc breakpoint`load {} {
  general`loadText breakpoint
}

############################################################
# These two function print/pipe the content of the
# 'breakpoint' buffer
############################################################
#proc breakpoint`print {} {
#  general`printText breakpoint
#}
#proc breakpoint`pipe {} {
#  general`pipeText breakpoint
#}
#

proc breakpoint`init {} {
}

proc breakpoint`reset {} {
  global SN
  $SN(breakpoint`lb) delete 0 end
}

############################################################
# This function (de)hilight the frame
# state - either on or off
############################################################
proc breakpoint`hilight {state} {
  general`hilight breakpoint $state
}

proc breakpoint`addBreakName {name num} {
  global SN breakNums
  set lb $SN(breakpoint`lb)

  $lb insert end "\[+\] $name"
  set breakNums($name) $num
}

proc breakpoint`addBreakPos {file line col num} {
  global SN breakNums
  if {$col != 1} {
    set text "$file line $line column $col"
  } else {
    set text "$file line $line"
  }

  set lb $SN(breakpoint`lb)
  $lb insert end "\[+\] $text"
  set breakNums($text) $num
}

### This function is called from the toolbox when a breakpoint is deleted.
proc breakpoint`deletebreak {num} {
  global SN breakNums
  set lb $SN(breakpoint`lb)
  for {set i 0} {$i<[$lb size]} {incr i} {
    set elm [string range [$lb get $i] 4 end]
    if {$num == $breakNums($elm)} {
      $lb delete $i
      unset breakNums($elm)
      break
    }
  }
}

proc breakpoint`delete {} {
  global SN breakNums
  set lb $SN(breakpoint`lb)
  set names {}

  # This two step approch is neccesary as the index otherwise might change when elements are deleted!
  foreach index [$lb curselection] {
    lappend names [string range [$lb get $index] 4 end]
  }

  foreach name $names {
    debug`menucmd "delete $breakNums($name)"
  }
}

proc breakpoint`toggle {} {
  global SN breakNums
  set lb $SN(breakpoint`lb)
  set elms {}
  foreach index [$lb curselection] {
    lappend elms [$lb get $index]
  }

  foreach elm $elms {
    set name [string range $elm 4 end]
    if {[string match {\[-\] *} $elm]} {
      debug`menucmd "enable $breakNums($name)"
    } else {
      debug`menucmd "disable $breakNums($name)"
    }
  }
}


proc breakpoint`enableDisable {num txt} {
  global SN breakNums
  set found 0

  ### Search for the name of the breakpoint
  foreach name [array names breakNums] {
    if {$breakNums($name) == $num} {
      set found 1
      set theName $name
      break
    }
  }
  
  if {!$found} {
    error "Internal error breakpoint #$num not found"
  }

  ### Next search for the index in the listbox
  set lb $SN(breakpoint`lb)
  set found 0
  for {set i 0} {$i<[$lb size]} {incr i} {
    set elm [string range [$lb get $i] 4 end]
    if {$elm == $theName} {
      set found 1
      set index $i
    }
  }

  if {!$found} {
    error "Internal error: breakpoint named `$name' not found"
  }

  ### Set the element in the listbox
  $lb insert $index "$txt $name"
  $lb delete [expr $index +1]
}


proc breakpoint`enable {num} {
  breakpoint`enableDisable $num {[+]}
}

proc breakpoint`disable {num} {
  breakpoint`enableDisable $num {[-]}
}


set brakpoint`bindTags {
  "<1>" "<Double-1>" "<B1-Motion>" "<ButtonRelease-1>" "<Shift-1>" "<Control-1>" "<B1-Leave>" "<B1-Enter>" 
  "<Up>" "<Shift-Up>" "<Down>" "<Shift-Down>" "<Left>" "<Control-Left>" "<Right>" "<Control-Right>" "<Prior>" 
  "<Next>" "<Control-Prior>" "<Control-Next>" "<Home>" "<End>" "<Control-Home>" "<Shift-Control-Home>" 
  "<Control-End>" "<Shift-Control-End>" "<space>" "<Select>" "<Control-Shift-space>" "<Shift-Select>" 
  "<Escape>" "<Control-slash>" "<Control-backslash>" "<2>" "<B2-Motion>"
}

proc breakpoint`enableWindow {} {
  global SN brakpoint`bindTags
  set lb $SN(breakpoint`lb)
  foreach  b ${brakpoint`bindTags} {
    bind $lb $b ""
  }
  general`ChangeLabelState breakpoint normal
}

proc breakpoint`disableWindow {} {
  global SN brakpoint`bindTags
  set lb $SN(breakpoint`lb)

  foreach  b ${brakpoint`bindTags} {
    bind $lb $b break
  }
  general`ChangeLabelState breakpoint disabled
}

proc breakpoint`checkBreakpointsFile {} {
  set breakPointFile [tkif_tmpBreakFileName]

  if [file exists $breakPointFile] {
    set FP [open $breakPointFile]
    while {![eof $FP]} {
      set line [gets $FP]
      if {[regexp "^ *$" $line]} {
        # empty line
      } elseif {![regexp {^(.*)-\+-\+-(.*)-\+-\+-(.*)$} $line all file lineNo colNo]} {
        error "Invalid line read from \"$breakPointFile\": \"$line\""
      } else {
        debug`menucmd "break [tkif_GetBasename $file] $lineNo $colNo"
      }
    }
	close $FP
    file delete $breakPointFile
  }
}
