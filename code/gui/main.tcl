######################################################################
# Main file. 
#
# $Id: main.tcl,v 1.33 2000/03/23 08:03:53 blackie Exp $
######################################################################

### testing if the argument count is right

#if {$argc !=0 && $argc != 1} {
#  puts "Wrong no. of args"
#  puts "Syntax: $argv0 [project]"
#  exit
#}
#puts "argc=$argc, argv=$argv"

### this flag indicate that the interface is initializing
set MODE main

set WinDirHack 0
if {[info exists tcl_platform(platform)] && \
    $tcl_platform(platform)=="windows"} {
  # Hack to add proper image path
  tix addbitmapdir $tix_library/bitmaps
  set WinDirHack 1
}

class MiscConfig {
  method isMonochome {} { return [expr [winfo depth .]==1]; }
  method isWinOS {} {
    global tcl_platform
    return [ expr {[info exists tcl_platform(platform)] &&   
           [string compare $tcl_platform(platform) "windows"]==0} ]
  }
}
MiscConfig Config

### source all the files of the project

source   default.tcl
source   util.tcl
source   menuline.tcl
source   windows.tcl
source   help.tcl
source   error.tcl
source   project.tcl
source   stub.tcl
source   buttons.tcl
source   functions.tcl
source   options.tcl
source   modules.tcl
source   editor.tcl
source   command.tcl
source   log.tcl
source   trace.tcl
source   breakpoint.tcl
source   variable.tcl
source   general.tcl
source   runtime_eh.tcl
source   debug.tcl
source   interp.tcl
if {[tkif_IsVdmppType]} {
  source   dependency.tcl
  source   inhtree.tcl
}
source   tcovtxt.tcl
if {[tkif_IsUmlType]} {
  source   umltool.tcl
}

### creating a stand in for log`add until the window is created
set main`loglist {}
set logproc [info body log`add]
set logargs [info args log`add]
proc log`add {args} {
  global main`loglist
  lappend main`loglist $args
}

### loading options and layout 
options`load
# windows`loadLayout

### packing the interface
Viewer error_editor error_editor Display
Viewer debug_editor debug_editor "Display"
ErrorLister error_lister error_lister "Errors and Warnings"

FrameWin log         log          "Log"
FrameWin umllog      umllog       "Log"
if {[tkif_IsVdmslType] || [tkif_IsVdmslLiteType]} {
  FrameWin modules     modules      "Modules"
}
if {[tkif_IsVdmppType]} {
  FrameWin modules     modules      "Classes"
}
FrameWin command     command      "Dialog"
FrameWin debug_trace debug_trace  "Function Trace"
FrameWin breakpoint  breakpoint   "Breakpoints"
if {[tkif_IsVdmppType]} {
  FrameWin dependencynamemess dependencynamemess ""
  FrameWin super_classes super_classes "Superclasses"
  FrameWin uses          uses          "Uses"
  FrameWin sub_classes   sub_classes   "Subclasses"
  FrameWin used_by       used_by       "Used by"
  FrameWin inhtree       inhtree       "Inheritance Tree"
}

Layout maintool maintool [set windows`sequence(maintool)] [tkif_GiveToolTitle]
maintool Pack
Layout interptool interptool [set windows`sequence(interptool)] "Interpreter tool"
interptool Pack
Layout errortool errortool [set windows`sequence(errortool)] "Error tool"
errortool Pack
if {[tkif_IsVdmppType]} {
  Layout dependencytool dependencytool [set windows`sequence(dependencytool)] "Dependency tool"
  dependencytool Pack
  Layout inhtreetool inhtreetool [set windows`sequence(inhtreetool)] "Inheritance tool"
  inhtreetool Pack
}


#interptool_open
#errortool_open

# wm geometry . [set windows`geometry(maintool,.)]
wm withdraw .
wm withdraw .errortool
wm withdraw .interptool
if {[tkif_IsVdmppType]} {
  wm withdraw .dependencytool
  wm withdraw .inhtreetool
}

wm protocol .maintool   WM_DELETE_WINDOW "project`quit"
wm protocol .interptool WM_DELETE_WINDOW "wm withdraw .interptool"
wm protocol .errortool WM_DELETE_WINDOW "wm withdraw .errortool"
if {[tkif_IsVdmppType]} {
  wm protocol .dependencytool WM_DELETE_WINDOW "wm withdraw .dependencytool"
  wm protocol .inhtreetool WM_DELETE_WINDOW "wm withdraw .inhtreetool"
}

### starting up in main mode
pack .maintool.root -fill both -expand 1
pack .interptool.root -fill both -expand 1
pack .errortool.root -fill both -expand 1
if {[tkif_IsVdmppType]} {
  pack .dependencytool.root -fill both -expand 1
  pack .inhtreetool.root -fill both -expand 1
}

#menuline`disable projectMenu Configure
#menuline`disable projectMenu "Clean Up"

modules`selectFunctions
debug_editor set_edit_button 
error_editor set_edit_button 
project`newtitle ""
command`insertPrompt

### Now the log function is ready for use
proc log`add $logargs $logproc
foreach log ${main`loglist} {
  eval log`add $log
}

### add token __top__ to point at the top, and insert it
### in the help text to be sure, that there always is a match
set help`ref(__top__) 1.0
help`add . __top__

proc emacslog`addmsg {msg} {
  puts -nonewline stdout $msg
}

proc emacs_reader {} {
  set line [ gets stdin ]
  tkif_executeCommand $line emacslog`addmsg
  puts -nonewline stdout "vdm> "
  flush stdout
}

proc emacs`goto {file line col} {
  global env
  if {[info exists env(EMACS)] && $env(EMACS)=="t"} {
    puts stdout "\032\032$file:$line:1"
  }
  flush stdout
}
  
if {[info exists env(EMACS)] && $env(EMACS)=="t"} {
  puts -nonewline stdout "vdm> "
  flush stdout
  fileevent stdin readable emacs_reader
}

# loading a project
#if {$argv != ""} {
# 
#}
#puts "$argc $argv"
if {$argc == 1} {
   project`openNow [lindex $argv 0]
}

bind all <F5> { help_dumpindex }

### bind enter of the syntax tool to update the status of modules.
bind .maintool <FocusIn> "if {\"%W\" == \".maintool\"} { modules`ChangeAllStates; breakpoint`checkBreakpointsFile }"

bind .interptool <FocusIn> "if {\"%W\" == \".interptool\"} { modules`ChangeAllStates; breakpoint`checkBreakpointsFile }"

