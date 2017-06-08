######################################################################
# Handle options.
#
# $Id: options.tcl,v 1.38 2000/10/30 17:30:21 paulm Exp $
######################################################################


proc options`getvalues {} {
  global options_vdm_setting

  set options_vdm_setting(dtc) 2
  
  tkif_Options get options_vdm_setting
  
#  foreach opt {
#    dtc inv pre post
#    pos def extended
#    prettyprint_index_defs prettyprint_index_defs_uses prettyprint_rti
#    cg_posinfo
#    sep
#    tex
#    ppr 
#    
#  }
}

proc options`open {page} {

  ###
  ### copying informations to the widgets
  ###

  if {![winfo exists .options ] || ([wm state .options] == "withdrawn")} {
    # Only copy the current options to temporary options`_*
    # when the window must be created a new or when it has been
    # withdrawn from the options`exit procedure.
    # Otherwise it is visible or iconified, and then the shown option
    # values should not be modified.

    # log`add "Reading options\n" 
    
    options`getvalues
    
    foreach group {editor misc vdm} {
      global options`$group options`_$group
      array set options`_${group} [ array get options`${group} ]
    }
  }
  
  if {[winfo exists .options]} {
    wm deiconify .options
    .options.book raise $page
    raise .options
    return
  }


  set w .options
  setSN options`top $w
  toplevel $w

  set book $w.book
  tixNoteBook $book 
  pack $book -fill both -expand 1
  
  set interp [$book add interpreter -label Interpreter]
  $book pageconfigure interpreter -createcmd "options`createInterpreter $interp"

  set typechecker [$book add typechecker -label "Type checker"]
  $book pageconfigure typechecker -createcmd "options`createTypeChecker $typechecker"

  set prettyprinter [$book add prettyprinter -label "Pretty printer" ]
  $book pageconfigure prettyprinter -createcmd "options`createPrettyPrinter $prettyprinter"

  set codegenerator [$book add codegenerator -label "C++ Code generator"]
  $book pageconfigure codegenerator -createcmd "options`createCodeGenerator $codegenerator"

  if { [tkif_IsVdmppType] } {
    set javaCodegenerator [$book add javaCodegenerator -label "Java code generator"]
    $book pageconfigure javaCodegenerator \
      -createcmd "options`createJavaCodeGenerator $javaCodegenerator"
  }

  set interface [$book add interface -label Interface]
  $book pageconfigure interface -createcmd "options`createInterface $interface"

  ### The ok, cancel and save buttons
  pack [tixButtonBox $w.but] -fill x -expand 1
  $w.but add ok -text "Ok" -command "options`exit ok"
  $w.but add cancel -text "Cancel" -command "options`exit cancel"
  # $w.but add save -text "Save" -command "options`exit ok;options`save"

  ### Select the right page.
  $book raise $page
}

proc options`createInterface w {
  ###
  ### The editor options
  ###
  tixLabelFrame $w.editor -label "External Editor" -labelside acrosstop
  pack $w.editor -fill x  -anchor n
  set editor [$w.editor subwidget frame]

  ### name
  tixLabelEntry $editor.name -label "Name: " -options {
    entry.textVariable options`_editor(name)
  }
  pack $editor.name -expand 1 -fill x -padx 5

  ### prefix
  ###tixLabelEntry $editor.prefix -label "Prefix: " -options {
  ###  entry.textVariable options`_editor(prefix)
  ###}
  ###pack $editor.prefix -expand 1 -fill x  -padx 5

  ### fileSpec for multiple files
  tixLabelEntry $editor.fileSpec -label "Format to load multiple files: " \
      -options {
	entry.textVariable options`_editor(fileSpec)
      }
  pack $editor.fileSpec -expand 1 -fill x  -padx 5

  ### fileSpec for single file with line number
  tixLabelEntry $editor.fileSpecWithLineNo \
      -label "Format to load single file with line pos: " -options {
	entry.textVariable options`_editor(fileSpecWithLineNo)
      }
  pack $editor.fileSpecWithLineNo -expand 1 -fill x  -padx 5

  ### postfix
  ###tixLabelEntry $editor.postfix \
  ###   -label "Postfix: " -options {
  ###	entry.textVariable options`_editor(postfix)
  ###  }
  ###pack $editor.postfix -expand 1 -fill x  -padx 5

  ### multiple files in one editor
  checkbutton $editor.multipleFiles -variable options`_editor(multipleFiles) \
      -text "The editor support multiple files in one editor"
  pack $editor.multipleFiles -anchor w
  
  ### additional help
  label $editor.help -text "In format to load multiple files \"%f\" expands to the filename, and in format to load single file with line pos, \"%l\" expands to the line number" -wraplength 10c -justify left
  pack $editor.help -anchor w -padx 5

  ###
  ###
  ### misc options
  ###
  ###
  tixLabelFrame $w.misc -label "Miscellaneous" -labelside acrosstop 
  pack $w.misc -fill x -anchor n
  set misc [$w.misc subwidget frame]
  
  checkbutton $misc.focus -text "Give a window focus when the mouse enter" \
      -variable options`_misc(giveFocus) -anchor w
  tixLabelEntry $misc.print -label "Print command" -options {
    entry.textVariable options`_misc(print)
  }
  
  pack $misc.focus $misc.print  \
      -anchor w -padx 5 -fill x  -expand 1

  ### additional help
  label $misc.help -wraplength 10c -justify left -text "In the command, the following substitutions occur: %f - the name of the frame, or the name of the file (for editor window) %d - the current date. %t - the current time."
  pack $misc.help -anchor w -padx 5
}

proc options`createInterpreter w {
  tixLabelFrame $w.runtimecheck -label "Interpreter" -labelside acrosstop
  pack $w.runtimecheck -anchor n -fill x -expand 1
  set runtimecheck [$w.runtimecheck subwidget frame]

  ### the check frame
  checkbutton $runtimecheck.dtc -text "Enable dynamic type check" \
      -variable options_vdm_setting(dtc)
  checkbutton $runtimecheck.inv -text "Enable dynamic checks of invariants" \
      -variable options_vdm_setting(inv) -command {
    if { [set options_vdm_setting(inv)] != 0} {set options_vdm_setting(dtc) 1}
  }
  checkbutton $runtimecheck.pre -text "Enable check of pre-conditions" \
      -variable options_vdm_setting(pre)
  checkbutton $runtimecheck.post -text "Enable check of post-conditions" \
      -variable options_vdm_setting(post)
  checkbutton $runtimecheck.ppr -text "Enable pretty printing of values" \
      -variable options_vdm_setting(ppr)
    tixControl $runtimecheck.seed \
        -label "Value to initialize the random generator with (-1: non random) " \
        -integer true -min -1 -variable options_vdm_setting(seed)

    pack $runtimecheck.dtc $runtimecheck.inv $runtimecheck.pre \
        $runtimecheck.post $runtimecheck.ppr $runtimecheck.seed \
        -anchor w

  if { [tkif_IsVdmppType] } {
    checkbutton $runtimecheck.priority \
      -text "Enable priority-based scheduling" \
      -variable options_vdm_setting(priority)

    tixControl $runtimecheck.maxinstr \
        -label "Maximum instructions per time slice " \
        -integer true -min 1 -variable options_vdm_setting(maxinstr)

    
    set alg_options {pc ins}
    set alg_labels(pc) "Pure Cooperative"
    set alg_labels(ins) "Instruction Number Slicing"
    tixOptionMenu $runtimecheck.algorithm \
      -label "Primary Scheduling Algorithm " \
      -variable options_vdm_setting(primaryalgorithm)
    foreach opt $alg_options {
      $runtimecheck.algorithm add command $opt -label $alg_labels($opt)
    }
    pack $runtimecheck.priority -anchor w
    pack $runtimecheck.maxinstr -anchor w
    pack $runtimecheck.algorithm -anchor w
  }

}

proc options`createTypeChecker w {
  tixLabelFrame $w.typecheck -label "Type checker" -labelside acrosstop
  pack $w.typecheck -anchor n -fill x -expand 1
  set typecheck [$w.typecheck subwidget frame]

  checkbutton $typecheck.pos -text "Enable \"pos\" type check" \
      -variable options_vdm_setting(pos) -command {
    set options_vdm_setting(def) [expr ! $options_vdm_setting(pos)]
  }
  checkbutton $typecheck.def -text "Enable \"def\" type check" \
      -variable options_vdm_setting(def) -command {
    set options_vdm_setting(pos)  [expr ! $options_vdm_setting(def)] 
  }
  checkbutton $typecheck.extended -text "Enable extended type check" \
      -variable options_vdm_setting(full)

  checkbutton $typecheck.sep -text "Enable warning/error message separation" \
      -variable options_vdm_setting(sep)

  pack $typecheck.pos $typecheck.def $typecheck.extended $typecheck.sep\
      -anchor w
  if {[tkif_IsVdmslLiteType]} {
    $typecheck.pos configure -state disabled
    $typecheck.def configure -state disabled
  }
}

proc options`createPrettyPrinter w {
  tixLabelFrame $w.prettyprint -label "Pretty printer" -labelside acrosstop
  pack $w.prettyprint -anchor n -fill x -expand 1
  set prettyprint [$w.prettyprint subwidget frame]

  checkbutton $prettyprint.def -text "Output index of definitions" \
      -variable options_vdm_setting(index_defs) -command {
    if { [set options_vdm_setting(index_defs)] } {
      set options_vdm_setting(index_uses) 0
    }
  }
    
  checkbutton $prettyprint.uses -text "Output index of definitions and uses" \
      -variable options_vdm_setting(index_uses) -command {
    if { [set options_vdm_setting(index_uses)] } {
      set options_vdm_setting(index_defs) 0
    }
  }
    
  checkbutton $prettyprint.tci -text "Enable test coverage coloring" \
      -variable options_vdm_setting(prettyprint_rti)
  
  pack $prettyprint.def $prettyprint.uses $prettyprint.tci \
      -anchor w
}

proc options`createCodeGenerator w {
  tixLabelFrame $w.codegenerator -label "Code generator" -labelside acrosstop
  pack $w.codegenerator -anchor n -fill x -expand 1
  set codegenerator [$w.codegenerator subwidget frame]

  checkbutton $codegenerator.posinfo -text "Output position information" \
      -variable options_vdm_setting(cg_rti)

  pack $codegenerator.posinfo -anchor w

  if { [tkif_IsVdmppType] } {
    checkbutton $codegenerator.checkprepost -text \
        "Check pre and post conditions" \
        -variable options_vdm_setting(cpp_checkprepost) -command {
          if { ![set options_vdm_setting(cpp_checkprepost)] } {
            set options_vdm_setting(cpp_checkprepost) 0
         }
       }

     pack $codegenerator.checkprepost -anchor w
   }


}

proc options`createJavaCodeGenerator w {

  tixLabelFrame $w.javaCodegenerator -label "Java Code generator" -labelside acrosstop
  pack $w.javaCodegenerator -anchor n -fill x -expand 1
  set javaCodegenerator [$w.javaCodegenerator subwidget frame]

  checkbutton $javaCodegenerator.skeletons -text \
      "Code generate only skeletons, except for types" \
      -variable options_vdm_setting(jcg_skel) \
      -command {
#       if { ![set options_vdm_setting(jcg_skel)] } {
#         set options_vdm_setting(jcg_smalltypes) 0
#       }   
       }
  pack $javaCodegenerator.skeletons -anchor w

  checkbutton $javaCodegenerator.types -text \
      "Code generate only types" \
      -variable options_vdm_setting(jcg_types)
  pack $javaCodegenerator.types -anchor w

#  checkbutton $javaCodegenerator.smalltypes -text\
#      "Code generate code with small types" \
#      -variable options_vdm_setting(jcg_smalltypes) \
#      -command {
#       if { [set options_vdm_setting(jcg_smalltypes)] } {
#         set options_vdm_setting(jcg_skel) 1
#      }}
#  pack $javaCodegenerator.smalltypes -anchor w

  checkbutton $javaCodegenerator.longs -text \
      "Code generate integers as Longs" \
      -variable options_vdm_setting(jcg_longs)
  pack $javaCodegenerator.longs -anchor w
  
  checkbutton $javaCodegenerator.concur -text \
      "Code generate code with concurrency constructs" \
      -variable options_vdm_setting(jcg_concur)
  pack $javaCodegenerator.concur -anchor w

  checkbutton $javaCodegenerator.genprepost -text \
      "Code generate pre and post functions/operations" \
      -variable options_vdm_setting(jcg_genprepost) -command {
        if { ![set options_vdm_setting(jcg_genprepost)] } {
           set options_vdm_setting(jcg_checkprepost) 0
        }
      }

  pack $javaCodegenerator.genprepost -anchor w

  checkbutton $javaCodegenerator.checkprepost -text \
      "Check pre and post conditions" \
      -variable options_vdm_setting(jcg_checkprepost) \
      -command {
       if { [set options_vdm_setting(jcg_checkprepost)] } {
         set options_vdm_setting(jcg_genprepost) 1
      }}

  pack $javaCodegenerator.checkprepost -anchor w

  ### Package
  tixLabelEntry $javaCodegenerator.package -label "Package: " -options {
    entry.textVariable options_vdm_setting(jcg_package)
  }
  pack $javaCodegenerator.package -expand 1 -fill x -padx 5

  button $javaCodegenerator.but -text "Select Interfaces" \
                                -command "options`createJavaInterfaces"
  pack $javaCodegenerator.but -side left

}

proc options`createJavaInterfaces {} {
    global possibleInterfaces
    global selectedInterfaces
    global options_vdm_setting

    if {![winfo exist .javaInterfaces]} {
      set w [toplevel .javaInterfaces]
      wm protocol $w WM_DELETE_WINDOW\
        { wm withdraw .javaInterfaces; puts "withd" }
      wm title $w "Select Java Interfaces"

    frame $w.top -border 1 -relief raised

    tixLabelFrame $w.top.a -label "Possible Interfaces" -labelside acrosstop \
                           -options {label.padX 5 }
    tixLabelFrame $w.top.b -label "Action" -labelside acrosstop \
                           -options { label.padX 5 }
    tixLabelFrame $w.top.c -label "Selected" -labelside acrosstop \
                           -options { label.padX 5 }

    set possibleInterfaces [$w.top.a subwidget frame]
    set selectedInterfaces [$w.top.c subwidget frame]
    set actions [$w.top.b subwidget frame]

    tixButtonBox $actions.a -orientation vertical 
    $actions.a add addbutton -text "Add" -command options`addInterface 
    $actions.a add removebutton -text "Remove" -command options`removeInterface
    pack $actions.a -expand yes -fill both \
                                -padx 10 -pady 10 -side left 

    pack $w.top.a $w.top.b $w.top.c -side left -expand yes -fill both


    tixScrolledListBox $possibleInterfaces.a -scrollbar auto -options {
      listbox.selectMode extended
    }
    pack $possibleInterfaces.a -expand yes -fill both \
                               -padx 10 -pady 10 -side left

    tixScrolledListBox $selectedInterfaces.a -scrollbar auto -options {
      listbox.selectMode extended
    }
    pack $selectedInterfaces.a -expand yes -fill both \
                               -padx 10 -pady 10 -side left

    $possibleInterfaces.a subwidget listbox config -cursor left_ptr
    raise [$possibleInterfaces.a subwidget listbox ]

    options`updateInterfaces

    tixButtonBox $w.box -orientation horizontal
    $w.box add ok -text Ok -width 6 \
                  -command "options`exitInterfaces ok $w"
	
    $w.box add cancel -text Cancel -width 6\
                      -command "options`exitInterfaces cancel $w"

    pack $w.box -side bottom -fill x
    pack $w.top -side top -fill both -expand yes
  } else {
    options`updateInterfaces
    wm deiconify .javaInterfaces
    raise .javaInterfaces
    focus .javaInterfaces
  }
}

proc options`updateInterfaces {} {
    global possibleInterfaces
    global selectedInterfaces
    global options_vdm_setting

    # Clean old file lists  
    $possibleInterfaces.a subwidget listbox delete 0 end
    $selectedInterfaces.a subwidget listbox delete 0 end

    set initInterfaces $options_vdm_setting(jcg_interfaces)
    foreach i $initInterfaces {
        $selectedInterfaces.a subwidget listbox insert end $i
    }

    set allClasses [tkif_GetPossibleInterfaces]
    foreach i $allClasses {
        $possibleInterfaces.a subwidget listbox insert end $i
    }

}

proc options`resetInterfaces {} {
    global options_vdm_setting
    tkif_resetInterfaces
    if {[winfo exists .javaInterfaces]} {
      if {[string compare [wm state .javaInterfaces] normal] == 0} {
          options`updateInterfaces 
      }
    }
}


proc options'updatePossibleInterfaces {} {
    global possibleInterfaces
    global selectedInterfaces

    $possibleInterfaces.a subwidget listbox delete 0 end
    set nowSelected [$selectedInterfaces.a subwidget listbox get 0 end]
    set allClasses [tkif_GetActivatedInterfaces $nowSelected ]
    foreach i $allClasses {
        $possibleInterfaces.a subwidget listbox insert end $i
    }
}

proc options`addInterface {} {
    global possibleInterfaces
    global selectedInterfaces
    global options_vdm_setting
    set lb [$possibleInterfaces.a subwidget listbox]
    set sel  [$lb curselection]
    set existingSelected [$selectedInterfaces.a subwidget listbox get 0 end]
    foreach i $sel {
        set item [$lb get $i]
        set ix [lsearch -exact $existingSelected $item]
        if {$ix < 0} {
          $selectedInterfaces.a subwidget listbox insert end $item
        }
    }
    options'updatePossibleInterfaces
}

proc options`removeInterface {} {
    global possibleInterfaces
    global selectedInterfaces

    set lb [$selectedInterfaces.a subwidget listbox]
    set sel  [$lb curselection]
    set orderedCurSelection [lsort -integer -decreasing $sel]
    foreach i $orderedCurSelection {
        $selectedInterfaces.a subwidget listbox delete $i
    }

    options'updatePossibleInterfaces
    set lb [$selectedInterfaces.a subwidget listbox]
    set sel  [$lb get 0 end]
    set orderedWholeList [lsort -integer -decreasing $sel]
    set allowed [$possibleInterfaces.a subwidget listbox get 0 end]
    for {set i [$lb index end]} {$i >= 0} {incr i -1} {
        puts $i
        set item [$lb get $i]
        set ix [lsearch -exact $allowed $item]
        if {$ix < 0} {
          $selectedInterfaces.a subwidget listbox delete $i
        }
    }
    
}


proc options`exitInterfaces {type w} {
  global selectedInterfaces
  global options_vdm_setting
  wm withdraw $w
  
  if {$type == "ok"} {
    set lb [$selectedInterfaces.a subwidget listbox]
    set options_vdm_setting(jcg_interfaces) [$lb get 0 end]

  } else { # type == "cancel"
    ### restoring the buttons listboxes
    options`hilightButtons
  }
}

proc option_menu_select`interpreter {} {
  options`open "interpreter"
}
proc option_menu_select`typeChecker {} {
  options`open "typechecker"
}
proc option_menu_select`prettyPrinter {} {
  options`open "prettyprinter"
}
proc option_menu_select`c++CodeGenerator {} {
  options`open "codegenerator"
}

proc option_menu_select`javaCodeGenerator {} {
  options`open "javaCodegenerator"
}

proc option_menu_select`interface {} {
  options`open "interface"
}
###
### Creating the command index
###

### common commands
foreach command {
  {"open project" project`open}
  {"new project"  project`new}
  {"configure project" project`configure}
  {quit project`quit}
  {preview export`preview}
  {print export`print}
  {"reopen window" windows`reopen}
  {"save layout" windows`saveLayout}
  {"vdm" option_menu_select`vdm}
  {"interface" option_menu_select`interface}
} {
  set text [lindex $command 0]
  set cmd [lindex $command 1]
  set options`name2cmd(maintool,$text) $cmd
  set options`name2cmd(interptool,$text) $cmd
}
### commands for main mode
foreach command {
  {"syntax check" functions`syntaxCheck}
  {"type check" functions`typeCheck}
  {"generate c++" functions`generateC++}
  {"generate java" functions`generateJava}
  {"pretty print" functions`prettyPrint}
} {
  set text [lindex $command 0]
  set cmd [lindex $command 1]
  set options`name2cmd(maintool,$text) $cmd
}
### commands for interpreter tool
foreach command {
  {"init"  debug`init}
  {"step"  debug`step}
  {"step in" debug`stepin}
  {"single step" debug`singlestep}
  {"continue" debug`continue}
  {"finish" debug`finish}
  {"stop interpreter" functions`stopInterpreter}
  {"up" debug`up}
  {"down" debug`down}
} {
  set text [lindex $command 0]
  set cmd [lindex $command 1]
  set options`name2cmd(interptool,$text) $cmd
}

# This function is unused. etn/970909
proc options`createButtons w {
  global options`text2index options`name2cmd options`buttons
  
  set left [frame $w.left]
  set right [frame $w.right]
  pack $left $right -side left -fill both -expand 1 -padx 10

  pack [label $left.label -text "Buttons for\nMain mode"]
  tixScrolledListbox $left.lb -scrollbar auto
  $left.lb subwidget listbox configure -selectmode multiple -export no
  pack $left.lb -fill both -expand 1
  set main [$left.lb subwidget listbox]
  setSN options`buttons`main $main

  pack [label $right.label -text "Buttons for\nDebug Mode"]
  tixScrolledListbox $right.lb -scrollbar auto 
  $right.lb subwidget listbox configure -selectmode multiple -export no
  pack $right.lb -fill both -expand 1
  set debug [$right.lb subwidget listbox]
  setSN options`buttons`debug $debug

  ### inserting elements into the lists
  foreach but [array names options`name2cmd] {
    regexp {^(.+),(.+)$} $but all mode text
    set index [[set $mode]  index end]
    [set $mode] insert end $text
    set options`text2index($mode,$text) $index
  }

  options`hilightButtons
}

proc options`hilightButtons {} {
  global SN options`text2index options`buttons
  if {[info exists SN(options`buttons`main)] &&
      [winfo exists $SN(options`buttons`main)]} {
    foreach mode {main debug} {
      set lb $SN(options`buttons`$mode)
      $lb selection clear 0 end
      foreach text [set options`buttons($mode)] {
	set index [set options`text2index($mode,$text)]
	$lb selection set $index
      }
    }
  }
}

############################################################
# This function is called when the ok button on the option
# page is pressed.
# type   - either ok or cancel, which indicate the type of
#          exit.
# return - nothing
############################################################
proc options`exit {type} {
  global SN options`buttons
  set w $SN(options`top)
  wm withdraw $w
  if {[winfo exists .javaInterfaces]} {
    wm withdraw .javaInterfaces
  }
  
  if {$type == "ok"} {

    foreach group {editor misc} {
      global options`$group options`_$group
      array set options`${group} [ array get options`_$group ]
    }

    tkif_Options set options_vdm_setting

  } else { # type == "cancel"
    ### restoring the buttons listboxes
    options`hilightButtons
  }
}

############################################################
# This function write the options to the options file
############################################################
proc options`save {} {
  global options`buttons options`file

  set text {}

  ### the options in "editor misc"
  foreach group {editor misc} {
    global options`$group
    foreach elm [array names options`$group] {
      lappend text  "set options`${group}($elm) \"[options`escape [set options`${group}($elm)]]\""
    }
  }

  ### the buttons options
  foreach mode {main debug} {
    lappend text "set options`buttons($mode) \"[options`escape [set options`buttons($mode)]]\""
  }

  ### writing the options
  functions`writeOptions "gui-tk" $text
  
  ### writting the options which belongs to VDM
  below`saveOptions [set options`file]
}

############################################################
# This function load the options
############################################################
proc options`load {} {
  set text [functions`readOptions gui-tk]
  if {$text == ""} {
    ### no option file found
    return 
  }
  
  foreach line $text {
    uplevel \#0 $line
  }
  
  ### loading the options from vdm
  below`loadOptions
}

############################################################
# This function escapes every control chars for the
# save file
############################################################  
proc options`escape {string} {
  regsub -all {["#\${}]} $string \\\\\\0 result
  regsub -all {[[]} $result \\\\\\0 string
  regsub -all {[]]} $string \\\\\\0 result
  return $result
}
