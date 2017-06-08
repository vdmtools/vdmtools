############################################################
# This function pack the debug_editor frame
# w - the window to pack in
############################################################

class Viewer {

  inherit FrameWin
  
  variable text
  variable textw
  variable editor_name
  variable ChangeState 1
  
  constructor {name title} {
    FrameWin::constructor $name $title;
  } {
    set editor_name $name
  }

  variable editor_state
  method Pack {w}
  method save {} {}
  method load {} {}
  method print {} {}
  method SetChangeState {on} { set ChangeState $on }
  method setstatus {value} {
    if ($ChangeState) {
      $textw configure -state $value
    }
  }
  method pipe {} {}
  method reset {} {}
  method edit {} {}
  method fileopen {file} {}
  method goto {file line col} {}
  method mark {file lclist} {}
  method getFileName {} {}
  method set_edit_button {} {}
  method enabled {} {}
  method break {x y} {}
  
}

body Viewer::break {x y} {
  set pos [$textw index @$x,$y]
  regexp {([0-9]+).([0-9]+)} $pos all line col
  debug`menucmd "break [ tkif_GetRealFileName [ getFileName ] ] $line [expr $col+1]"
}


body Viewer::Pack w {
  global SN
  setSN $editor_name`top $w
  help`add $w anyPage`$editor_name
  general`packLabel $w $editor_name
  set text [tixScrolledText $w.text -scrollbar auto]
  pack $text -fill both -expand 1
  bind $text <Enter> "maybe_setFocus $text"
  
  setSN $editor_name`text [$text subwidget text]
  set textw [$text subwidget text]

  bind $textw <Double-1> "$editor_name break %x %y; break"

  AddFocusWin $textw

  ## set m [ menu $textw.menu ]
  ## $m add command -label "Print" -command {puts "Printing."}
  ## $m add command -label "Pipe" -command {puts "Pipe."}
  ## bind $textw <3> "tk_popup $m %X %Y"
  
  setstatus disabled
}

body Viewer::save {} {
  general`saveText $editor_name
}

body Viewer::load {} {
  general`loadText $editor_name
}

############################################################
# These two function print/pipe the content of the
# 'debug_editor' buffer
############################################################

body Viewer::print {} {
  general`printText $editor_name
}
body Viewer::pipe {} {
  general`pipeText $editor_name
}

body Viewer::reset {} {
  global SN
  $SN($editor_name`extraInfo) configure -text ""
  setstatus normal
  $textw delete 0.0 end
  setstatus disabled
  catch {unset editor_state}
  set_edit_button
}

############################################################
# This function start an external debug_editor with the
# file in the debug_editor window, and located on the line in
# the debug_editor window
# return - nothing
############################################################
body Viewer::edit {} {
  set pos [ $textw index insert ]
  regexp {([0-9]+)\.([0-9]+)} $pos dummy line col
  set rfn [ tkif_GetRealFileName [ getFileName ] ]
  general`startEditorAtPos  $rfn  $line 
#  general`startEditorAtPos [ getFileName ]  $line 
}

############################################################
# Open file. 
############################################################
body Viewer::fileopen {file} {
  global SN

  $SN($editor_name`extraInfo) configure -text "([ tkif_GetRealFileName $file])"
  if {[info exists editor_state(filename)] && \
      ($editor_state(filename) == $file) && \
      [info exists editor_state(mtime)] && \
      ($editor_state(mtime) == [file mtime $editor_state(filename)]) } { 
  } else {
    set editor_state(filename) ""
    if { [ catch {set editor_state(fileid) [ open $file "r" ]} OK ] } {
    } else {
      set editor_state(filename) $file
      set editor_state(mtime) [file mtime $file]
      setstatus normal
      $textw delete 1.0 end
      $textw insert insert [ read $editor_state(fileid) ]
      setstatus disabled
      ::close $editor_state(fileid) 
    }
  }
  set_edit_button 
}

body Viewer::goto {file line col} {
  global SN
  fileopen $file

  setstatus normal
  
  set ranges [ $textw tag ranges highlightError ]
  if {[ llength $ranges ] > 0} {
    $textw delete highlightError.first highlightError.last
  }
  $textw tag delete highlightError 
  $textw mark set insert $line.$col
  $textw insert insert " " highlightError
  if {[ Config isMonochome ]} {
    $textw tag configure highlightError -background black 
  } else {
    $textw tag configure highlightError -background red
  }
  $textw see $line.$col

  setstatus disabled
}

body Viewer::mark {file lclist} {
  global SN
  fileopen $file

  setstatus normal
  
  $textw tag delete highlightMarks
  while {[llength $lclist]>1} {
    set line [ lindex $lclist 0 ]
    set col  [ lindex $lclist 1 ]
    incr line 1
    set lclist [ lreplace $lclist 0 1 ]
    $textw tag add highlightMarks $line.$col
  }  
  if {[ Config isMonochome ]} {
    $textw tag configure highlightMarks -underline true 
  } else {
    $textw tag configure highlightMarks -background red
  }

  setstatus disabled
}

body Viewer::getFileName {} {
  if {[info exists editor_state(filename)]} {
    return $editor_state(filename)
  } else {
    return "??"
  }
}

body Viewer::set_edit_button {}  {
  functions`setWindowButtonState $editor_name edit [ enabled ]
}

body Viewer::enabled {} {
  if {[info exists editor_state(filename)]} {
    return "normal"
  } else {
    return "disabled"
  }
}

######################################################################

proc error_editor`pack w  { error_editor Pack $w }

proc error_editor`save {} { error_editor save } 

proc error_editor`load {} { error_editor load }

proc error_editor`print {} { error_editor print }

proc error_editor`pipe {} { error_editor pipe }

proc error_editor`reset {} { error_editor reset }

proc error_editor`edit {} { error_editor edit }

proc error_editor`open {file} { error_editor open $file }

proc error_editor`goto {file line col} { error_editor goto $file $line $col }

proc error_editor`getFileName {} { return [ error_editor getFileName ] }

proc error_editor`set_edit_button {}  { error_editor set_edit_button }

######################################################################


######################################################################


proc debug_editor`pack w {
  debug_editor Pack $w
}

proc debug_editor`save {} {
  debug_editor save 
}

proc debug_editor`load {} {
  debug_editor load 
}

proc debug_editor`print {} {
  debug_editor print 
}

proc debug_editor`pipe {} {
  debug_editor pipe
}

proc debug_editor`edit {} {
  debug_editor edit
}



proc debug_editor`enableWindow {} {
  general`ChangeLabelState debug_editor normal
}

proc debug_editor`disableWindow {} {
  general`ChangeLabelState debug_editor disabled
}


