############################################################
#                   Global variables
############################################################
set command`cmds {}  ;# history of previous commands
set command`index -1 ;# current indedx in the history
set command`lastLine "";# the last supplied match
set command`lastSearch ""; # The seach string the user typed.
set command`prompt "vdm> "
set command`lastCmd ""; # The last command executed

############################################################
# This function pack the command frame
# w - the window to pack in
############################################################
proc command`pack w {
  global command`prompt
  setSN command`top $w
  help`add $w interpPage`dialog
  general`packLabel $w command
  set Stext [tixScrolledText $w.text -scrollbar auto]
  pack $Stext -fill both -expand 1

  set text [$Stext subwidget text]
  AddFocusWin $text
  bind $text <Return> "set command`index -1;command`evaluate 0;break"
  bind $text <Control-Return> "set command`index -1;command`evaluate 1;break"
  bind $text <F2> "set command`index -1;$text insert insert {`}"

  ### History mechanism
  bind $text <Up>    "command`nextHist 1; break"
  bind $text <Alt-p> "command`nextHist 1; break"
  bind $text <Down>  "command`nextHist -1; break"
  bind $text <Alt-n> "command`nextHist -1; break"
  bind $text <Key>   "set command`index -1"
  bind $text <ButtonRelease-1> "$text mark set insert end"
  bind $text <Prior> "tkTextScrollPages %W -1;break"
  bind $text <Next> "tkTextScrollPages %W 1;break"
  bind $text <Control-Home> "break"

  ### ensure that the command prompt is not erasured.
  foreach key {<BackSpace> <Left>} {
    bind $text $key "set command`index -1;if {\[$text get {insert linestart} insert\] == \"${command`prompt}\"} break"
  }
  bind $text <Control-a> "command`begofline;break"
  bind $text <Home> "command`begofline;break"
  bind $text <ButtonRelease-2> "set command`index -1;command`paste %x %y;break"
  if {[Config isWinOS]} {
    bind $text <Control-v> "set command`index -1;command`paste %x %y;break"
  }

  bindtags $text "$text Text all"
  setSN command`text $text
  $text tag configure prompt -foreground white -background black
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc command`save {} {
  general`saveText command
}
proc command`load {} {
  general`loadText command
}

############################################################
# These two function print/pipe the content of the
# 'command' buffer
############################################################
proc command`print {} {
  general`printText command
}
proc command`pipe {} {
  general`pipeText command
}

############################################################
# This function takes care of evaluating command
# which is typed in the command window
############################################################
proc command`evaluate {isControlPressed} {
  global SN command`prompt command`cmds command`index command`lastCmd
  set text $SN(command`text)
  set index [$text index insert]
  set end [$text index end]

  set startLine [$text index "insert linestart"]
  set endLine [$text index "insert lineend"]
  set line [$text get $startLine $endLine]
  set line [string range $line [string length ${command`prompt}] end]

  set line [string trim $line " \t\n"]
  if {[string match "${command`prompt}*" $line]} {
    set line [string range $line [string length ${command`prompt}] end]
  }
  
  if {$line != ""} {
    # If the new cmd line exists anywhere in the history then remove
    # it before adding it as the newest. 
    set index [lsearch -exact ${command`cmds} $line]
    if {$index != -1} {
      set command`cmds [lreplace ${command`cmds} $index $index]
    }    
    set command`cmds [concat [list $line] ${command`cmds}]
  }

  ## This can only happend, if you place the cursor offline with the mouse.
  if {[$text index $endLine] != [$text index end-1c]} {
    $text delete "end-1lines linestart" "end-1lines lineend"
    command`insertPrompt
    $text insert end "$line"
    $text mark set insert "end linestart"
    $text see insert
    return
  }

  if {$isControlPressed && $line == ""} {
    # The line was empty and the user pressed control-enter
    set line [set command`lastCmd]
    $text insert end $line
  }

  command`docmd $line
  set command`lastCmd $line

  set command`index -1
}

# 'debug_editor' is prefix name set by tkif_SetEditorName
proc debug_editor`goto {file line col} {
  debug_editor goto $file $line $col
}

proc command`docmd {cmd} {
  global SN
  set text $SN(command`text)

  $text mark set insert end
  $text insert insert "\n"
  $text see insert
  tkif_SetEditorName debug_editor
  if {$cmd != ""} {
    tkif_executeCommandPrompt $cmd commandlog`addmsg
  } else {
    command`insertPrompt
  }
#  if {$cmd != ""} {
#    set printprompt [ tkif_executeCommand $cmd commandlog`addmsg ]
#  } else {
#    set printprompt 1
#  }
#  if {$printprompt} {
#    command`insertPrompt
#  }
}

proc command`removePrompt {} {
  global SN
  set text $SN(command`text)
  set l [ $text tag ranges lastprompt ]
  if {[llength $l] > 0} {
    $text delete lastprompt.first lastprompt.last
  }
}

############################################################
# This function browses to the next element of the history
############################################################
proc command`nextHist {direction} {
  global command`cmds command`index command`prompt SN command`lastLine command`lastSearch
  set text $SN(command`text)
  set line [$text get "insert linestart" insert]
  set lineCmd [string range $line [string length ${command`prompt}] end]
  set lastIndex ${command`index}

  if {![string match ${command`prompt}* $line]} {
    ### I can't see any prompt!
    return
  }
  if {$lineCmd == ${command`lastLine}} {
    set lineCmd ${command`lastSearch}
  } else {
    ## text added to the search
    set command`lastSearch $lineCmd
  }
  if {${command`index} < [expr [llength ${command`cmds}] -1] || $direction == -1} {
    incr command`index $direction
  }
  
  while {(${command`index} >= -1 && $direction == -1) || (${command`index} < [llength ${command`cmds}]  && $direction == 1)} {
    set cmd [lindex ${command`cmds} ${command`index}]
    if {[string match $lineCmd* $cmd]} {
      $text delete "insert linestart" insert
      command`insertPrompt
      $text insert insert $cmd
      set command`lastLine $cmd
      $text see insert
      break
    }
    incr command`index $direction
  }
  if {${command`index} == [llength ${command`cmds}]} {
    set command`index $lastIndex
  }
  if {${command`index} < 0} {
    set command`index -1
    $text delete "insert linestart" insert
    command`insertPrompt
  }
  if {${command`index} > [llength ${command`cmds}]} {
    set command`index [llength ${command`cmds}]
  }
}

############################################################
# This function takes care of inserting a prompt in
# inversed video
############################################################
proc command`insertPrompt {} {
  global SN command`prompt
  set text $SN(command`text)
  set dodisable 0
  if {[$text cget -state] == "disabled"} {
    $text configure -state normal
    set dodisable 1
  }
  $text tag delete lastprompt
  set index [$text index end-1c]
  $text insert $index ${command`prompt} lastprompt
  $text tag add prompt $index $index+4c
  if {$dodisable} {
    $text configure -state disabled
  }

}


############################################################
# This function add an error from the expression parser.
############################################################
proc command`AddErr {idx file line col txt} {
  global SN
  set text $SN(command`text)
  set dodisable 0
  if {[$text cget -state] == "disabled"} {
    $text configure -state normal
    set dodisable 1
  }
  $text insert end "$txt"
  $text see insert
  if {$dodisable} {
    $text configure -state disabled
  }
}


############################################################
# This function clear the command window
############################################################
proc command`clear {} {
  global SN
  set text $SN(command`text)
  set clear [general`clear command]
  if {$clear} {
    command`insertPrompt
  }
}

proc command`paste {x y} {
  global SN
  set text $SN(command`text)
  if {![catch {set sel [selection get]}]} {
    $text insert insert [join [split $sel "\n"] " "] 
  }
  $text yview end
}

proc command`enableWindow {} {
  global SN
  set text $SN(command`text)
  $text configure -state normal
  general`ChangeLabelState command normal  
}
proc command`disableWindow {} {
  global SN
  set text $SN(command`text)
  $text configure -state disabled
  general`ChangeLabelState command disabled
}

proc command`begofline {} {
  global command`index SN command`prompt
  set command`index -1;
  $SN(command`text) mark set insert "insert linestart + [string length ${command`prompt}]chars"
}
