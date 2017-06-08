############################################################
# Capitalize a word
# word - the word to capitalize
############################################################
proc capitalize {words} {
  set result ""
  set space ""
  foreach word $words {
    append result $space
    append result [string toupper [string index $word 0]]
    append result [string range $word 1 end]
    set space " "
  }
  return $result
}

############################################################
# This will take a list of words, and create a legal
# function name from them: eg. "this is a test" => thisIsATest
############################################################
proc createFuncName {namel} {
  set result [string tolower [lindex $namel 0]]
  foreach name [lrange $namel 1 end] {
    append result [capitalize $name]
  }
  return $result
}

############################################################
# This function adds the symbolic name 'widget' to the
# list of path names with 'path' as the value
############################################################
proc setSN {widget path} {
  global SN
  set SN($widget) $path
}

############################################################
# This function works just like the ordanary Tcl lappend
# except that it only append the element if it doesn't
# exists in the list
# list   - the name of the list to append to
# val    - the value to append
# return - the list (tcl do so)
############################################################
proc uniqListAppend {list val} {
  upvar $list l
  if {[lsearch -exact $l $val] == -1} {
    lappend l $val
  }
  return $l
}

proc NYI {} {
  tk_dialog .dialog "Not Implemented Yet" "Not Implemented Yet" error 0 OK
}


proc lreverse list {
  set result ""
  for {set i [expr [llength $list]-1]} {$i >= 0} {incr i -1} {
    lappend result [lindex $list $i]
  }
  return $result
}
		    

############################################################
# This function search for an element in a list of lists
# elm - the element to search for
# return - the index as a list {x y}
############################################################
proc util`dlsearch {list elm} {
  set y -1
  foreach line $list {
    incr y
    set x -1
    foreach win $line {
      incr x
      if {$win == $elm} {
	return [list $x $y]
      }
    }
  }
}

############################################################
# float incr
############################################################
proc fincr {var value} {
  upvar $var variable
  set variable [expr $variable + $value]
}
