######################################################################
# File and module handling. 
#
# $Id: modules.tcl,v 1.58 2000/09/08 07:29:06 paulm Exp $
######################################################################

set modules`select module

if {! [info exists modules_ShowHierarchys]} {
  set modules_ShowHierarchys 1
}

if {! [array exists RestoreSelections]} {
  array set RestoreSelections {}
}

############################################################
# This function initialize (create) the frame with
# the module
# w      - the path in which the elements shall be packed
# return - nothing
############################################################
proc modules`pack w {

  setSN modules`top $w
  ### create the label
  help`add $w mainPage`modules
  general`packLabel $w modules

  ### creating the module listboxes
  set tree $w.win
  tixTree $tree -options {
	hlist.separator "^"
	hlist.width 35
	hlist.height 25
        hlist.selectMode extended
        hlist.columns 8
  }
  pack $w.win -expand 1 -fill both
  set hlist [$w.win subwidget hlist]
  $hlist configure -selectmode extended
  bind $hlist <ButtonRelease-1> {
    global modules`select
    set modules`select module
    modules`selectFunctions
  }
  # Get rid of scrollbar. This should not be needed (version 4.0??). 
  $hlist delete all
  
  setSN modules`modbox $hlist
  setSN modules`tree $tree
  AddFocusWin $hlist
  
  help`add $hlist mainPage`modules`status

  
  $tree config -opencmd "DynTree:OpenDir $tree"

  # Add the root directory the TixTree widget
  ##### DynTree:AddDir $tree /
  
  # The / directory is added in the "open" mode. The user can open it
  # and then browse its subdirectories ...
    

}


proc DynTree:OpenDir {tree dir} {

    set hlist [$tree subwidget hlist]

    if {[$hlist info children $dir] != {}} {
	# We have already loaded this directory. Let's just
	# show all the child entries
	#
	# Note: since we load the directory only once, it will not be
	#       refreshed if the you add or remove files from this
	#	directory.
	#
	foreach kid [$hlist info children $dir] {
	    $hlist show entry $kid
	}
	return
    }
}


############################################################
# This function add a file to the listbox with modules
# file - the name of the file to add
# return - nothing
############################################################
proc files`addFiles files {
  foreach f $files {
    set modules [ tkif_getModules $f ]
    if {$modules == ""} {
      modules`changeState_module_file "" $f MapNoModule
    } else {
      modules`changeStates $modules
    }
  }
}

############################################################
# This function remove a file from the listbox with modules
# file - the name of the file to remove
# return - nothing
############################################################
proc files`removeFiles files {
  global SN BITMAPDIR STATEBITMAP

  foreach f $files {
    files`removeFile $f
  }  
}

proc files`removeFile {file} {
  
  global SN
  
  set hlist $SN(modules`modbox)
  set tree $SN(modules`tree)

  # get package(s) that contains file
  set packages [ tkif_getPackage_names $file ]
  
  foreach package $packages {
    set packpath  [modules_getPackagePath $package ]
    # this package may contain one or more entries containing the
    # file
    set list [ $hlist info children $packpath ]
    foreach e $list {
      set data  [ $hlist info data $e ]
      set f [ modules_extract_data_file $data ]
      if {$file == $f} {
        $hlist delete entry $e
      }
    }
    modules`removeIfEmptyPackage $hlist $packpath
  }
}

proc modules`removeIfEmptyPackage {hlist p} {
  set children [ $hlist info children $p ]
  if {$children == ""} {
    $hlist delete entry $p
  }
}

proc modules_make_package_data {package} {
  return [ list P $package ]
}

proc modules_make_module_data {package module file } {
  return [ list M $package $module $file ]
}

proc modules_extract_data_type {data} {
  return [ lindex $data 0 ]
}

proc modules_extract_data_package {data} {
  return [ lindex $data 1 ]
}

proc modules_extract_data_module {data} {
  return [ lindex $data 2 ]
}

proc modules_extract_data_file {data} {
  return [ lindex $data 3 ]
}

############################################################
# This function remove a file from the listbox with modules
# module - the name of the module to remove
# return - nothing
############################################################

proc modules`removeModules modules {
  global SN BITMAPDIR STATEBITMAP
  
  set hlist $SN(modules`modbox)
  set tree $SN(modules`tree)

  if { [ llength $modules ] == 0 } {
    return
  }
  
  set children [ $hlist info children ]
  foreach c $children {
    set package [ modules_extract_data_package [ $hlist info data $c ] ]
    foreach module $modules {
      set modpath  [modules_getPackageModulePath $package $module ]
      if {[$hlist info exists $modpath]} {
        $hlist delete entry $modpath
        modules`removeIfEmptyPackage $hlist $c
      }
    }
  }
  modules`selectFunctions
}

############################################################
# This function add a module to the listbox with modules
# module - the name of the module to add
# return - nothing
############################################################
proc modules`addModules {modules} {
  # This functions also handles new modules, lucky us!
  modules`changeStates $modules
}

proc modules`selected_modules {} {
  global SN modules`select
  set hlist $SN(modules`modbox)

  set sel [$hlist info selection]
  set res {}
  foreach s $sel {
    set data  [ $hlist info data $s ]
    if {[ modules_extract_data_type $data ] == "M" } {
      lappend res [ modules_extract_data_module $data ]
    }
  }
  return $res
}

proc modules`selected_files {} {
  global SN modules`select
  set hlist $SN(modules`modbox)

  set sel [$hlist info selection]
  set res {}
  foreach s $sel {
    set data  [ $hlist info data $s ]
    if {[ modules_extract_data_type $data ] == "M" } {
      lappend res [ modules_extract_data_file $data ]
    }
  }
  return $res
}


############################################################
# This function return the names of the modules which
# is selected in the listbox
############################################################

proc modules`selectedModules {} {
  global SN modules`select
  set modbox $SN(modules`modbox)
###  set result {}

  if {[tkif_IsVdmslType] || [tkif_IsVdmslLiteType]} {
    return [ modules`selected_modules ]
  } else {  ### VDM++
    if {${modules`select} == "module"} {
      return [ modules`selected_modules ]
    } elseif {${modules`select} == "super_classes"} {
      set modbox $SN(modules`superclassbox)
      set result [ modules`get_selection $modbox ]
      return $result
    } elseif {${modules`select} == "sub_classes"} {
      set modbox $SN(modules`subclassbox)
      set result [ modules`get_selection $modbox ]
      return $result
    } elseif {${modules`select} == "uses"} {
      set modbox $SN(modules`usesbox)
      set result [ modules`get_selection $modbox ]
      return $result
    } elseif {${modules`select} == "usedby"} {
      set modbox $SN(modules`usedbybox)
      set result [ modules`get_selection $modbox ]
      return $result
    } elseif {${modules`select} == "inhtree"} {
      set modbox $SN(modules`inhtreebox)
      set result [ inhtree`get_selection $modbox ]
      return $result
    } else {
      # FIXME do we ever get here? Don't think so /etn 2000-03-02
      return [ modules`selected_modules ]
    }
  }
}

proc modules`get_selection {modbox} {
  set selectedmods {}
  set numselect [$modbox curselection]
  foreach module $numselect {
    lappend selectedmods [$modbox get $module]
  }
  return $selectedmods
}

############################################################
# This function return the names of the files which
# is selected in the listbox
############################################################
proc modules`selectedFiles {} {
  global SN modules`select
  set modbox $SN(modules`modbox)

  
  set files [modules`selected_files]
  set result ""
  foreach file $files {
    uniqListAppend result $file
  }
  return $result
}

############################################################
# Callback function for changing state of file
############################################################

# callback function
proc files`changeState {file} {
  # puts "file`changeState $file"
  foreach m [ tkif_getModules $file ] {
    modules`changeState_module_file $m $file MapNoModule
  }
  return
}

############################################################
# This function change the state of a module
# in it's listbox (syntaxchecked,typechecked, etc.)
############################################################

# callback function
proc modules`changeStates {modules} {
  # puts "changestates = $modules"
  foreach m $modules {
    set files [ tkif_getFilesOfModule $m ]
    foreach f $files {
      modules`changeState_module_file $m $f MapNoModule
    }
  }
}

proc tkif_getPackage_names { file} {
  # Remove file name
  set dir  [ tkif_GetBasedir $file  ]

  # if dir is x:\ use it as the package name
  if { [string length $dir] == 3 &&
       [string range $dir 1 end] == ":\\" } {
    set package $dir
  } else {
    set package [ tkif_GetBasename $dir  ]
  }
  if {$package == "."} {
    set package "DEFAULT"
  }
  # We use ^ has as the hlist.separator in the tixTree so this char
  # cannot be part of a package name so replace it with '_' 
  regsub "\\^" $package "_" result
  # we are supposed to return a list 
  return [ list $result ]
}

# modules`ChangeAllStates calls modules`changeState for each module
# modules`changeState (m) calls modules`changeState_module_file (m "")
# modules`changeState_module_file (m f)
#   find packages and calls modules`changePackageState
#   for each package
# files`addFiles{files} calls files`changeState for each file
# files`changeState calls modules`changeState_module_file for each module in file

proc modules_getPackageModulePath {p m} {
  global modules_ShowHierarchys
  if {$modules_ShowHierarchys} {
    return "$p^$m"
  } else {
    return "$m"
  }
}

proc modules_getPackagePath {p} {
  # ^ is the hlist.separator
  global modules_ShowHierarchys
  if {$modules_ShowHierarchys} {
    return "$p"
  } else {
    return ""
  }
}
proc modules`clear_RestoreSelections {} {
  global RestoreSelections
  if {[array exists RestoreSelections]} {
    unset RestoreSelections
  }
}
proc modules`toggleHierarchy {button} {
  global SN
  global modules_ShowHierarchys
  set modules_ShowHierarchys [expr ! $modules_ShowHierarchys ]
  if {$modules_ShowHierarchys} {
    $button configure -image imbm`hier_off
  } else {
    $button configure -image imbm`hier_on
  }
  # RestoreSelections is an array used by modules`
  global RestoreSelections
  set hlist $SN(modules`modbox)
  set modules [ modules`selected_modules ]
  foreach module $modules {
    set RestoreSelections($module) 1
  }

  $hlist delete all

  if {$modules_ShowHierarchys} {
    $hlist configure -indicator true -drawbranch true
  } else {
    $hlist configure -indicator false -drawbranch false
  }
  files`addFiles [ tkif_getFiles ]
  modules`clear_RestoreSelections
}

proc modules`makePackageRoot {hlist package packpath} {
  if {[ $hlist info exists $packpath ]} {
    return;
  }

  global SN modules_ShowHierarchys

  set tree $SN(modules`tree)
  
  # find where to insert $packpath
  set atcount 0
  set names [$hlist info children]
  foreach n $names {
    if {[string compare $packpath $n] == -1} {
      break
    }
    incr atcount
  }
  set e [ $hlist add $packpath -at $atcount ]
  if {$modules_ShowHierarchys} {
    $hlist entryconfigure $e -itemtype imagetext \
        -text $package -image [tix getimage folder]
  }
  $hlist entryconfigure $e -data [ modules_make_package_data $package ]
  $tree setmode $packpath open
  $tree open $packpath
}


proc modules`changePackageState {hlist package module file selmap} {
  global SN

  set w    $SN(modules`modbox)
  set tree $SN(modules`tree)

  set state [ tkif_getStatus $module ]
  if {$module == ""} {
    set state [ list error none none none none]
  }
  set syntax   [lindex $state 0]
  set type      [lindex $state 1]
  set generated [lindex $state 2]
  if {[tkif_IsVdmppType]} {
    set generatedJava [lindex $state 3]
  }
  set printed   [lindex $state 4]
  set i 1

  ### Verify if the files has changed.
  if {[tkif_IsFileChanged $file]} {
    set syntax "$syntax-changed"
    set state [ lreplace $state 0 0 $syntax ]
  }

  upvar $selmap MapNoModule
  
  set data_module $module
  set display_module $module
  set syntax_error_module "*[tkif_GetBasename $file]*"
  if {$module == ""} {
    set display_module $syntax_error_module
    set data_module ""
  }

  set modpath   [modules_getPackageModulePath $package $display_module ]
  set packpath  [modules_getPackagePath $package ]
  set nomodpath [modules_getPackageModulePath $package $syntax_error_module ]

  # Perhaps there is an entry for a file with no known module/class
  # If so then delete it.
  if {$module != ""} {
    if {[ $hlist info exists $nomodpath ]} {
      set MapNoModule($nomodpath) 1
      $hlist delete entry $nomodpath
    }
  }

  # Create the package package entry
  modules`makePackageRoot $hlist $package $packpath 

  # create the module entry
  if {![$hlist info exists $modpath]} {
    set names [$hlist info children $packpath ]
    # all names are returned with a prefix of $packpath/ but since
    # they all have it it does not change the comparison.
    set atcount 0
    foreach name $names {      
      if {[string compare $modpath $name] == -1} {
        break
      }
      incr atcount
    }
    $hlist add $modpath -at $atcount \
        -data [ modules_make_module_data $package $display_module $file ]

    set showfile [ tkif_GetBasename $file ]
    $hlist item create $modpath 0 -itemtype text -text $display_module
    $hlist item create $modpath 6 -itemtype text -text $showfile
    set new 1
  } else {
    set new 0
  }

  global RestoreSelections
  if {[info exists RestoreSelections($display_module)]} {
    $hlist selection set $modpath
  }
  
  set i 1
  global STATEBITMAP
  if {[tkif_IsVdmppType]} {
      set states [list syntax type generated generatedJava printed]
  } else {
      set states [list syntax type generated printed]
  }
  foreach st $states {
    if {$new} {
      $hlist item create $modpath $i -itemtype imagetext \
          -bitmap bm`$STATEBITMAP($st,[set $st])
    } else {
      $hlist item configure $modpath $i -itemtype imagetext \
          -bitmap bm`$STATEBITMAP($st,[set $st])
    }
    incr i
  }

  # if a file with an empty module has existed in this round of updates
  # the set the selection of the same file with with a real module
  if {[info exists MapNoModule($nomodpath)]} {
    $hlist selection set $modpath
  }
}

# modules`changeState_module_file is called with a third argument which
# is an array that saves the names of entries which has no module part
# but only a file part (this is all files that have never been parsed or
# never been parsed with success).

proc modules`changeState_module_file {module file mapname_} {
  global SN BITMAPDIR 

  upvar $mapname_ M2
  set w $SN(modules`modbox)
  set hlist $w
  
  if {$file == ""} {
    set file [ tkif_getFilesOfModule $module ]
  }
  
  # get package name for $module
  set packages [ tkif_getPackage_names $file ]
  # if packages does not exist

  foreach p $packages {
    modules`changePackageState $hlist $p $module $file M2
  }
###  modules`selectFunctions
}

proc modules`changeState_new {module} {
  global SN BITMAPDIR STATEBITMAP

  set w $SN(modules`modbox)
  set state [ tkif_getStatus $module ]

  if {![$w info exists $module]} {
    set names [lsort [$w info children]]
    set found 0
    foreach name $names {
      if {[string compare $module $name] == -1} {
        set found 1
        $w add $module -before $name
        break
      }
    }
    if {!$found} {
      $w add $module
    }
    set new 1
  } else {
    set new 0
  }
  set syntax   [lindex $state 0]
  set type      [lindex $state 1]
  set generated [lindex $state 2]
  if {[tkif_IsVdmppType]} {
    set generatedJava [lindex $state 3]
  }
  set printed   [lindex $state 4]
  set i 0

  ### Verify if the files has changed.
  set files [tkif_getFilesOfModule $module]
  foreach file $files {
    if {[tkif_IsFileChanged $file]} {
      set syntax "$syntax-changed"
    }
  }
  

  if {[tkif_IsVdmppType]} {
      set states [list syntax type generated generatedJava printed]
  } else {
      set states [list syntax type generated printed]
  }

  foreach st $states {
    if {$new} {
      $w item create $module $i -itemtype imagetext \
	  -bitmap bm`$STATEBITMAP($st,[set $st])
    } else {
      $w item configure $module $i -itemtype imagetext \
	  -bitmap bm`$STATEBITMAP($st,[set $st])
    }
    incr i
  }
  if {$new} {
    $w item create $module 4 -itemtype text -text $module
  }
###  modules`selectFunctions
}

############################################################
# This function is called everytime the windows get focus
# so the status of files (is they changed on disk)
# is updated.
############################################################

# timer callback
proc modules`ChangeAllStates {} {
#  modules`changeStates [tkif_getModules]
  foreach file [tkif_getFiles] {
    if {[tkif_IsFileChanged $file]} {
      files`changeState $file
    }
  }
}


############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc modules`save {} {
  global modules`mods modules`files SN modules`buttonState
  set modules`mods [modules`selectedModules]
  set modules`buttonState [$SN(modules`editButton) cget -state]
}
proc modules`load {} {
  global SN modules`mods modules`files modules`buttonState

  set modbox $SN(modules`modbox)
  foreach mod ${modules`mods} {
    $modbox selection set $mod
  }
  $SN(modules`editButton) configure -state [set modules`buttonState]
  $SN(files`editButton) configure -state [set modules`buttonState]
}
proc files`save {} {
  modules`save
}
proc files`load {} {
  modules`load
}

proc modules`set_module_selection {modules reset} {
  global RestoreSelections SN
  modules`clear_RestoreSelections
  if {$reset} {
    $SN(modules`modbox) selection clear
  }
  foreach module $modules {
    set RestoreSelections($module) 1
  }
  modules`changeStates $modules
  modules`clear_RestoreSelections
}

############################################################
# This function is called when files or modeules had been
# selected in the moudule/files window
# Its purpose is to enable/disable the correct buttons
# and menu entries depending on the modules state
# return - nothing
############################################################
proc modules`selectFunctions {} {
  global SN modules`select MODE
  set mods  [modules`selectedModules]
  if {$mods == ""} {
    if {[modules`selectedFiles] == ""} {
      set selectState disabled
    } else {
      set selectState normal
    }
    set syntaxState disabled
    set typeState disabled
    set codegenState disabled
    if {[tkif_IsVdmppType]} {
      set javaCodegenState disabled
    }
    set prettyprintState disabled
  } else {
    set syntaxState normal
    set typeState normal
    set codegenState normal
    if {[tkif_IsVdmppType]} {
      set javaCodegenState normal
    }
    set prettyprintState normal
    set selectState normal
  }
  
  foreach module $mods {
    if {! [tkif_allowedTypecheck $module]} {
      set typeState disabled
    }
    if {! [tkif_allowedCodegen $module]} {
      set codegenState disabled
      if {[tkif_IsVdmppType]} {      
        set javaCodegenState disabled
      }
    }
  }

  foreach file [modules`selectedFiles] {
    if {! [tkif_allowedSyntax $file ] } {
      set syntaxState disabled
    }
    if {! [tkif_allowedPrettyprint $file ] } {
      set prettyprintState disabled
    }
  }


  ### edit
  functions`setWindowButtonState modules edit $selectState

  ### preview
  functions`setState exportmenu preview $selectState

  ### syntax check
  functions`setState mainsub "syntax check" $selectState 

  ### print
  functions`setState exportmenu "print" $selectState

  ### typeCheck
  functions`setState mainsub "type check" $typeState

  ### preview
  functions`setState exportmenu preview $syntaxState

  ### pretty print
  functions`setState exportmenu "pretty print" $prettyprintState

  ### code generate
  functions`setState exportmenu "generate c++" $codegenState

  ### code generate
  if {[tkif_IsVdmppType]} {
    functions`setState exportmenu "generate java" $javaCodegenState
  }

  ###
  ### hilight the other listbox (modules / files)
  ###
  if {[tkif_IsVdmslType] || [tkif_IsVdmslLiteType]} {
    # Nothing to do
  }
  if {[tkif_IsVdmppType]} {
    if {${modules`select} == "module"} {
      set inhtreebox $SN(modules`inhtreebox)
      
      inhtree`clearSelection $inhtreebox
      foreach module [modules`selectedModules] {
        inhtree`markSelectedModule $inhtreebox $module
      }
    } elseif {${modules`select} == "super_classes" ||
      ${modules`select} == "sub_classes" ||
      ${modules`select} == "uses" ||
      ${modules`select} == "usedby"} {
        set modbox $SN(modules`modbox)
        set inhtreebox $SN(modules`inhtreebox)
        if {${modules`select} == "super_classes"} {
          set dependencymodbox $SN(modules`superclassbox)
        } elseif {${modules`select} == "sub_classes"} {
          set dependencymodbox $SN(modules`subclassbox)
        } elseif {${modules`select} == "uses"} {
          set dependencymodbox $SN(modules`usesbox)
        } elseif {${modules`select} == "usedby"} {
          set dependencymodbox $SN(modules`usedbybox)
        } 
        $modbox selection clear
        $modbox anchor clear
        inhtree`clearSelection $inhtreebox

        set modules [modules`selectedModules]
        modules`set_module_selection  $modules true
        foreach module $modules {
          inhtree`markSelectedModule $inhtreebox $module
        }
        ### Selections disappear from the four dependency windows on
        ### selection, therefore modules`select is set to module.
        set modules`select "module"
    } elseif {${modules`select} == "inhtree"} {
      set modbox $SN(modules`modbox)
      set inhtreebox $SN(modules`inhtreebox)
      $modbox selection clear
      $modbox anchor clear
      
      set selmods [modules`selectedModules]
      inhtree`clearSelection $inhtreebox
      
      modules`set_module_selection $selmods true
      foreach module $selmods {
        inhtree`markSelectedModule $inhtreebox $module
      }
    } else {
      runtime`internalError "No window ${modules`select} exists"
    }
    
    if [IsDependOpen] {
      set selnames [modules`selectedModules]  
      if {[llength $selnames] == 1} {
        tkif_updateDepends $selnames
      } elseif {[llength $selnames] == 0} {
        dependency`reset
        dependencynamemess`reset
      } else {
        dependency`reset
        dependencynamemess`toomany
      }
    }
  }
}


############################################################
# this function start an external editor with the files
# selected.
# return - nothing
############################################################
proc modules`edit {} {
  global options`editor
  set files [modules`selectedFiles]
  general`startEditor $files
}

proc files`edit {} {
  modules`edit
}

############################################################
# This function indicates in the files / modules lisyboxes
# which files / modules has changed on disk.
# return - nothing
############################################################
