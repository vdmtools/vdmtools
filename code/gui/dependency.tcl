######################################################################
# Dependency Window. 
#
# $Id: dependency.tcl,v 1.13 1999/02/19 13:12:43 blackie Exp $
######################################################################

############################################################
# The Name of the selected class
############################################################


proc dependencynamemess`pack w {
  setSN dependencynamemess`top $w
  ### create the label
  help`add $w dependencyPage`dependencynamemess

  pack [frame $w.labelframe -relief ridge -bd 2] -fill x 
  label $w.labelframe.labeltext -text "No classes selected" 
  button $w.labelframe.update -text "Update Dependencies" -command tkif_compDepend
  pack $w.labelframe.labeltext -anchor w -side left
  pack $w.labelframe.update -anchor e
  setSN modules`dependencyclasslabel $w.labelframe.labeltext
}

proc dependencynamemess`init {} {
  dependencynamemess`reset
}

proc dependencynamemess`reset {} {
  global SN
  set classlabel $SN(modules`dependencyclasslabel)
  $classlabel configure -text "No classes selected"
###  dependencynamemess setTitle "No class dependencyiations"
}

proc dependencynamemess`toomany {} {
  global SN
  set classlabel $SN(modules`dependencyclasslabel)
  $classlabel configure -text "More than one class selected"
###  dependencynamemess setTitle "No class dependencyiations"
}

proc dependencynamemess`updateNameMess {name compl} {
  global SN
  set classlabel $SN(modules`dependencyclasslabel)
  if {$compl} {
    $classlabel configure -text "Dependency information on class: $name"
###    dependencynamemess setTitle  "Dependency information of class: $name"
  } else {
    $classlabel configure -text "Dependency information on class: $name *"
###    dependencynamemess setTitle  "Dependency information of class: $name *"
  }
}




############################################################
# The Super classes frame
# w - the window to pack in
############################################################
proc super_classes`pack w {
  setSN super_classes`top $w
  ### create the label
  help`add $w dependencyPage`super_classes
  general`packLabel $w super_classes

  ### creating the superclass listboxes
  tixScrolledListBox $w.listbox
  pack $w.listbox -expand 1 -fill both
  set superclassbox [$w.listbox subwidget listbox]
  setSN modules`superclassbox $superclassbox
  AddFocusWin $superclassbox
  $superclassbox configure -selectmode extended -export no

  bind $superclassbox <ButtonRelease-1> {
    global modules`select
    set modules`select super_classes
    modules`selectFunctions
  }
  $superclassbox delete 0 end
}

proc super_classes`init {} {
  global SN
  set w $SN(modules`superclassbox)
  $w delete 0 end
}

proc super_classes`reset {} {
  global SN
  $SN(modules`superclassbox) delete 0 end
}

############################################################
# The Uses frame
# w - the window to pack in
############################################################
proc uses`pack w {
  setSN uses`top $w
  ### create the label
  help`add $w dependencyPage`uses
  general`packLabel $w uses

  ### creating the uses listboxes
  tixScrolledListBox $w.listbox
  pack $w.listbox -expand 1 -fill both
  set usesbox [$w.listbox subwidget listbox]
  setSN modules`usesbox $usesbox
  AddFocusWin $usesbox
  $usesbox configure -selectmode extended -export no

  bind $usesbox <ButtonRelease-1> {
    global modules`select
    set modules`select uses
    modules`selectFunctions
  }
  $usesbox delete 0 end
}

proc uses`init {} {
  global SN
  set w $SN(modules`usesbox)
  $w delete 0 end
}

proc uses`reset {} {
  global SN
  $SN(modules`usesbox) delete 0 end
}

############################################################
# The Sub classes frame
# w - the window to pack in
############################################################
proc sub_classes`pack w {
  setSN sub_classes`top $w
  ### create the label
  help`add $w dependencyPage`sub_classes
  general`packLabel $w sub_classes

  ### creating the subclass listboxes
  tixScrolledListBox $w.listbox
  pack $w.listbox -expand 1 -fill both
  set subclassbox [$w.listbox subwidget listbox]
  setSN modules`subclassbox $subclassbox
  AddFocusWin $subclassbox
  $subclassbox configure -selectmode extended -export no

  bind $subclassbox <ButtonRelease-1> {
    global modules`select
    set modules`select sub_classes
    modules`selectFunctions
  }
  $subclassbox delete 0 end
}

proc sub_classes`init {} {
  global SN
  set w $SN(modules`subclassbox)
  w delete 0 end
}

proc sub_classes`reset {} {
  global SN
  $SN(modules`subclassbox) delete 0 end
}

############################################################
# The Used by frame
# w - the window to pack in
############################################################
proc used_by`pack w {
  setSN used_by`top $w
  ### create the label
  help`add $w dependencyPage`used_by
  general`packLabel $w used_by

  ### creating the usedby listboxes
  tixScrolledListBox $w.listbox
  pack $w.listbox -expand 1 -fill both
  set usedbybox [$w.listbox subwidget listbox]
  setSN modules`usedbybox $usedbybox
  AddFocusWin $usedbybox
  $usedbybox configure -selectmode extended -export no

  bind $usedbybox <ButtonRelease-1> {
    global modules`select
    set modules`select usedby
    modules`selectFunctions
  }
  $usedbybox delete 0 end
}

proc used_by`init {} {
  global SN
  set w $SN(modules`usedbybox)
  $w delete 0 end
}

proc used_by`reset {} {
  global SN
  $SN(modules`usedbybox) delete 0 end
}

proc dependencytool_open {} {

  set w .dependencytool

  if {[winfo exist $w]} {
    if {[wm state $w] != "normal"} {
      dependencytool maybesetwmgeo
    }
    wm deiconify $w
    raise $w
    ### Compute dependency
    pack [label $w.wait -text "Calculating dependency information..."]
    update
    tkif_compDepend
    destroy $w.wait
    ### Update Dependencies
    set selnames [modules`selectedModules]  
    if {[llength $selnames] == 1} {
      tkif_updateDepends $selnames
    } else {
      dependency`reset
    }
    return
  }
  
  toplevel $w
 
}

proc IsDependOpen {} {
  if {[string compare [wm state .dependencytool] normal] == 0} {
    return 1
  } else {
    return 0
  }
}

############################################################
# Generate Dependency Information
############################################################

## What if more than one class is selected???

proc dependency`reset {} {
  super_classes`reset
  sub_classes`reset
  uses`reset
  used_by`reset
}

proc UpdateDependInfo {name superclasses subclasses usesclasses usedbyclasses compl} {
  global SN
  set selnames [modules`selectedModules]  
  ## Is this a good solution???
  if {[llength $selnames] == 1} {
    ### Only one class selected - create dependencies.
    dependencynamemess`updateNameMess $name $compl
    set superclassbox $SN(modules`superclassbox)
    set usesbox $SN(modules`usesbox)
    set subclassbox $SN(modules`subclassbox)
    set usedbybox $SN(modules`usedbybox)
    $superclassbox delete 0 end
    $usesbox delete 0 end
    $subclassbox delete 0 end
    $usedbybox delete 0 end

    foreach el $superclasses {
      $superclassbox insert end $el
    }
    foreach el $subclasses {
      $subclassbox insert end $el
    }
    foreach el $usesclasses {
      $usesbox insert end $el
    }
    foreach el $usedbyclasses {
      $usedbybox insert end $el
    }

  } elseif {[llength $selnames] == 0} {
    ### No classes selected - do not create dependencies.
    dependency`reset
    dependencynamemess`reset
  } else {
    ### More than one class selected - do not create dependencies.
    dependency`reset
    dependencynamemess`toomany
  }
}

proc UpdateDepends {classnames} {
  global SN
  if {[IsDependOpen]} {
    if {[llength $classnames] == 1} {
      set clname [lindex $classnames 0]
      tkif_updateDepends $clname
    } else {
      dependency`reset
    }
  }
}
    
