###
### Default options
###

### Default options
if {[info exists env(VDM_EDITOR)]} {
    set options`editor(name) $env(VDM_EDITOR)
} else {
    if {[info exists env(EDITOR)]} {
	set options`editor(name) $env(EDITOR)
    } else {
	if {! [Config isWinOS]} {
	    set options`editor(name) "emacs"
	} else {
	    set options`editor(name) "notepad"
	}
    }
}
if {[Config isWinOS]} {
  set options`editor(prefix) ""
  set options`editor(fileSpec) "%f"
    if {[info exists env(VDM_EDITOR)]} {
	set options`editor(fileSpecWithLineNo) "-l %l %f"
      } else {
	set options`editor(fileSpecWithLineNo) "%f"
      }
  set options`editor(postfix) ""
  set options`editor(multipleFiles) 0
  set options`misc(print) {}
  set options`misc(giveFocus) 0
} else {
  set options`editor(prefix) ""
  set options`editor(fileSpec) "%f"
  set options`editor(fileSpecWithLineNo) "+%l %f"
  set options`editor(postfix) ""
  set options`editor(multipleFiles) 1
  set options`misc(print) {lpr}
  set options`misc(giveFocus) 1
}

set option_vdm_setting(seed) -1
set option_vdm_setting(tex) 1
set option_vdm_setting(pos) 1
set option_vdm_setting(sep) 0


### This is a list of bitmaps, to display for the
### module listbox
foreach var {
  {syntax    none  none}
  {syntax    error checkFailed}
  {syntax    ok  checkOk}
  {syntax    error-changed checkFailedChanged}
  {syntax    ok-changed  checkOkChanged}
  {type      none  none}
  {type      error  typeFailed}
  {type      ok  typeOk}
  {generated none  none}
  {generated error  generatedWarn}
  {generated ok  generatedOk}
  {generatedJava none  none}
  {generatedJava error  generatedJavaWarn}
  {generatedJava ok  generatedJavaOk}
  {printed   none  none}
  {printed   ok  printedOk}
  {printed   error  printedFailed}
} {
  set STATEBITMAP([lindex $var 0],[lindex $var 1]) [lindex $var 2]
}

### setting the default buttons

if {[tkif_IsVdmppType]} {
  set options`buttons(maintool) {"syntax check" "type check" "generate c++" "generate java" "pretty print"}
} else {
  set options`buttons(maintool) {"syntax check" "type check" "generate c++" "pretty print"}
}

set options`buttons(interptool) {"init" "step" "step in" "single step"
                            "continue" "finish" "up" "down" "stop interpreter"}
set options`buttons(errortool) {}
if {[tkif_IsVdmppType]} {
  set options`buttons(dependencytool) {}
  set options`buttons(inhtreetool) {}
}

### This is the default layout
set windows`sequence(maintool) {
  {{modules 125 0.5} }
  {{log  150 0.5}}
}
set windows`sequence(interptool) {
  { {command 150 0.5} }
  { {debug_editor 200 1} }
  { {debug_trace  150 0.5} {breakpoint 150 0.5}}
}

set windows`sequence(errortool) {
  { {error_lister 200 1.0} }
  { {error_editor 200 1.0} }
}

if {[tkif_IsVdmppType]} {
  set windows`sequence(dependencytool) {
    { {super_classes 150 0.5} {uses    150 0.5} }
    { {sub_classes   150 0.5} {used_by 150 0.5} }
  }

  set windows`sequence(inhtreetool) {
    { {inhtree 300 1.0} }
  }
}

if {[info exists geometry]} {
  set windows`geometry(maintool,.) $geometry
  set windows`geometry(debug,.) $geometry
  puts "Using $geometry"
} else {  
  set windows`geometry(maintool,.) "600x400+10+10"
  # set windows`geometry(debug,.) "803x400+0+0"
  set windows`geometry(interptool,.) "600x667+0+0"
  set windows`geometry(errortool,.) "600x667+0+0"
  if {[tkif_IsVdmppType]} {
    set windows`geometry(dependencytool,.) "450x667+0+0"
    set windows`geometry(inhtreetool,.) "600x667+0+0"
  }
}

### set the number of digits when converting from floating point to string
set tcl_precision 17

### setting the title of the program
wm title . [ tkif_GiveToolTitle ]


###
### Bindings
###


### keep track of the size of "." all the time
bind all <Configure> {
  global windows`geometry
  set window [winfo toplevel %W]
  # puts "configure: $window"
  if {$window == "%W" && [lsearch -exact {. .interptool .errortool .modules .error_lister .editor .command .log .variable .debug_trace .breakpoint .manpage} $window] != -1} {
    set windows`geometry($window) [wm geometry $window]
    # puts "Geo [wm geometry $window ]"
  }
}

### bind help all over the application
bind all <Key-F1> {help`lookUp [tkif_getwin %X %Y]}
bind all <Key-Help> {help`lookUp [tkif_getwin %X %Y]}
# bind all <Help> {puts "%T %W %K %E" ; help`lookUp %W}

###
### Looking up tixOption in DefSchm.tcl
###

global tixOption

proc lookUpTixColour coltag {
  global tixOption
  catch {set colour $tixOption($coltag)}
  return $colour
}
