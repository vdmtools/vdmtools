############################################################
#      This file contain the functions to convert a
#      html file to the format required by Tk in the
#      help pages.
############################################################

############################################################
# Standard definitions
############################################################
set unaryTokens {
  hr br li dt dd p
  img
}

set binaryTokens {
  h1 h2 h3 h4 h5 h6
  b i u s
  head body title html address
  ul ol dl
}

set defaultSize 14
set headerSize(h1) 30
set headerSize(h2) 25
set headerSize(h3) 20
set headerSize(h4) 18
set headerSize(h5) 16
set headerSize(h6) 15
set headerSize(h7) $defaultSize; # a programming hack, to make it easyer
# to create original text which is italic and bold

############################################################
# sets the global setting
############################################################
set size $defaultSize
set bold "medium"
set italic "r"
set imgNo 0

############################################################
# Simple stack operations
############################################################
proc push {elm name} {
  upvar \#0 $name stack
  lappend stack $elm
}
proc pop {name} {
  upvar \#0 $name stack
  set elm [lindex $stack end]
  set stack [lrange $stack 0 [expr [llength $stack]-2]]
  return $elm
}
proc emptyStack {name} {
  upvar \#0 $name stack
  set stack ""
}
proc isEmptyStack {name} {
  upvar \#0 $name stack
  return [expr ![llength $stack]]
}
proc printStack {name} {
  upvar \#0 $name stack
  return $stack
}

############################################################
# Read the input file
# IN       - A file identifyer for the input file
# return   - a list where the first element is a text
#            then every second element is a token, and every
#            second is a text
############################################################
proc readFile {IN} {
  set result {}
  
  set FILE [read -nonewline $IN]
  set length [string length $FILE]
  set state text; # what is the next to read? text or token
  set text ""
  set line 1; # These two are used for error messages
  set pos 0;  # when "parsing" the input
  emptyStack tokenStack
  set lastspace 0
  
  
  for {set index 0} {$index < $length} {incr index} {
    set char [string index $FILE $index]
    if {$char == "\n"} {
      if {!$lastspace} {
	append text " "
	set lastspace 1
      }
      incr line
      set pos 0
      continue
    }
    if {$char == "\t" || $char == " "} {
      if {!$lastspace} {
	append text " "
	set lastspace 1
      }
      incr pos
      continue
    }
    incr pos
    set lastspace 0
    
    if {$state == "text"} {
      if {$char != "<"} {
	append text $char
      } else {
	# end of text, save it
	if {! [regexp "^\[ \t\]+\$" $text]} {
	  lappend result \
	      [string trimright [string trimleft $text " \t\n"] " \t\n"]
	} else {
	  lappend result ""
	}
	set text ""
	set state token
      }
      
    } else { # reading a token
      if {$char != ">"} {
	append text $char
      } else {
	# end of token, verify and save
	
	if {[string index $text 0] == "/"} {
	  # end token
	  set token [string range $text 1 end]
	  set match [lindex [pop tokenStack] 0]
	  if {[string tolower $match] !=  [string tolower $token]} {
	    error "line $line, pos $pos: \"$token\" doesn't match \"$match\""
	  }
	  lappend result $text
	} else {  # tsrat token
	  if {[unaryToken $text]} {
	    lappend result [list $text {}]
	  } elseif {[binaryToken $text]} {
	    push [list $text {}] tokenStack
	    lappend result [list $text {}]
	  } else {
	    set tokenl [getOtherToken $text]
	    if {$tokenl == -1} {
	      error "line $line, pos $pos: unknown token: \"$text\""
	    } else {
	      # token with argument
              set type [lindex $tokenl 0]
              if {$type == "binary"} {
		push [lrange $tokenl 1 end] tokenStack
	      }
	      lappend result [lrange $tokenl 1 end]
 	    }
	  }
	}
	set text ""
	set state text
      }
    }
  }
  if {![isEmptyStack tokenStack]} {
    error "Unmatch token, at end of file\nNon terminated tokens:\n[printStack tokenStack]"
  }
  return $result
}
  
############################################################
#                 Token Functions
############################################################
proc unaryToken {token} {
  global unaryTokens
  set token [string tolower $token]
  if {[lsearch -exact $unaryTokens $token] != -1} {
    return 1
  } else {
    return 0
  }
}

proc binaryToken {token} {
  global binaryTokens
  set token [string tolower $token]
   if {[lsearch -exact $binaryTokens $token] != -1} {
    return 1
  } else {
    return 0
  }
}

proc getOtherToken {token} {
  if {[regexp -nocase {^a *href *= *"([^"]+)" *$} $token all value]} {
    return [list binary a href $value]
  } elseif {[regexp -nocase {^a *name *= *"([^"]+)" *$} $token all value]} {
    return [list binary a name $value]
  } elseif {[regexp -nocase {^img src *= *"([^"]+)" *$} $token all value]} {
    return [list unary img $value]
  } elseif {[regexp {^!--(.*)--$} $token all value]} {
    # The token is a comment.
    # This part of the script ought not to translate this information
    # but to make things simple, it will translate this to nothing
    # The reason for the complexity is that this is an unary token, while
    # this section only handles binary tokens, and at the moment unary tokens
    # doesn't take any argument
    return {unary}
  } else {
    return -1
  }
}

############################################################
# This functions translate the special character
# sequence in text. Ie '&gt;' -> '>'
# list    - the list to translate
# returns - the translated text
############################################################
proc translate {list} {
  set result {}
  set index -1
  foreach elm $list {
    incr index
    if {$index %2 == 0} {
      # text
      regsub -all -- {&gt;} $elm ">" elm
      regsub -all -- {&lt;} $elm "<" elm
      regsub -all -- {&amp;} $elm {\&} elm
      regsub -all -- {&quot;} $elm "\"" elm
      regsub -all -- {&aelig;} $elm "æ" elm
      regsub -all -- {&AElig;} $elm "Æ" elm
      regsub -all -- {&oslash;} $elm "ø" elm
      regsub -all -- {&Oslash;} $elm "Ø" elm
      regsub -all -- {&aring;} $elm "å" elm
      regsub -all -- {&Aring;} $elm "Å" elm
    }
    lappend result $elm
  }
  return $result
}

############################################################
# Write the format the help page need in TK.
# list - the list read with readFile
# outfile - the file to write to
############################################################
proc convert {list} {
  global wantSpace size bold italic headerSize defaultSize first indentlevel last
  
  writeStdDef

  set state text;    # what to read next, text or token
  set wantSpace 0;   # controles when a space delimeter shall be inserted
  set first 1;       # flag indicating when the first text is wrote
  set indentlevel 0; # the number of nested indentation
  for {set i 1} {$i <= 6} {incr i} {
    set headerIndex($i) 0
  }
  set last 1.0
  
  emptyStack tokenStack
  
  foreach elm $list {
    if {$state == "text"} {
      if {$elm != ""} {
	.text insert insert "$elm"
	set wantSpace 1
	set first 0
      }
      set state token

    } else {   #state == token
      set token [lindex $elm 0]
      if {[string index $token 0] != "/"} {
	# start token
	if {$wantSpace} {
	  .text insert insert " "
	  set wantSpace 0
	}
	set tok [string tolower $token]
	if {$tok == "h1" || $tok == "h2" || $tok == "h3" ||
	    $tok == "h4" || $tok == "h5" || $tok == "h6" ||
	    $tok == "i"  || $tok == "b"} {
	  # special care must be taken for these, since they all manipulate
	  # the font
	  .text tag add $italic$bold$size $last insert
	  switch -exact -- $tok {
	    h1 -
	    h2 -
	    h3 -
	    h4 -
	    h5 -
	    h6 {
	      set size $headerSize($tok)
	      if {!$first} {
		.text insert insert \n\n
		set wantSpace 0
	      }
	      set last [.text index insert]
	      set text ""
	      set index [string index $tok 1]
	      incr headerIndex($index)
	      for {set i 1} {$i <= $index} {incr i} {
		append text $headerIndex($i) .
	      }
	      for {set i [expr $index +1]} {$i <= 6} {incr i} {
		set headerIndex($i) 0
	      }
	      .text insert insert "$text  "
	    }
	    i {
	      set italic o
	      set last [.text index insert]
	    }
	    b {
	      set bold bold
	      set last [.text index insert]
	    }
	  }
	}

	if {$tok == "ol" || $tok == "ul" || $tok == "dl"} {
	  push [list $tok 1] indentStack
	  incr indentlevel
	}

	set value [lrange $elm 1 end]
	if {[unaryToken $token]} {
	  $tok $value
	} else {
	  push [list [.text index insert] $value] tokenStack
	}
      } else {
	# end token
	set token [string range $token 1 end]
	set tok [string tolower $token]
	if {$tok == "h1" || $tok == "h2" || $tok == "h3" ||
	    $tok == "h4" || $tok == "h5" || $tok == "h6" ||
	    $tok == "i" || $tok == "b"} {
	  # special care must be takn for these since they
	  # all manipulate the font
	  .text tag add $italic$bold$size $last insert
	  set last [.text index insert]
	  switch -exact -- $tok {
	    h1 -
	    h2 -
	    h3 -
	    h4 -
	    h5 -
	    h6 {set size $defaultSize}
	    i {set italic r}
	    b {set bold medium}
	  }
	}
	
	set token_val [pop tokenStack]
	set start [lindex $token_val 0]
	set value [lindex $token_val 1]

	# call the apropriate html-print function
	$tok $start $value
	if {$wantSpace} {
	  .text insert insert " "
	  set wantSpace 0
	}

	if {$tok == "ol" || $tok == "ul" || $tok == "dl"} {
	  pop indentStack
	  incr indentlevel -1
	}

      }
      set state text
    }
  }
}

############################################################
# Write standard tags at the beginning
# OUT - the filehandler to write to
############################################################
proc writeStdDef {} {
  global headerSize

  foreach header {h1 h2 h3 h4 h5 h6 h7} {
    foreach italic {o r} {
      foreach bold {bold medium} {
	.text tag configure $italic$bold$headerSize($header) -font "-*-helvetica-${bold}-${italic}-*-*-$headerSize($header)-*-*-*-*-*-*-*"
      }
    }
  }
  
  .text tag configure strike -overstrike 1
  .text tag configure underline -underline 1

  for {set i 0} {$i < 10} {incr i} {
    .text tag configure indent$i -lmargin2 [expr 25*$i] \
	-lmargin1 [expr 25*($i-1)]
  }
}

############################################################
#              Html-print functions
# These function takke care of transforming the tokens
# into the Tk format
############################################################
proc hr {value} {
  global wantSpace
  .text insert insert "\n------------------------\n"
  set wantSpace 0
}

proc br {value} {
  global wantSpace
  .text insert insert \n
  set wantSpace 0
}

proc p {valeu} {
  global wantSpace
  .text insert insert \n\n
  set wantSpace 0
}

proc li {value} {
  global indentlevel
  set elm [pop indentStack]
  set type [lindex $elm 0]
  if {$type == "ul"} {
    set char [lindex {* - o} [expr $indentlevel % 3]]
    .text insert insert "\n$char  "
    push $elm indentStack
  } else {
    set indent [lindex $elm 1]
    .text insert insert "\n$indent)  "
    push [list ol [expr $indent+1]] indentStack
  }
}

proc img {value} {
  global imgNo images
  incr imgNo
  label .text.img$imgNo -bitmap @$value
  .text window create insert -window .text.img$imgNo
  lappend images [list $value [.text index insert]]
}

proc html {start value} {
  # Do Nothing
}

proc body {start value} {
  # Do Nothing
}

proc title {start value} {
  # Do Nothing
}

proc head {start value} {
  # remove the header
  global wantSpace first
  .text delete $start insert
  set wantSpace 0
  set first 1
}

proc address {start value} {
  .text delete $start [.text index insert]
  set wantSpace 0
}

foreach header {h1 h2 h3 h4 h5 h6} {
  proc $header {start value} "header \$start $header "
}

proc header {start header} {
  global wantSpace help`headers
  lappend help`headers "{[.text get $start insert]} $header $start"
  .text insert insert \n
  set wantSpace 0
  # DO nothing else, since this manipulate the font
}
 
proc b {start value} {
  # DO nothing, since this manipulate the font
}

proc i {start value} {
  # DO nothing, since this manipulate the font
}

proc u {start value} {
  .text tag add underline $start insert
}

proc s {start value} {
  .text tag add strike $start insert
}

proc ol {start value} {
  global indentlevel wantSpace
  .text tag add indent$indentlevel $start insert
  if {$indentlevel == 1} {
    .text insert insert \n
    set wantSpace 0
  }
}

proc ul {start value} {
  global indentlevel wantSpace
  .text tag add indent$indentlevel $start insert
  if {$indentlevel == 1} {
    .text insert insert \n
    set wantSpace 0
  }
}

proc dl {start value} {
  global indentlevel wantSpace
  .text tag add indent$indentlevel $start insert
  if {$indentlevel == 1} {
    .text insert insert \n
    set wantSpace 0
  }
}

proc dt {value} {
  global italic bold size last
  .text tag add $italic$bold$size $last insert
  set last [.text index insert]
  set bold bold
  .text insert insert \n
}
proc dd {value} {
  global italic bold size last
  .text tag add $italic$bold$size $last insert
  set last [.text index insert]
  set bold medium
  .text insert insert "  "
}

proc a {start value} {
  global bold size help`ref hyperref
  set type [lindex $value 0]
  set val [lindex $value 1]
  if {$type == "name"} {
    set help`ref($val) $start
  } else { #type = href
    if {[string index $val 0] == "\#"} {
      lappend hyperref [list $start [.text index insert]\
			    [string range $val 1 end]]
    } else {
      .text insert insert " ($val)"
    }
  }
}

############################################################
# This function write the text widget to a file
############################################################
proc exportWidget file {
  global help`headers help`ref hyperref images
  set OUT [open $file w]

  puts $OUT "\#\# This file has been auto generated from makeman.tcl"
  puts $OUT "\#\# on date [exec date]"
  puts $OUT "\#\#\n\#\#\n"
  puts $OUT "proc createHelpPage \{prefix\} \{"
  puts $OUT "\tglobal help`ref help`headers"

  puts $OUT "set help`headers {${help`headers}}"
  foreach key [array names help`ref] {
    puts $OUT "set help`ref($key) {[set help`ref($key)]}"
  }
  puts $OUT "\$prefix insert end {[.text get 1.0 end]}"
  
  puts $OUT "global BITMAPDIR"
  ### writing the images
  set imgNo 0
  foreach img $images {
    set value [lindex $img 0]
    set index [lindex $img 1]
    incr imgNo
    regsub {bitmaps/} $value {bm`} value 
    puts $OUT "label \$prefix.img$imgNo -bitmap $value"
    puts $OUT "\$prefix window create $index -window \$prefix.img$imgNo"
  }

  ### writing the tags
  foreach tag [.text tag names] {
    set ranges [.text tag ranges $tag]
    if {$ranges != {}} {
      foreach conf [.text tag configure $tag] {
	if {[lindex $conf 4] != {}} {
	  puts $OUT "\$prefix tag configure $tag [lindex $conf 0] {[lindex $conf 4]}"
	}
      }
      puts $OUT "\$prefix tag add $tag $ranges"
    }
  }

  set i 0
  foreach ref $hyperref {
    set start [lindex $ref 0]
    set end [lindex $ref 1]
    set tag [lindex $ref 2]
    puts $OUT "\$prefix tag add tag$i $start $end"
    puts $OUT "\$prefix tag configure tag$i -underline 1"
    puts $OUT "\$prefix tag bind tag$i <1> \"help`gotoTag $tag\""
    incr i
  }

  puts $OUT "\}"
  close $OUT
}

############################################################
# main procedure.
# arguments: infile [outfile]
############################################################
if {[llength $argv] < 1} {
  puts "Syntax $argv0 infile \[outfile\]"
  exit 1
}

set infile [lindex $argv 0]
if {[llength $argv] == 1} {
  set outfile help.input
} else {
  set outfile [lindex $argv 1]
}

set hyperref {}
set images {}
set IN [open $infile r]
set list [readFile $IN]
set newlist [translate $list]
set t [text .text]
wm geometry . "600x300+0+0"
pack $t
convert $newlist
close $IN
exportWidget $outfile
# update idletasks
update
after 10 "destroy ."
