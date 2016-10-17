
############################################################
# Create the buttons initially, and unmap them, so they
# can be map very fast.
# return    - nothing 
############################################################
proc buttons`pack {tool} {
  global options`misc options`buttons SN options`name2cmd MODE

  ### packing the container frame
  set root [windows`getroot $tool]
  set butwin "$root.buttons"
  if {![winfo exists $butwin]} {
    # puts "butwin=$butwin" 
#    set tsw [ tixScrolledWindow $butwin -scrollbar none ] 
#    pack $tsw -fill x -anchor w
#    set w [$butwin subwidget window]
    set w [frame $root.buttons  -relief ridge -bd 1] 
    pack $w -fill x -anchor w
    setSN $tool`buttons`top $w
  } else {
    set w $SN($tool`buttons`top)
  }
  # puts "SN($tool`buttons`top)"
  
  # Rip out text frame from tixScrolledWindow widget and insert
  # our frame instead. 
  catch "destroy $w.$tool"

  pack [frame $w.$tool] -fill x -anchor w
  
  foreach text [set options`buttons($tool)] {
    set but $w.$tool.[createFuncName $text]
    button $but  -text [capitalize $text] \
        -command [set options`name2cmd($tool,$text)] 
    setSN $tool`button`top`$text $but
    # -font {-*-helvetica-bold-r-*-*-12-*-*-*-*-*-*-*}
    pack $but -side left
    help`add $but ${tool}Page`[createFuncName $text]
  }
  
  ###### hiding the other mode
  ###if {$MODE == "maintool"} {
  ###  pack forget $w.debug
  ###} else {
  ###  pack forget $w.maintool
  ###}
}

############################################################
# These two functions enables/disables the line of buttons
############################################################
proc buttons`enableLine {} {
  pack .buttons -after .menuline -fill x
}
proc buttons`disableLine {} {
  pack forget .buttons
}

proc buttons`setState {text state} {
  global options`buttons SN
  foreach tool {maintool interptool} {
    if {[lsearch -exact [set options`buttons($tool)] $text] != -1} {
      $SN($tool`buttons`top).$tool.[createFuncName $text] configure \
          -state $state
    }
  }
}

proc buttons`changeInterptoolState {state} {
  global SN options`buttons
  set texts [set options`buttons(interptool)] 
  foreach text $texts {
    set but $SN(interptool`button`top`$text)
    if  {$state == "enable"} {
      if {$text == "stop interpreter"} {
        $but configure -state disabled
      } else {
        $but configure -state normal
      }
    } else {
      if {$text == "stop interpreter"} {
        $but configure -state normal
      } else {
        $but configure -state disabled
      }
    }
  }
}
