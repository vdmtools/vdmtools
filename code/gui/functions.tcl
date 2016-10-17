######################################################################
# User functions handling. 
#
# $Id: functions.tcl,v 1.25 2000/08/17 08:05:15 paulm Exp $
######################################################################


############################################################
# This function will syntax check the seleted files
############################################################
proc functions`syntaxCheck {} {
  windows`Busy on
  set files [modules`selectedFiles]
#  foreach file $files {
  tkif_syntaxCheck $files mainlog`addmsg
#  }
#  tkif_updateAssocs   ## This should be moved to be taken care of by the C++ code
  windows`Busy off
}

############################################################
# This function will type check the selected files
############################################################
proc functions`typeCheck {} {
  windows`Busy on
  tkif_typeCheck [modules`selectedModules] mainlog`addmsg
  windows`Busy off
}

############################################################
# This will pretty print the selected files/modules
############################################################
proc functions`prettyPrint {} {
  windows`Busy on
  tkif_prettyPrint [modules`selectedFiles] mainlog`addmsg
  windows`Busy off
}

############################################################
# This function will generate C++ code for ALL the
# selected files, (even if they are up to date)
############################################################
proc functions`generateC++ {} {
  windows`Busy on
  tkif_codeGenerate [modules`selectedModules]  mainlog`addmsg
  windows`Busy off
}

proc functions`generateJava {} {
  windows`Busy on
  tkif_javaCodeGenerate [modules`selectedModules]  mainlog`addmsg
  windows`Busy off
}

proc function`functions {} {
  windows`Busy on
  tkif_executeCommand "functions"
  windows`Busy off
}
proc functions`modules {} {
  windows`Busy on
  tkif_executeCommand "modules"
  windows`Busy off
}

proc functions`functions {} {
  windows`Busy on
  tkif_executeCommand "functions"
  windows`Busy off
}

proc functions`states {} {
  windows`Busy on
  tkif_executeCommand "states"
  windows`Busy off
}

proc functions`types {} {
  windows`Busy on
  tkif_executeCommand "types"
  windows`Busy off
}

proc functions`values {} {
  windows`Busy on
  tkif_executeCommand "values"
  windows`Busy off
}

proc functions`push {} {
#  tkif_executeCommand "values"
}

proc functions`pop {} {
  windows`Busy on
  tkif_executeCommand "pop"
  windows`Busy off
}
proc functions`stack {} {
  windows`Busy on
  tkif_executeCommand "stack"
  windows`Busy off
}

############################################################
# This functions enable/disable a command button named
# 'name' on the frame 'w'
# w - the name of the window, which the button is in
# name - the name of the button
# state - the state to set
############################################################
proc functions`setWindowButtonState {w name state} {
  global SN
  set ww [set SN($w`top)].label.$name
  # It is not for certain that the window has the button
  if {[winfo exist $ww]} { 
    $ww configure -state $state
  }
}

proc functions`setState {menu text state} {
  menuline`setState $menu [capitalize $text] $state
  buttons`setState $text $state
}

############################################################
# This function expand the command to print or pipe
# cmd    - the command to print
# return - an escaped command
############################################################
proc functions`expandCommand {window cmd} {
  ### exchanging the %f with the name of the buffer or the file in the buffer
  if {$window == "editor"} {
    set str [editor`getFileName]
  } else {
    set str [ $window getTitle ]
  }
  regsub -all -- %f $cmd $str cmd
  set clockval [ clock seconds ]
  set date [clock format $clockval -format {%m/%d/%Y} ]
  set time [clock format $clockval -format {%H:%M:%S} ]
  regsub -all -- %d $cmd $date cmd
  regsub -all -- %t $cmd $time cmd
  return $cmd
}

############################################################
# This function write "text" to the optionfile, in
# the region "token"
############################################################
proc functions`writeOptions {token text} {
  global options`file
  if {[file exists [set options`file]]} {
    set IN [open [set options`file] r]
  } else {
    set IN -1
  }
  
  set outFile "/tmp/toolbox-[pid]"
  set OUT [open $outFile w]
  
  if {$IN != -1} {
    set notEof 0
    ### reading until the begin token
    while {[gets $IN line] != -1} {
      if {$line == "begin $token"} {
        set notEof 1
        break
      }
      puts $OUT $line
    }
    ### reading until the end token
    set notEof 0
    while {[gets $IN line] != -1} {
      if {$line == "end $token"} {
        set notEof
        break
      }
    }
  }  

  puts $OUT "begin $token"
  foreach line $text {
    puts $OUT $line
  }
  puts $OUT "end $token"

  ### reading the rest of the file
  if {$IN != -1} {
    while {[gets $IN line] != -1} {
      puts $OUT $line
    }
  }
  if {$IN != -1} {
    close $IN
  }
  close $OUT
  exec mv $outFile [set options`file]
}

############################################################
# This function read options which relates "token" from
# the option file
############################################################
proc functions`readOptions token {
  global options`file
  set file ""
  set text {}
  if {[file exists ./.vdmslrc]} {
    set file "./.vdmslrc"
  } else {
    # glob may throw an error if the argument is not a valid path
    # e.g., if it does not exist. 
    if {[catch {glob ~/}]} {
      return ""
    }
    if {[file exists  [glob ~/].vdmslrc]} {
      set file [glob ~/.vdmslrc]
    } else {
      return ""
    }
  }

  set options`file $file
  set IN [open $file r]
  set eof 1
  while {[gets $IN line] != -1} {
    if {$line == "begin $token"} {
      set eof 0
      break
    }
  }
  if {!$eof} {
    set eof 1
    while {[gets $IN line] != -1} {
      if {$line == "end $token"} {
        set eof 0
        break
      }
      lappend text $line
    }
    if {$eof} {
      runtime`userError "Token \"end $token\" not found in $file" options`load`endGui-tkNotFound
    }
  }
  return $text
}

proc AddFocusWin {w} {
  bind $w <Enter> "maybe_setFocus $w"
}  

proc maybe_setFocus {w} {
  global options`misc functions`interfaceEnabled
  if {[set options`misc(giveFocus)] && [set functions`interfaceEnabled]} {
    focus $w
  }
}

############################################################
# This function sends a signal to the interpreter to
# stop.
############################################################
proc functions`stopInterpreter {} {
  tkif_StopInterpreter
}

############################################################
# This function disables the interface when interpretation
# runs.
############################################################
proc function`disableInterface {} {
  global functions`interfaceEnabled
  grab set [interptool GetToplevel]
  buttons`changeInterptoolState disable
  breakpoint`disableWindow
  command`disableWindow
  debug_trace`disableWindow
  debug_editor`disableWindow
  set functions`interfaceEnabled 0
}

############################################################
# This function enables the interface when interpretation
# runs.
############################################################
proc function`enableInterface {} {
  global functions`interfaceEnabled
  grab release [interptool GetToplevel]
  buttons`changeInterptoolState enable
  breakpoint`enableWindow
  command`enableWindow
  debug_trace`enableWindow
  debug_editor`enableWindow
  set functions`interfaceEnabled 1
}

set functions`interfaceEnabled 1
