############################################################
# This is the procedure which takes care of user error
# text   - a text which shortly describe the error
# tag    - the tag in the help, where this error is described.
# return - never
############################################################
proc runtime`userError {text tag} {
  error [list userError $text $tag]
}

############################################################
# This is the procedure which takes care of internal errors
# if any exists ;-)
# text   - a short text which describe the error
# return - never
############################################################
proc runtime`internalError {text} {
  error [list internalError $text]
}

############################################################
# This is the function Tk call when an background error
# occur. I had to define this myself, so that internal-
# and user errors can unwind and still give special
# help for the user.
# err    - for Tk error's this is a help description
#          for internal- and user error this is a list
#          of description and help tag.
# return - never
############################################################
proc tkerror err {
  global errorInfo
  set info $errorInfo

  catch {windows`Busy off}
  # test to see if the error is a user error, an internal error or 
  # an error from tk
  if {[lindex [split $err " "] 0] == "userError"} {
    set error [lindex $err 1]
    set tag [lindex $err 2]
    if {$tag != ""} {
      set button [tk_dialog .tkerrorDialog "User Error" \
          $error error 1 Help OK]
      if {$button == 0} {
        runtime`processErr help`gotoTag $tag
      }
    } else {
      set button [tk_dialog .tkerrorDialog "User Error" $error error 0 OK]    
    }      
  } elseif {[lindex [split $err " "] 0] == "internalError"} {
    set error [lindex $err 1]
    set button [tk_dialog .tkerrorDialog "Internal Error" \
		    "An internal error occured: $error" error 0 \
		    "OK" "See Stack Trace"]
    if {$button == 1} {
      runtime`processErr runtime`showStackTrace $info
    }
  } else {
    set button [tk_dialog .tkerrorDialog "TK Error" \
		    "An error occured in Tcl/Tk: $err" error 0 \
		    "OK" "See Stack Trace"]
    if {$button == 1} {
      runtime`processErr runtime`showStackTrace $info 
    }
  }
}

############################################################
# This is the function which call a function from within
# the tk_error function. Special care must be taken, so that
# tk will not grab the error and report it, if it occurs
# in a call from tk_error
# func   - the function to call
# args   - the arguments to the function
# return - nothing
############################################################
proc runtime`processErr {func args} {
  if {[catch "$func $args" err]} {
    tkerror $err
  }
}

############################################################
# This function show the stack trace if an error had occur
# info   - the stack trace
# return - nothing
############################################################
proc runtime`showStackTrace {info} {
  set w .tkerrorTrace
  catch "destroy $w"
   toplevel $w -class ErrorTrace
  wm minsize $w 1 1
  wm title $w "Stack Trace for Error"
  wm iconname $w "Stack Trace"
  button $w.ok -text OK -command "destroy $w"
  text $w.text -relief sunken -bd 2 -yscrollcommand "$w.scroll set" \
      -setgrid true -width 60 -height 20
  scrollbar $w.scroll -relief sunken -command "$w.text yview"
  pack $w.ok -side bottom -padx 3m -pady 2m
  pack $w.scroll -side right -fill y
  pack $w.text -side left -expand yes -fill both
  $w.text insert 0.0 "An error occured while evaluating the following code.\nPlease send this in an email to toolbox@ifad.dk with a description\n\of what you did to trigger this bug. Thanks.\n$info"

  # Center the window on the screen.

  wm withdraw $w
  update idletasks
  set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
	     - [winfo vrootx [winfo parent $w]]]
  set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
	     - [winfo vrooty [winfo parent $w]]]
  wm geom $w +$x+$y
  wm deiconify $w

  # Be sure to release any grabs that might be present on the
  # screen, since they could make it impossible for the user
  # to interact with the stack trace.

  if {[grab current .] != ""} {
    grab release [grab current .]
  }
}

############################################################
# Report an error
# info   - the stack trace
# err    - the description of the error
# return - nothing
############################################################
proc runtime`sendBugReport {info err} {
}
