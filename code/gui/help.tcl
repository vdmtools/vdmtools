######################################################################
# Help Window. 
#
# $Id: help.tcl,v 1.22 2000/03/13 15:47:00 erik Exp $
######################################################################

set help`loading 0
############################################################
# Create the help page
# arguments - none
# return    - nothing
############################################################
proc help`helpOnHelp {} {
  global help`headers help`history help`history_index BITMAPDIR help`loading

  if {[winfo exist .manpage]} {
    set w .manpage
    if {[wm state $w] != "normal"} {
      global windows`geometry
      if {[info exists windows`geometry($w) ]} {
        set size [set windows`geometry($w) ]      
        wm geometry $w $size
        # puts "setting geo $w $size"
      } 
      wm deiconify .manpage
    }
    raise .manpage
    return
  }

  if {${help`loading}} {
    return
  } else {
    set help`loading 1
  }
  toplevel .manpage 
  wm title .manpage "Help"
  wm protocol .manpage WM_DELETE_WINDOW { wm withdraw .manpage; puts "withd" }
  ### Create the paned window
  tixPanedWindow .manpage.paned -orient horizontal
  set w .manpage.paned
  pack $w -expand 1 -fill both

  set index [$w add index -size 250]
  set help [$w add help -size 500]

  ### create the text window
  pack [tixScrolledText $help.txt -scrollbar auto] -fill both -expand 1
  set prefix [$help.txt subwidget text]
  $prefix configure -wrap word -cursor arrow
  setSN help`text $prefix
  
  ### source the man definition
  # SHOULD THIS BE DELETED.
  source help.input
  createHelpPage $prefix
  $prefix configure -state disabled

  ### Creating the help index
  tixScrolledListBox $index.box -scrollbar auto
  pack $index.box -fill both -expand 1

  set listbox [$index.box subwidget listbox]
  $listbox configure -selectmode single
  bind $listbox <1> help`indexBrowse
  bind $listbox <Double-1> { # Nothing } 
  
  # The selection should be set before our binding is called. 
  bindtags $listbox "Listbox $listbox"
  setSN help`listbox $listbox

  ### insert elements into the index list
  foreach elm ${help`headers} {
    set text [lindex $elm 0]
    set header [lindex $elm 1]
    set location [lindex $elm 2]
    set level [string index $header 1]
    set listboxtext [string range "                                         " \
			 0 [expr ($level-1)*5]]
    append listboxtext $text
    $listbox insert end $listboxtext
  }

  ### create the buttons's
  pack [frame .manpage.buttons] -fill x
  button .manpage.buttons.back -bitmap bm`Left -command help`back
  button .manpage.buttons.forward -bitmap bm`Right \
      -command help`forward 
  button .manpage.buttons.ok  -text "Close" -command {
    wm iconify .manpage
  }
  frame .manpage.buttons.dummy
  pack .manpage.buttons.ok -side left
  pack .manpage.buttons.dummy -padx 40 -side left
  pack .manpage.buttons.back .manpage.buttons.forward \
      -side left -padx 10
  .manpage.buttons.forward configure -state disabled
  .manpage.buttons.back configure -state disabled
  setSN help`back .manpage.buttons.back
  setSN help`forward .manpage.buttons.forward

  ### create the search entry
  label .manpage.buttons.label -text "Search String: "
  entry .manpage.buttons.search -textvariable help`search 
  button .manpage.buttons.up -bitmap bm`Up \
      -command "help`search backwards"
  button .manpage.buttons.down -bitmap bm`Down \
      -command "help`search forwards"
  bind .manpage.buttons.search <Return> "help`search forwards"
  bind .manpage.buttons.search <KP_Enter> help`search
  pack .manpage.buttons.down .manpage.buttons.up .manpage.buttons.search .manpage.buttons.label   -side right -padx 10

  ### create a handler for window delete
  wm protocol .manpage WM_DELETE_WINDOW {
    wm withdraw .manpage
  }

  ### set up the hypertext history
  set help`history_index 0
  set help`history {}

  set help`loading 0
}

############################################################
# Change the view in the textbox due to a
# selection in the listbox
# return  - nothing
############################################################
proc help`indexBrowse {} {
  global help`headers help`history help`history_index SN
  set listbox $SN(help`listbox)
  set text $SN(help`text)
  set back $SN(help`back)
  set forward $SN(help`forward)
  
  # add the element to the history
  set help`history [lrange ${help`history} 0 [expr ${help`history_index}-1]]
  lappend help`history [lindex [$text yview] 0]
  incr help`history_index
  $back configure -state normal
  $forward configure -state disabled

  set index [lindex [lindex ${help`headers} [$listbox curselection]] 2]
  $text yview $index

  # remove the selection from the listbox
  $listbox selection clear 0 end
}

############################################################
# goto a tag which have been reference with href in the
# html code.
# tag    - the name of the tag
# return - nothing
############################################################
proc help`gotoTag {tag} {
  global help`ref help`history help`history_index SN help`loading

  # puts "gototag $tag"

  # test wether an other help search is in progress
  if {${help`loading}} return
  
  # open the help page
  help`helpOnHelp

  set text $SN(help`text)
  set back $SN(help`back)
  set forward $SN(help`forward)
  
  # add the element to the history
  set help`history [lrange ${help`history} 0 [expr ${help`history_index}-1]]
  lappend help`history [lindex [$text yview] 0]
  incr help`history_index
  $back configure -state normal
  $forward configure -state disabled

  if {[info exists help`ref($tag)]} {
    set index [set help`ref($tag)]
  } elseif {[info exists help`ref(unknown)]} {
    set index [set help`ref(unknown)]
  } else {
    set index 0.0
  }
  $text yview $index
}
  
############################################################
# go forward in the "help path" (like netscape)
# return - nothing
############################################################
proc help`forward {} {
  global help`history help`history_index SN
  set back $SN(help`back)
  set forward $SN(help`forward)
  set text $SN(help`text)

  # changeing the current location in the history list
  set index ${help`history_index}
  set help`history [concat [lrange ${help`history} 0 [expr $index-1]]\
		    [lindex [$text yview] 0]\
		    [lrange ${help`history} [expr $index+1] end]]
  
  incr help`history_index
  set fraction [lindex ${help`history} ${help`history_index}]
  if {${help`history_index} == [llength ${help`history}]-1} {
    $forward configure -state disabled
  }
  $back configure -state normal
  $text yview moveto $fraction
}

############################################################
# go back in the "help path" (like netscape)
# return - nothing
############################################################
proc help`back {} {
  global help`history help`history_index SN
  set back $SN(help`back)
  set forward $SN(help`forward)
  set text $SN(help`text)

  # changeing the current location in the history list
  set index ${help`history_index}
  set help`history [concat [lrange ${help`history} 0 [expr $index-1]]\
		    [lindex [$text yview] 0]\
		    [lrange ${help`history} [expr $index+1] end]]

  incr help`history_index -1
  set fraction [lindex ${help`history} ${help`history_index}]
  if {${help`history_index} == 0} {
    $back configure -state disabled
  }
  $forward configure -state normal
  $text yview moveto $fraction
}

############################################################
# Setup the help text
# path   - the widget path, which the help applys to
# token  - the token, which identifies the help text
# return - nothing
############################################################
proc help`add {path token} {
  global help`helpindex
  set help`helpindex($path) $token
}

proc help_dumpindex {} {
  global help`helpindex
  puts stderr "Writing file tclhelpref."
  set reffile [ open tclhelpref w ]
  foreach p [ array names help`helpindex ] {
    puts $reffile "[ set help`helpindex($p) ] $p"
  }
}

############################################################
# Open the help page, and go to the location where token is
# defined
# path   - the path, which help shal be found on
# return - nothing
############################################################
proc help`lookUp {path} {
  global help`helpindex

  # try to match the name with something in the helpindex
  # until it matches
  # puts "lookup $path"
  for {set i [string length $path]} {$i > 0} {incr i -1} {
    set str [string range $path 0 [expr $i-1]]
    if {[info exists help`helpindex($str)]} {
      set tag [set help`helpindex($str)]
      break
    }
  }
  if {$i == 0} {
    runtime`internalError "Lookup couldn't match string \"$path\""
  }

  help`gotoTag $tag
}

############################################################
# This function search for the text typed in the label entry
# direction - forwards or backwards
############################################################
proc help`search {direction {start ""} } {
  global help`search SN help`history help`history_index
  set text $SN(help`text)
  set back $SN(help`back)
  set forward $SN(help`forward)

  $text tag delete help_search
  if {${help`search} == ""} return

  if {$direction == "forwards"} {
    set end end
    set op +
  } else {
    set end 1.0
    set op -
  }

  ### if start is not given we calculate it from the offset in the document.
  ### This is a bit tricky since we cant get the index of the first character
  ### of the page, but only which fraction of the whole document this
  ### character is located. So from the total amount of lines and this
  ### fraction we try to calculate the index. Rounding errors force us
  ### to add 2 lines to this calculated index, to avoid infiniti loop,
  ### due to that it always find the same text
  if {$start == ""} {
    set totalNoOfLines [lindex [$text index end] 0]
    set indexStartOfPage [lindex [$text yview] 0]
    set start [$text index \
		   "[expr int($totalNoOfLines * $indexStartOfPage)].0 $op 2l"]
  }
  set yview [$text yview]
  

  ### color all matches
  set index [$text search -nocase -count count -- ${help`search} 1.0]
  while {$index  != ""} {
    if {$count == 0} {
      runtime`userError "The pattern match the empty string" \
	  help_search_empty_string
    }
    set next [$text index $index+${count}c]
    $text tag add help_search $index $next
    set index [$text search -count count -nocase -forwards\
		   -- ${help`search} $next end]
  }

  ### find the first match from the ofset given
  $text tag configure help_search -foreground white -background black
  
  set first [$text search -$direction -nocase -- \
		 ${help`search} $start $end]

  if {$first != ""} {
    $text yview $first
  }

  ### test if the view has changed, if not it means that we are at the
  ### bottom of the document
  if {$first != "" && $yview != [$text yview] } {
    # add the element to the history
    set help`history [lrange ${help`history} 0 [expr ${help`history_index}-1]]
    lappend help`history [lindex [$text yview] 0]
    incr help`history_index
    $back configure -state normal
    $forward configure -state disabled
    $text yview $first    
  } else {
    if {$direction == "forwards"} {
      set new_start 1.0
      set new_text beginning
    } else {
      set new_start end
      set new_text end
    }
    set answer [tk_dialog .notfound "Text not found" "Text not found. Start from the $new_text ?" questhead 0 Yes No]
    if {$answer == 0} {
      help`search $direction $new_start
    }
  }
}

############################################################
# This function shows the about page
############################################################
proc help`about {} {
  global BITMAPDIR
  if {[winfo exists .about]} {
    wm deiconify .about
    return
  }
  
  toplevel .about
  wm title .about "About"
  
  label .about.header -text "[tkif_GiveToolTitle]\n[tkif_GiveToolVersion]" \
      -font "-*-helvetica-medium-r-*-*-20-*-*-*-*-*-*-*"
  pack .about.header -pady 15
  label .about.logo -bitmap bm`toolbox_logo
  pack .about.logo -anchor c -padx 3c

  set features [frame .about.features]
  pack $features -pady 10

  pack [button .about.ok -text Ok -command "wm withdraw .about"] -anchor e -expand 1
}
