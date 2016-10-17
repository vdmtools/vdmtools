############################################################
# This function create the menu line.
# w - the path for the menu line
############################################################
proc menuline`pack {w} {
  setSN menuline`menuline $w 
  
  ### Project ###
  menubutton $w.project -text Project -menu $w.project.menu -underline 0
  pack $w.project -side left
  menu $w.project.menu -tearoff no
  menuline`insertElms $w.project menu project {new open save {save as} configure quit}
  setSN menuline`projectMenu $w.project.menu
  
  ### Tools ###
  menubutton $w.tools -text Tools -menu $w.tools.menu -underline 0
  pack $w.tools -side left
  menu $w.tools.menu -tearoff no
  if { [tkif_IsUmlType] != 0 && [tkif_IsVdmppType] != 0 } {
    menuline`insertElms $w.tools menu tools_menu_select \
      {interpreter error dependency inheritance {Rose-VDM++ Link}}
  } else { 
    if {[tkif_IsVdmppType]} {
      menuline`insertElms $w.tools menu tools_menu_select \
        {interpreter error dependency inheritance}
    } else {
      menuline`insertElms $w.tools menu tools_menu_select \
        {interpreter error}
    }  
  }
  setSN menuline`tools $w.tools
  setSN menuline`toolsmenu $w.tools.menu
  help`add $w.tools menuline`tools

  ### Options ###
  menubutton $w.options -text Options -menu $w.options.menu -underline 0
  pack $w.options -side left
  menu $w.options.menu -tearoff no
  if { [tkif_IsVdmppType] } {
    menuline`insertElms $w.options menu option_menu_select \
          {interpreter {type checker} {pretty printer} {c++ code generator} 
           {java code generator} interface}
  } else {
    menuline`insertElms $w.options menu option_menu_select \
          {interpreter {type checker} {pretty printer} {c++ code generator} 
           interface}
  }
  setSN menuline`options $w.options
  setSN menuline`optionsmenu $w.options.menu
  help`add $w.options menuline`options
  
  ### Version Control ###
  ###  menubutton $w.versionControl -text "Version Control" \
  ###    -menu $w.versionControl.menu -underline 0
  ### pack $w.versionControl -side left
  ### menu $w.versionControl.menu -tearoff no

  ### Windows ###
  ## set winmenu $w.windows.menu
  ## menubutton $w.windows -text Windows -menu $winmenu -underline 0
  ## pack $w.windows -side left
  ## menu $winmenu -tearoff no
  ## setSN menuline`winmenu $winmenu
  ## $winmenu add cascade -label Reopen -menu $winmenu.reopen
  ## menu $winmenu.reopen -tearoff no
  ## setSN menuline`reopen $winmenu.reopen
  ## menuline`insertElms $w.windows menu windows {"save layout"}
  ## 
  ## ### disableing the rest of the menus
  ## menuline`disable winmenu Reopen
  
  ### Help ###
  menubutton $w.help -text Help -menu $w.help.menu -underline 0
  pack $w.help -side right
  menu $w.help.menu -tearoff no
  menuline`insertElms $w.help menu help {about "help on help"}
}

############################################################
# insert element into the menuline
# w    - the path prefix
# func - the functions prefix
# elms - the elements to insert
############################################################
proc menuline`insertElms {path menu func elms} {
  set w $path.$menu
  foreach elm $elms {
    $w add command -label [capitalize $elm] \
	-command ${func}`[createFuncName $elm] 
    # -font {-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*}
    help`add $w.[createFuncName $elm] $func`[createFuncName $elm]
  }
  bind $w <Key-F1> "help`gotoTag $func`\[createFuncName \[%W entrycget active -label\]\];break"


  bind $path <Key-F1> {
    help`lookUp %W
  }
  
  help`add $path menuline`[createFuncName $func]
  # $path configure -font {-*-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*}
 
}


############################################################
# These thre functions changes the state of an element in
# the menuline
############################################################
proc menuline`enable {widget element} {
  menuline`setState $widget $element normal
}
proc menuline`disable {widget element} {
  menuline`setState $widget $element disabled
}
proc menuline`setState {widget element state} {
  global SN
  if {![info exists SN(menuline`$widget)]} {
    return
    runtime`internalError "\"$widget not found in name map\""
  }
  set w $SN(menuline`$widget)
  set size [expr [$w index end]+1]
  for {set i 0} {$i < $size} {incr i} {
    if {$element == [$w entrycget $i -label]} {
      break
    }
  }
  if {$i == $size} {
    return 
    runtime`internalError "\"$element\" not found in \"$widget\""
  }
  $w entryconfigure $i -state $state
}
    
proc tools_menu_select`interpreter {} {
  interptool_open
}

proc tools_menu_select`error {} {
  errortool_open
}

proc tools_menu_select`dependency {} {
  dependencytool_open
}

proc tools_menu_select`inheritance {} {
  inhtreetool_open
}

proc tools_menu_select`rose-vdm++Link {} {
   umltool_open
}

