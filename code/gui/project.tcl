######################################################################
# Project handling. 
#
# $Id: project.tcl,v 1.51 2000/10/09 11:06:39 paulm Exp $
######################################################################

#
# This rutine has the effect on Windows that update is called
# It is used to get the menu bar removed
# 
proc project`updatewin {} {
  if {[Config isWinOS]} {
    update
  }
}

############################################################
# Open a window for the user to select a project to open
# return - nothing
############################################################

proc project`newtitle {name} {
  set file [ tkif_GetBasename $name ]
  if {$file == ""} {
    set file "New Project"
  }
  # regsub -- {\.prj$} $file  {} file
  maintool setTitle "$file   -   [tkif_GiveToolTitle]"
}

# Callback function: 
proc project`changedprojname {old new} {
  # log`add "Project changed from {$old} to {$new}\n"

  project`newtitle $new

  global project_Silent_callback
  if {[info exists project_Silent_callback]} {
    #log`add "silent=$project_Silent_callback\n"
    if $project_Silent_callback {
      # log`add "Silent\n"
      return
    } 
  }
  set dir   [ tkif_GetBasedir $new ]
  if {$new == ""} {
    # log`add "\ncd [tkif_Getcwd]\n"
  } else {
    set olddir [ tkif_GetBasedir $old ]
    if {[string compare $olddir $dir] != 0} {
      set res [ tkif_chdir $dir ]
      if {$res == 0} {
        # log`add "\ncd $dir\n"
      } else {
        log`add "\nFailed to: cd $dir\n"
      }
    }
    SetLastConfigDir $dir
  }
}

# Return 1 if save was not need or project was saved
# or changes shouldn't be saved.
# Otherwise 0 (if save was needed but user canceled)
 
proc project`saveifmodified {} {
  global project_Silent_callback
  set project_Silent_callback 1
  set res 1
  if {![tkif_StateIsSaved]} {

    if {[Config isWinOS]} {
      # This causes the tk_dialog below to open above the main window,
      # without it the dialog may be put below it!
      # It is not certain why it works, though. 
      focus [ modules getPath ]
      update
    }
    
    set answer [tk_dialog .savechanges \
        "Project is modified" "Save changes to project?" \
        {} 0 "Yes" "No" "Cancel" ]
    if {$answer == 0} {
      # Yes, save changes
      set file [tkif_GetProjectName]
      if {$file != ""} {
        project`save
      } else {
        set saved [ project`saveAs ]
        if !$saved {
          # Cancel
          set res 0
        }
      }
    } elseif {$answer == 1} {
      # No. Fall through
    } else {
      # Cancel 
      set res 0
    }
  }
  set project_Silent_callback 0
  return $res
}


proc project`open {} {

  project`updatewin

  set ok [ project`saveifmodified ]
  if !$ok {
    return
  }
  ### creating the file selection box
  global tk_strictMotif tcl_platform
  # if tk_strictMotif:
  # tk_getOpenFile does not put the file types pattern
  # into the Filter entry. 
  
  set types {
    {"Project files" {.prj}	}
    {"All files" *}
  }
  
  global tk_strictMotif tcl_platform
  if ![string compare $tcl_platform(platform) unix] {
    set oldmotif $tk_strictMotif
    # Change back to 1 when getOpen file works better. 
    set tk_strictMotif 0
  }
  set tk_strictMotif 0
  set file [tk_getOpenFile -filetypes $types \
      -parent [maintool GetToplevel] \
      -defaultextension {.prj} \
      -title {Select a project}
  ]
  set file [general`winfixslash $file]
  
  if ![string compare $tcl_platform(platform) unix] {
    set tk_strictMotif $oldmotif
  }

  if {$file != ""} {
    project`openNow $file;
  }

  ### creating the help references
  # help`add $dialog project`open

  # $dialog popup

}

############################################################
# This function is called when a project shall be opened
# path   - path to the project
# return - nothing
############################################################
proc project`openNow {path} {
  project`updatewin

  if {[file isdirectory $path]} {
    log`add "No project was selected.\n"
    return;
  }
  
  set dir [ tkif_GetBasedir $path ]

  general`clearwindows
  windows`Busy on
  tkif_OpenProject $path
  windows`Busy off
  
  set pname [tkif_GetProjectName]
  if {$pname == ""} {
    log`add "No project was opened.\n"
    return
  }

  log`add "Project opened: $pname\n"
  
  # tkif_executeCommand "dir $dir" mainlog`addmsg

  ##################################################
  # Now project is opened.
  # Do general initialisation.
  ##################################################
  
  menuline`enable projectMenu Configure
  
  if {[tkif_IsVdmppType]} {
    options`resetInterfaces
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
    if [IsInhTreeOpen] {
      tkif_drawInhTree
    }
  }
}


############################################################
# Open a new window for selecting a name for a new project
# return - nothing
############################################################
proc project`new {} {

  project`updatewin

  set ok [ project`saveifmodified ]
  if !$ok {
    return
  }
  general`clearwindows
  tkif_NewUnnamedProject
  log`add "New project created\n"
  # log`add "cd [tkif_Getcwd]\n"
  if {[tkif_IsUmlType]} {
    tkif_ResetMapper
  }
  options`resetInterfaces
}

############################################################
# SaveAs
# Open a new window for selecting a name to save a project to
# a file. 
# return : 1 if project was saved. otherwise 0. 
############################################################
proc project`saveAs {} {

  project`updatewin
  
  ### creating the file selection box
  set types {
    {"Project files"		{.prj}	}
    {"All files"		*}
  }
  global tk_strictMotif tcl_platform
  if ![string compare $tcl_platform(platform) unix] {
    set oldmotif $tk_strictMotif
    set tk_strictMotif 0
  }
  set currentname [tkif_GetProjectName]
  set file [tk_getSaveFile -filetypes $types \
      -parent [maintool GetToplevel] \
      -title {Save As} \
      -initialfile "$currentname" \
      -defaultextension {.prj}
  ]
  set file [general`winfixslash $file]
  
  if ![string compare $tcl_platform(platform) unix] {
    set tk_strictMotif $oldmotif
  }

  if {$file != ""} {
    tkif_SaveAs $file
    log`add "Project saved to: $file\n"
    return 1
  } else {
    return 0
  }
}

proc project`save {} {

  project`updatewin

  ### creating the file selection box
  set types {
    {"Project files"		{.prj}	}
    {"All files"		*}
  }
  set currentname [tkif_GetProjectName]    
  if {$currentname == ""} {
    project`saveAs
  } else {
    tkif_SaveAs $currentname
    log`add "Project saved to: $currentname\n"
  }
}

############################################################
# This function is called when a new project shall be
# created.
# directory - the directory in which the project file
#             should be
# filename  - the name of the new project
############################################################
proc project`update_state_add {} {
  global SN
  set but $SN(project`removeButton)
  $but configure -state disabled

  set lb $SN(project`fromListBox)
  set indices [$lb curselection]
  if {[llength $indices]>0} {
    $SN(project`addButton) configure -state normal
  } else {
    $SN(project`addButton) configure -state disabled
  }
}
proc project`update_state_remove {} {
  global SN
  set but $SN(project`addButton)
  $but configure -state disabled

  set lb $SN(project`toListBox)
  set indices [$lb curselection]
  if {[llength $indices]>0} {
    $SN(project`removeButton) configure -state normal
  } else {
    $SN(project`removeButton) configure -state disabled
  }
}

proc project`doAdd {} {
  global SN
  set lb $SN(project`fromListBox)
  set indices [$lb curselection]
  set dir [ $SN(project`dirList) cget -value ]
  foreach i $indices {
    project`addToProject [tkif_JoinPaths $dir [$lb get $i]]
  }
}

proc project`doRemove {} {
  global SN
  set lb $SN(project`toListBox)
  set indices [$lb curselection]
  set dir [ $SN(project`dirList) cget -value ]
  foreach i [lsort -integer -decreasing $indices] {
    $lb delete $i 
  }
  # Remove selection and disable remove button. 
  $lb selection clear 0 end
  project`update_state_remove
}

##################################################

global LastConfigureDir

proc SetLastConfigDir {dir} {
  global LastConfigureDir
  set LastConfigureDir $dir
}

proc GetLastConfigDir {} {
  global LastConfigureDir
  if ![info exists LastConfigureDir] {
    set LastConfigureDir [tkif_Getcwd]
  }
  return $LastConfigureDir

}
  
proc SaveCwd {} {
  global CurCWD
  set CurCWD [ tkif_Getcwd ]
}

proc RestoreCwd {} {
  global CurCWD
  set res [ tkif_chdir $CurCWD ]
}


############################################################
# This will configure a project.
# new - true if a project doesn't already exists.
# return nothing
############################################################
proc project`configure {} {
  
  project`updatewin

  set w .configure

  if {[winfo exist $w]} {
    wm deiconify $w
    raise $w
    return
  }
  toplevel $w

  SaveCwd
  
  set f1 [frame $w.f1]
  set f5 [frame $w.f5] 
  set f2 [frame $w.f2]
  set f4 [frame $w.f4]
  pack $f1 $f5 $f2 $f4 -fill both -expand 1

  help`add $w project`configure

  ### the label over the files
  label $f1.label -text "Files on the file system"
  pack $f1.label  -anchor c -ipady 3
  button $f5.add  -text "Add to project" -command { project`doAdd }
  button $f5.remove -text "Remove from project" -command { project`doRemove }
  setSN project`addButton $f5.add
  setSN project`removeButton $f5.remove
  pack $f5.add $f5.remove  -side left -expand 1 -pady 10

  label $f2.label -text "Files in the project"
  pack $f2.label  -anchor c -ipady 3



  ### the filter field
  tixLabelEntry  $f1.filter -label "Filter: "  -options {
    entry.width 0
    entry.textVariable project`filter
  }
  pack $f1.filter -fill x -expand 1 -padx 3
  set entry [$f1.filter subwidget entry]
  bind $entry <Return> {
    set dirList $SN(project`dirList)
    project`setFromFiles [$dirList cget -value]
  }
  bind $entry <KP_Enter> {
    set dirList $SN(project`dirList)
    project`setFromFiles [$dirList cget -value]
  }
  
  ### setting the filter, on windows the default is rtf
  global project`filter
  if {[tkif_IsVdmslType] || [tkif_IsVdmslLiteType]} {
    set project`filter "*.vdm"
  }
  if {[tkif_IsVdmppType]} {
    set project`filter "*.vpp"
  }  
  if {[Config isWinOS]} {
    set project`filter "*.rtf"
  }
    
  ### the directory/file frame
  set paned [tixPanedWindow $f1.paned -orientation horizontal]
  pack $paned -fill both -expand 1
  set dir [$paned add dir]
  set file [$paned add file]

  ### the file browser
  set filelist [tixScrolledListBox $file.box -options {
    listbox.height 10
  } ]
  set listbox [$filelist subwidget listbox]
  setSN project`fromListBox $listbox
  # Now the directory browser. We finish the file browser below
  
  ### the directory browser
  set dirlist [tixDirList $dir.dirlist -value [GetLastConfigDir] \
      -command project`setFromFiles -options {
    hlist.height 2
  } ]
  pack $dirlist -fill both -expand 1
  setSN project`dirList $dirlist

  # Return to the file browser. 
  set lb [$filelist subwidget listbox ]
  $lb configure -selectmode extended
  # Both the mouse and the keyboard can be used for selection. 
  bind $lb <ButtonRelease-1> "project`update_state_add"
  bind $lb <KeyRelease> "project`update_state_add"
  bind $lb <Double-Button-1> { project`doAdd }
  pack $filelist -fill both -expand 1

  
  ### the files in the project
  set tofiles [tixScrolledListBox $f2.box -options {
    listbox.height 5
  } ]
  set lb [$tofiles subwidget listbox ]
  $lb configure -selectmode extended
  # Both the mouse and the keyboard can be used for selection. 
  bind $lb <ButtonRelease-1> "project`update_state_remove"
  bind $lb <KeyRelease> "project`update_state_remove"
  bind $lb <Double-Button-1> { project`doRemove }

  pack $tofiles -fill both -expand 1
  setSN project`toListBox [$tofiles subwidget listbox]
  ### !!! ETN Here the old project files are added 
  foreach file [tkif_getFiles] {
    project`addToProject $file
  }

  ### buttons
  set buttons [frame $f4.buttons]
  pack $buttons -expand 1 -fill x -pady 10

  button $buttons.ok -text Ok -command "project`endConfigure;destroy $w; RestoreCwd"
  button $buttons.cancel -text Cancel -command "destroy $w; RestoreCwd"
  pack $buttons.ok $buttons.cancel -side left -expand 1

  # Set button states
  project`update_state_add 
  project`update_state_remove
}

############################################################
# Update the listbox with files, to include the files in
# the directory given with 'dir'
# dir    - the directory in which the files are located
# return - nothing
############################################################
proc project`setFromFiles {dir} {
  global SN project`filter
  set listbox $SN(project`fromListBox)
  $listbox delete 0 end
  # Escape backslash in $dir for the globbing command.
  regsub -all -- {\\} $dir {\\\\} dir
  foreach file [glob -nocomplain $dir/${project`filter}] {
    if [file isfile $file] {
      $listbox insert end [file tail $file]
    }
  }		      
}

############################################################
# Add a file to the "files in project" listbox
# file   - then file to add to the listbox
# return - nothing
############################################################
proc project`addToProject {file} {
  global SN
  set listbox $SN(project`toListBox)
  set size [$listbox size]
  for {set i 0} {$i < $size} {incr i} {
    set c [string compare $file [$listbox get $i]]
    if {$c == 0} {
      $listbox see $i
      return
    }
    if {$c == -1} {      
      break
    }
  }
  $listbox insert $i $file
  $listbox see $i
}


############################################################
# This function is called when the user press the OK
# button in the project->configure window
# Tell the toolbox which files is in the project
############################################################
proc project`endConfigure {} {
  global SN 

  # Update LastConfigureDir
  set dirlist $SN(project`dirList)
  SetLastConfigDir [ $dirlist cget -value ]
  
  set old [tkif_getFiles]
  set listbox $SN(project`toListBox)
  set new [$listbox get 0 end]
  set size [$listbox size]
  set rlist ""
  for {set i 0} {$i < [llength $old]} {incr i} {
    set file [lindex $old $i]
    if {[lsearch -exact $new $file] == -1} {
      lappend rlist $file
    }
  }
  tkif_removeFiles $rlist
  set alist ""
  for {set i 0} {$i < $size} {incr i} {
    set file [lindex $new $i]
    if {[lsearch -exact $old $file] == -1} {
      lappend alist $file
    }
  }
  tkif_addFiles $alist


}

############################################################
# This will quit the program
############################################################
proc project`quit {} {
  set ok [ project`saveifmodified ]
  if !$ok {
    return
  }
  tkif_CleanUp
  exit 0
}
