###################################################################
#                    UML tool for VDM++ toolbox
###################################################################
#
# The UML tool is used by creating a toplevel or a frame and then
# calling umltool_tool_open, with the toplevel or frame as parameter
#
###################################################################



####################################################################
#                          UML tool Window. 
####################################################################
proc umltool_open {} {
  umltool_init_merge_win
}



###################################################################
#                     Class selecter packer
###################################################################
proc umltool_Create_SubWindow {tl title lbl_cat lbl_cl lbl_lv blcombo} {
   wm title $tl "$title"
   global glb_category

   ### Create the paned window

   frame $tl.topframe -borderwidth {2}

   frame $tl.topframe.cf -borderwidth {2}
   if {$blcombo} {
      frame $tl.topframe.cf.catframe -borderwidth {2}


      label $tl.topframe.cf.catframe.lbl -text $lbl_cat

      tixCombobox $tl.topframe.cf.catframe.comb\
          -width 50\
          -height 1\
          -editable {1} \
          -variable {glb_category}
   }

   if {$blcombo} {
      pack $tl.topframe.cf.catframe.lbl -side left

      pack $tl.topframe.cf.catframe.comb\
        -side left -fill both -expand 1

      pack $tl.topframe.cf.catframe -side top -fill x
   }

   pack $tl.topframe.cf -side left -fill both -expand 1

   pack $tl.topframe -fill both -side top -expand 1

   frame $tl.listframe -borderwidth {2}

   frame $tl.listframe.cl -borderwidth {2}

   label $tl.listframe.cl.lbl_class -text $lbl_cl
   pack  $tl.listframe.cl.lbl_class -anchor nw

   tixPanedWindow $tl.listframe.cl.classes\
      -orient horizontal

   set w $tl.listframe.cl.classes
   pack $w -expand 1 -fill both -anchor w

   set index [$w add index -size 250]

   tixScrolledListBox $index.box -scrollbar auto
   pack $index.box -fill both -expand 1

   set listbox [$index.box subwidget listbox]
   $listbox configure -selectmode extended

   pack  $tl.listframe.cl -side left


   frame $tl.listframe.buttons -borderwidth {2}

   button $tl.listframe.buttons.right  -text ">"

   button $tl.listframe.buttons.left  -text "<"

   button $tl.listframe.buttons.allright  -text ">>"

   button $tl.listframe.buttons.allleft  -text "<<"

   pack $tl.listframe.buttons.right\
        $tl.listframe.buttons.left\
        $tl.listframe.buttons.allright\
        $tl.listframe.buttons.allleft\
        -fill both -side top

   pack $tl.listframe.buttons -side left -fill x

   frame $tl.listframe.lv -borderwidth {2}

   label $tl.listframe.lv.lbl_class -text $lbl_lv
   pack  $tl.listframe.lv.lbl_class -anchor nw


   tixPanedWindow $tl.listframe.lv.lviews -orient horizontal
   set w $tl.listframe.lv.lviews
   pack $w -expand 1 -fill both -side left

   set index [$w add index -size 250]

   tixScrolledListBox $index.box -scrollbar auto
   pack $index.box -fill both -expand 1

   set listbox [$index.box subwidget listbox]
   $listbox configure -selectmode extended

   pack  $tl.listframe.lv -side left

   pack $tl.listframe -side top -fill x

   frame $tl.but_frame -borderwidth {2}  -relief sunken

   frame $tl.but_frame.buttons -borderwidth {2}



   button $tl.but_frame.buttons.ok  -text "OK" -command {
     puts "OK"
   }

   button $tl.but_frame.buttons.can  -text "Cancel" -command {
     puts "Cancel"
   }

   button $tl.but_frame.buttons.help  -text "Help" -command {
     puts "Help"
   }

   pack $tl.but_frame.buttons.ok\
        $tl.but_frame.buttons.can\
        $tl.but_frame.buttons.help\
        -fill both -side left

   pack $tl.but_frame.buttons -fill both -side top

   pack $tl.but_frame -fill both -side top -expand 1

}


########################################################################
#                        Close all windows
####################################################################
proc umltool_Close_Windows {} {
  set w .uml_tool
  if {[winfo exist $w]} {
     destroy $w
  }

  set w .uml_lviews
  if {[winfo exist $w]} {
     destroy $w
  }

  set w .umltool_msgbox
  if {[winfo exist $w]} {
     destroy $w
  }

}




###################################################################
#                        UML tool message box
###################################################################
proc msg_box {msg} {
   set w .umltool_msgbox

  if {[winfo exist $w]} {
     [$w.txt subwidget text] delete 0.0 end
     [$w.txt subwidget text] insert end $msg
     wm deiconify $w
     raise $w
     focus $w
     return
  }
  
   toplevel $w

   wm title $w "UML tool message"
   wm protocol $w WM_DELETE_WINDOW\
    { wm withdraw .umltool_msgbox; puts "withd" }


   set text [tixScrolledText $w.txt -scrollbar auto\
              -width 350 -height 150]
   pack $text -fill both -expand 1

   set tixtxt [$text subwidget text]
   $tixtxt delete 0.0 end
   $tixtxt insert end $msg

   button $w.but -text "OK" -command {wm withdraw .umltool_msgbox}
   pack $w.but -side top
   focus $w
}



###################################################################
#                    Extract the selected classes
###################################################################
proc umltool_get_selected {} {
  global umllist
  set sellist {}
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {
    global var$i
    set list1 [lindex $umllist $i]
    set name [lindex $list1 0]
    if {[set var$i] == 1} {
      lappend sellist $name
      set var$i 0
    }
  }
  return $sellist
}


###################################################################
#                    Extract status of the buttons
###################################################################
proc umltool_extract_status {} {
  global umllist
  set statelist {}
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {
    set list1 [lindex $umllist $i]
    set status [lindex $list1 3]
    lappend statelist $status
  }
  return $statelist
}


###################################################################
#                    Extract status of the buttons
###################################################################
proc umltool_extract_name_status {} {
  global umllist
  set statelist {}
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {
    set list1 [lindex $umllist $i]
    set name [lindex $list1 0]
    set status [lindex $list1 3]
    set tmplist {}
    lappend tmplist $name
    lappend tmplist $status
    lappend statelist $tmplist
  }
  return $statelist
}



###################################################################
#          Calculate possible states of the buttons
###################################################################
proc umltool_possible_states {list} {
  set ret_list {0 1 2 3}
  if {[string compare [lindex $list 2] "D"] == 0} {
     set ret_list [lreplace $ret_list 3 3]
  } else {
     if {[string compare [lindex $list 2] "?"] == 0} {
        if {[catch {set ret_list [lreplace $ret_list 3 3]}]} {
        }
        set ret_list [lreplace $ret_list 1 1]
    }
  }
  if {[string compare [lindex $list 1] "D"] == 0} {
     if {[catch {set ret_list [lreplace $ret_list 3 3]}]} {
     }
     if {[string compare [lindex $list 2] "D"] == 0} {
        set ret_list [lreplace $ret_list 1 2]
     }
     if {[string compare [lindex $list 2] "?"] == 0} {
        set ret_list [lreplace $ret_list 2 2]
     }
  } else {
    if {[string compare [lindex $list 1] "?"] == 0} {
      if {[catch {set ret_list [lreplace $ret_list 3 3]}]} {
      }
      if {[string compare [lindex $list 2] "D"] == 0} {
        set ret_list [lreplace $ret_list 1 1]
      }
      set ret_list [lreplace $ret_list 2 2]
    }
  }
  return $ret_list
}


###################################################################
#           Get the next state of possible states
###################################################################
proc umltool_nextstate {posstates status} {
  set lidx [lsearch $posstates $status]
  if {$lidx == -1} {
    set status 0
  } else {
    incr lidx
    if { $lidx < [llength $posstates] } {
      set status [lindex $posstates $lidx]
    } else {
      set status 0
    }
  }
  return $status
}




###################################################################
#              Change state of button
###################################################################
proc umltool_toggle_state {w idx} {
  global umllist
  global ACT_LIST

  set newstat [lindex $umllist $idx]
  set posstates [umltool_possible_states $newstat]

  set status [lindex [lindex $umllist $idx] 3]

  set status [umltool_nextstate $posstates $status]

  set newstat [lreplace [lindex $umllist $idx] 3 3 $status]
  set umllist [lreplace $umllist $idx $idx $newstat]

  $w.f$idx.b config -bitmap bm`$ACT_LIST($status)
}


###################################################################
#               Create a button in scrolled window
###################################################################
proc umltool_run_select {w idx classdesc} {
    global var$idx
    global ACT_LIST

    set class [lindex $classdesc 0]
    set vpp [lindex $classdesc 1]
    set uml [lindex $classdesc 2]
    set status [lindex $classdesc 3]

    frame $w.f$idx
    checkbutton $w.f$idx.a \
      -text $class \
      -variable var$idx \
      -anchor w

    set var$idx 0

    button $w.f$idx.b -command "umltool_toggle_state $w $idx" \
                     -bitmap bm`$ACT_LIST($status) \
                     -width 40

    label $w.f$idx.vpp -text $vpp -width 5 -anchor c
    label $w.f$idx.uml -text $uml -width 5 -anchor c

    pack $w.f$idx.uml -side right -expand no -padx 5
    pack $w.f$idx.b -side right -expand no -anchor w -padx 5 
    pack $w.f$idx.vpp -side right -expand no -padx 5

    pack $w.f$idx.a -side left -expand no -anchor w -padx 5
    pack $w.f$idx -expand yes -fill x
}


###################################################################
#            Initialize the class description list
###################################################################
proc umltool_init_list {w} {
  proc sortcmd {seq1 seq2} {
    set str1 [lindex $seq1 0]
    set str2 [lindex $seq2 0]
    return [string compare $str1 $str2]
  }

  global umllist
  global defaultstatus
  eval destroy [winfo children $w]
  set umllist [lsort -command sortcmd [tkif_GetClassStatus]]
  set defaultstatus [umltool_extract_status]
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {
     umltool_run_select $w $i [lindex $umllist $i]
  }
##  update
}


###################################################################
#              Main window in the UML merge tool
###################################################################
proc umltool_init_merge_win {} {
  global umllist
  global ACT_LIST

  global rightbox
  global f
  
  set i 0
  foreach var { no_action right2left left2right both_ways } {
    set ACT_LIST($i) $var
    incr i 1
  }
  if {[tkif_SetMapperState 1]} {
    if {![winfo exist .uml_tool]} {
      set w [toplevel .uml_tool]
      wm protocol $w WM_DELETE_WINDOW\
        { wm withdraw .uml_tool; puts "withd" }
      wm title $w "Rose-VDM++ Link"
      help`add $w uml_tool`main

      set pane [tixPanedWindow $w.pane]
      set upperf [$pane add dialog]
      set bottomf [$pane add output]
      pack $pane
  
      set mainframe [frame $upperf.main -bd 1 -relief raised]
      set top [frame $mainframe.top -bd 1 -relief raised]
      set lf [frame $top.lblframe]
      tixScrolledWindow $top.sw -height 180 -scrollbar y
      set f [$top.sw subwidget window]

      set boxframe [frame $mainframe.f]
      help`add $boxframe uml_tool`buttons

      set bottomlbox [tixButtonBox $boxframe.lbb -bd 1 -bg gray ]
      set bottomrbox [tixButtonBox $boxframe.rbb -bd 1 -bg gray ]
      set rightbox [tixButtonBox $upperf.rb -bd 1 -orient vertical\
                    -pady 2 -bg gray ]
      
      label $lf.cl -text "Classes"
      label $lf.sp1 -text ""
      label $lf.vpp -text "VDM++"
      label $lf.uml -text "UML"
  
      pack $lf.cl -side left -padx 15
      pack $lf.sp1 -side right -padx 10
      pack $lf.uml -side right -padx 20
      pack $lf.vpp -side right -padx 45
  
      pack $lf -fill x -side top
      pack $mainframe.top.sw -expand yes -fill both -padx 10 -pady 10 \
          -side top
      
      pack $boxframe -side bottom -expand no -fill x -anchor s
      pack $mainframe.top -side top  -expand yes -fill both

      help`add $bottomlbox uml_tool`selButs
      pack $bottomlbox -side left -expand yes -fill x

      help`add $bottomrbox uml_tool`dirButs
      pack $bottomrbox -side right -expand yes -fill x

      # The relation between w.bottom and umllog must be registered
      # in order to call umllog`pack
      windows`register_root $bottomf umllog 

      # pack the contents of upperf
      pack $mainframe -side left -fill both -expand yes

      help`add $rightbox uml_tool`actionButs
      pack $rightbox -side right -fill both -expand yes -anchor n

      umllog`pack $bottomf
      umltool_init_list $f

      # Create the buttons
      #
      $bottomlbox add all -text All -width 6 -under 0 \
          -command {umltool_selectALL}

      $bottomlbox add none  -text None -width 6 -under 0 \
          -command {umltool_selectNONE}

      $bottomrbox add toright -bitmap bm`all_right -width 40 -under 0 \
        -command "umltool_setall 2 $f"
      
      $bottomrbox add toleft -bitmap bm`all_left -width 40 -under 0 \
        -command "umltool_setall 1 $f"

      $bottomrbox add default -text Default -width 6 -under 0 \
        -command "umltool_setdefault $f"
      
      $rightbox add diff -text Diff -width 10 -under 0 \
          -command { 
             set selected [umltool_get_selected]
             if {[llength $selected] > 0} {
               tkif_UML_Diff $selected umllog`addmsg
             }
           }

      $rightbox add map -text Map -width 10 -under 0 \
          -command  { 
               if {![tkif_Conflicts [umltool_extract_name_status] umllog`addmsg ]} {
                   set status [umltool_extract_name_status]
                   destroy .uml_tool
                   tkif_MapUML $status mainlog`addmsg
               }
           }

      $rightbox add can -text Close -width 10 -under 0 \
          -command "destroy $w"
      
      wm geometry $w 550x530
      raise .uml_tool
      focus .uml_tool
    } else {
      umltool_init_list $f
      wm deiconify .uml_tool
      raise .uml_tool
      focus .uml_tool
    }
  }
}



###################################################################
#                Set ALL checkbuttons to selected
###################################################################
proc umltool_selectALL {} {
  global umllist
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {
    global var$i
    set var$i 1
  }
}

###################################################################
#               Set ALL checkbuttons to unselected
###################################################################
proc umltool_selectNONE {} {
  global umllist
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {
    global var$i
    set var$i 0
  }
}

###################################################################
#                   Set status to default
###################################################################
proc umltool_setdefault {w} {
  global umllist
  global defaultstatus
  global ACT_LIST

#  set w .uml_tool.upperf.main.top.sw.f2.window
  
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {

    set newstat [lindex $umllist $i]
    set posstates [umltool_possible_states $newstat]

    set status [lindex $defaultstatus $i]

    set newstat [lreplace [lindex $umllist $i] 3 3 $status]
    set umllist [lreplace $umllist $i $i $newstat]

    $w.f$i.b config -bitmap bm`$ACT_LIST($status) 
    
  }

}


###################################################################
#               Select seacrhed status or none
###################################################################
proc umltool_newstate {posstates stat1st} {
  set lidx [lsearch $posstates $stat1st]
  if {$lidx == -1} {
    set status 0
  } else {
    set status $stat1st
  }
  return $status
}


###################################################################
#   Set all status to either left or right depend on state
###################################################################
proc umltool_setall {state w} {
  global umllist
  global ACT_LIST

#  set w .uml_tool.upperf.main.top.sw.f2.window
  
  for {set i 0} {$i < [llength $umllist]} {incr i 1} {

    set newstat [lindex $umllist $i]
    set posstates [umltool_possible_states $newstat]

    set status [umltool_newstate $posstates $state]

    set newstat [lreplace [lindex $umllist $i] 3 3 $status]
    set umllist [lreplace $umllist $i $i $newstat]

    $w.f$i.b config -bitmap bm`$ACT_LIST($status)
  }
}



###################################################################
#
###################################################################
proc umltool_diffresult {s} {
  if {![winfo exist .uml_diff]} {
    set w [toplevel .uml_diff]
    wm title $w "Diff"

    wm protocol $w WM_DELETE_WINDOW\
      { wm withdraw .uml_diff; puts "withd" }

    set lf [frame $w.lf]
    set l [label $lf.l -text "The result of diff:"]
    set f [frame $w.f]
    set t [tixScrolledText $f.t -scrollbar auto \
              -width 500 -height 300]



    pack $lf -side top  -expand yes -fill both
    pack $l -side left -pady 2
    pack $t -side top  -expand yes -fill both
    pack $f -side top  -expand yes -fill both

    set bbox [tixButtonBox $w.bb -bd 1 -relief raised]
    pack $bbox -side left -expand yes -fill x

    $bbox add close -text Close -width 6 -under 0 \
       -command { wm withdraw .uml_diff }



    set tixtxt [$t subwidget text]
    $tixtxt delete 0.0 end
    $tixtxt insert end $s

    raise $w
    focus $w
  } else {
    set tixtxt [.uml_diff.f.t subwidget text]
    $tixtxt delete 0.0 end
    $tixtxt insert end $s

    wm deiconify .uml_diff
    raise .uml_diff
    focus .uml_diff
  }
}



############################################################
# This function pack the log frame
# w - the window to pack in
############################################################
proc umllog`pack w {
  setSN umllog`top $w
  help`add $w uml_tool`log
  general`packLabel $w umllog
  set text [tixScrolledText $w.text -scrollbar auto]
  pack $text -fill both -expand 1
  setSN umllog`text [$text subwidget text]
  set text [$text subwidget text]
  AddFocusWin $text
  umllog`init
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc umllog`save {} {
  general`saveText umllog
}
proc umllog`load {} {
  general`loadText umllog
}

proc umllog`addmsg {msg} {
  umllog`add $msg
}

proc umllog`add mesg {
  global SN umllog`text

  umllog`setstatus normal

  set text $SN(umllog`text)
  $text insert end $mesg
  $text yview [$text index end]
##  update idletasks

  umllog`setstatus disabled
}


############################################################
# These two function print/pipe the content of the
# 'umllog' buffer
############################################################
proc umllog`print {} {
  general`printText umllog
}
proc umllog`pipe {} {
  general`pipeText umllog
}

############################################################
# These two function are used to modify read-only status 
############################################################

proc umllog`setstatus {value} {
  global SN
  $SN(umllog`text) configure -state $value
}

proc umllog`init {} {
  umllog`setstatus disabled
}

############################################################
# This functions clear the umllog window
############################################################
proc umllog`clear {} {
  umllog`setstatus normal
  general`clear umllog
  umllog`setstatus disabled
}
