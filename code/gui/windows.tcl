
class FrameWin {
  constructor {n title} {
    set Name $n;
    setTitle $title
  }
  variable Name
  variable WPath
  variable closed 0 
  variable toplevel 0

  method setPath {w} {}
  method getPath {} {}

  variable Title 
  method getTitle {} {}
  method setTitle {t} {}

  method setFocus {} {}
  
  method close {} {}
  method isToplevel {} {}
  method addAsToplevel {} {}
  method removeAsToplevel {} {}
  method isClosed {} {}
  method addAsClosed {} {}
  method removeAsClosed {} {}
}

############################################################
# This function deside wether a frame is maped as a toplevel
# win - the name of the frame
# return - 1 iff the frame is  maped as a toplevel
#          0 iff the frame is not maped as a toplevel
############################################################

body FrameWin::getPath {} {
  return $WPath
}

body FrameWin::setPath {w} {
  set WPath $w
}

body FrameWin::setTitle {t} {
  set Title $t
}

body FrameWin::getTitle {} {
  return $Title
}

body FrameWin::setFocus {} {
  maybe_setFocus $WPath
}

body FrameWin::close {} {
  [ windows`gettoolroot $Name ] close $Name
}

body FrameWin::isToplevel {} {
  return $toplevel
}

############################################################
# This functions append a frame to the list of frames
# which are maped as toplevels
############################################################
body FrameWin::addAsToplevel {} {
  set toplevel 1 
}  

############################################################
# This function remove a frame from the list of frames
# which have been maped as toplevels
############################################################
body FrameWin::removeAsToplevel {} {
  set toplevel 0 
}

############################################################
# This function deside wether a frame is cloed or not
# win - the name of the frame
# return - 1 iff the frame is closed
#          0 iff the frame is not closed
############################################################
body FrameWin::isClosed {} {
  return $closed
}

############################################################
# This functions append a frame to the list of frames
# which is closed
############################################################
body FrameWin::addAsClosed {} {
  set closed 1
}  

############################################################
# This function remove a frame from the list of frames
# which have been maped as toplevels
############################################################
body FrameWin::removeAsClosed {} {
  set closed 0 
}  

class Layout {
  variable Windows 
  variable closed {}
  variable toplevels {}
  variable LineCounter 0
  variable Sequence  
  variable Topw
  variable packInfo
  variable line 
  variable geometry
  variable Size
  variable Title ""
  
  constructor {w conf title} {}
  method setTitle {title} {}
  method Pack {} {}
  method GetToplevel {} {}
  method VisibleInLine {win} {}
  method createToplevel {window} {}
  method close {window} {}
  method reopen {window} {}
  method insertToplevel {window} {}
  method getheight {win} {}
  method getfraction {win} {}
  method setfraction {win frac} {}
  method getlinename {win} {}
  method getframe {win} {}
  method setpackinfo {win lineName frame}
  method packInfo`clear {win} {}
  method insert {window new} {}
  method closeInLine {window keep} {}
  method paneResize {sizes} {}
  method move {window} {}
  method closeMove {window} {}
  method moveLeftRight {window direction} {}
  method moveUpDown {window direction} {}
  method removeWindowFromLine {winline win} {}
  method nextWindowIndex {offsetX offsetY direction} {}
  method unClosedLine {offsetY direction window} {}
  method insertWindowOnLine {winline window} {}
  method brothers {win} {}
  method linesAfter {win} {}
  method saveLayout {} {}
  method loadLayout {} {}
  method hideWin {win} {}
  method showWin {win} {}
  method updateGeometry {opr window} {}
  method maybesetwmgeo {} {}
}

body Layout::setTitle {title} {
  set Title $title
  if [winfo exists .$Topw ] {
    wm title .$Topw $Title
  }
}

body Layout::VisibleInLine {win} {
  return [expr ![$win isClosed] && ![$win isToplevel]]
}

body Layout::constructor {w conf title} {
  toplevel .$w
#  text .$w.text
#   pack .$w.text
  
  set Topw $w
  setTitle $title
  foreach l $conf {
    set line {}
    foreach winconf $l {
      set win [ lindex $winconf 0 ]
      set h [ lindex $winconf 1]
      set frac [ lindex $winconf 2]
      lappend line $win
      set Size($win) [ list $h $frac ]
      windows`register_root $w $win
    }
    lappend Sequence $line
  }
}

body Layout::GetToplevel {} {
  return .$Topw
}

############################################################
# This function initialize the windows of the screen,
# and pack those which are visiable at startup,
# and create the toplevels which are visiable at startup
# return - nothing
############################################################

body Layout::Pack {} {
  global  argv SN options`misc windows`geometry MODE 

  ### the menu line
  if {$this == "::maintool"} {
    # puts "this=$this"
    set m .$Topw.menuline
    pack [frame $m] -fill x
    menuline`pack $m
  }

  ### the command buttons
  buttons`pack $Topw

  set mode $Topw.root

  ### The info on select status for the dependency tool
  if {$this == "::dependencytool"} {
    dependencynamemess`pack .$Topw
  }
  ### Set title
  # wm title .$Topw $Title

  help`add .$Topw $Topw
  
  ### creating the paned window
  set $mode [tixPanedWindow .$mode -paneborderwidth 2 -panerelief flat \
      -command "$this paneResize"]
  
  ### calculating the width of the window
  regexp {^([0-9]+)x} [set windows`geometry($Topw,.)] all totalWidth
  
  
  ### runing through all the lines of the window
  foreach lineSeq $Sequence {
    set visualFract 0
    
    ### runing though each line in this line to calculate the
    ### visual witdh
    foreach win $lineSeq {
      if {[VisibleInLine $win]} {
        set height [ getheight $win ]
        fincr visualFract [ getfraction $win ]
      }
    }
    
    ###
    ### creating the line
    ###
    
    if {$visualFract > 0} {
      ### creating the paned window
      set lineName line$LineCounter
      incr LineCounter
      set line [[set $mode] add $lineName -size $height]
      set frame [tixPanedWindow $line.pane -orient horizontal \
          -paneborderwidth 2 -panerelief flat]
      pack $frame -fill both -expand 1
    }
    
    ### runing through all the windows of the line
    foreach win $lineSeq {
      ### the window is not closed and is not a toplevel:
      if {[VisibleInLine $win]} {
        set fraction [getfraction $win]
        set width [expr int($totalWidth*double($fraction)/$visualFract)]
        set subframe [$frame add $win -size $width]
        
        set winframe ".$Topw.$win"
        frame $winframe
        $win`pack $winframe
        pack $winframe -in $subframe -fill both -expand 1
        $win setPath $winframe
        
        setSN $mode`$win $subframe
        setpackinfo $win $lineName $frame
      }

      ### the window is a toplevel:
      if {[$win isToplevel]} {
        
        toplevel .$win
        wm geometry .$win [ getgeometry $win ]
        $win`pack .$win
        
        packInfo`clear $win
      }
      
      ### the window is closed:
      if {[$win isClosed ]} {
        if {[$win isToplevel ]} {
          wm withdraw .$win
        } else {
          frame .$win
          $win`pack .$win
        }
        
        set reopen $SN(menuline`reopen)
        $reopen add command -label [ $win getTitle ] \
            -command "$win reopen"
        menuline`enable winmenu Reopen
        packInfo`clear $win
      }
    }
  }
  
}

############################################################
# This function create a toplevel window from a frame
# window - the name of the frame to transfer into a toplevel
# return - nothing
############################################################
body Layout::createToplevel window {
  global windows`geometry MODE
  
  ### saving the content of the window 
  $window`save

  ### destroying the window
  closeInLine $window 0
  
  ### adding the window as a toplevel
  $window addAsToplevel
  toplevel .$window
  if {[info exists geometry($window)]} {
    wm geometry .$window $geometry($window)]
  }
  wm title .$window [ $window getTitle ]
  
  ### creating and loading the content of the window
  $window`pack .$window
  $window`load

  ### create a handler for window delete
  wm protocol .$window WM_DELETE_WINDOW "$window close"
}

############################################################
# This function closes a window either an inline or a
# toplevel.
# window - the name of the window to close
############################################################
body Layout::close window {
  global SN 
  set reopen $SN(menuline`reopen)
  if {![$window isToplevel]} {
    closeInLine $window 1
  } else {
    wm withdraw .$window
  }
  $window addAsClosed
  $reopen add command -label [ $window getTitle ] \
      -command "$this reopen $window"
  menuline`enable winmenu Reopen
}  

############################################################
# This function insert a window into the main window
# again, after it has been closed
# window - the name of the window to insert
############################################################
body Layout::reopen window {
  global SN 
  set reopen $SN(menuline`reopen)
  set size [expr [$reopen index end]+1]

  for {set i 0} {$i < $size} {incr i} {
    if {[$reopen entrycget $i -label] == [ $window getTitle ]} {
      break
    }
  }
  $reopen delete $i
  if {[$window isToplevel]} {
    wm deiconify .$window
    raise .$window
    $window removeAsClosed
  } else {
    insert $window 0
  }
  if {$size == 1} {
    menuline`disable winmenu Reopen
  }
}

############################################################
# This function insert a toplevel window into the main
# window
############################################################
body Layout::insertToplevel window {
  $window`save
  destroy .$window
  insert $window 1
}

body Layout::getheight {win} {
  return [lindex $Size($win) 0]
}

body Layout::getfraction {win} {
  return [lindex $Size($win) 1]
}

body Layout::setfraction {win frac} {
  set Size($win) [lreplace $Size($win) 1 1 $frac ]
}

body Layout::getlinename {win} {
  return [lindex $packInfo($win) 0]
}

body Layout::getframe {win} {
  return [lindex $packInfo($win) 1]
}

body Layout::setpackinfo {win linename frame} {
  set packInfo($win) [list $linename $frame]
}

body Layout::packInfo`clear {win} {
  set packInfo($win) ""
}

############################################################
# This is the function which insert a window into the main
# page. It takes care of resizing the frames, and inserting
# the window in the right sequence
# window - The name of the window to insert
# new    - flag indicating wether the window exists or
#          have to be created
############################################################
body Layout::insert {window new} {
  global SN MODE 

  ### remoing the window from the toplevel/closed list
  if {[$window isToplevel]} {
    $window removeAsToplevel
  } else {
    $window removeAsClosed
  }
  set mode $Topw.root
  
  set brothers [brothers $window]
    set left [lindex $brothers 0]
  set right [lindex $brothers 1]
  set inserted 0
  set newLineInserted 0

  ### try to find the element next to the element in the same line
  foreach child $right {
    if {[VisibleInLine $child]} {
      set lineName [ getlinename $child ]
      set frame [ getframe $child ] 
      set subframe [$frame add $window -before $child]
      set inserted 1
      setpackinfo $window $lineName $frame
      break
    }
  }

  ### try to find the element before the element in the same line
  if {!$inserted} {
    foreach child [lreverse $left] {
      if {[VisibleInLine $child]} {
        set lineName [ getlinename $child ]
        set frame [ getframe $child ]
        set subframe [$frame add $window -after $child]
        set inserted 1
        setpackinfo $window $lineName $frame
        break
      }
    }
  }

  ### try to find the line after and insert it before that
  if {!$inserted} {
    foreach line [linesAfter $window] {
      if {$inserted} break
      foreach child $line {
        if {[VisibleInLine $child]} {
          set lineName [ getlinename $child ]            
          updateGeometry + $window
          set newLine [.$mode add line$LineCounter -before $lineName]
          set frame [tixPanedWindow $newLine.pane -orient horizontal \
              -paneborderwidth 2 -panerelief flat]
          pack $frame -fill both -expand 1
          set subframe [$frame add $window]
          set inserted 1
          setpackinfo $window line$LineCounter $frame
          incr LineCounter
          set newLineInserted 1
          break
        }
      }
    }
  }
  
  ### pack the window at the end, if it has not been packed yet.
  if {!$inserted} {
    updateGeometry + $window
    set newLine [.$mode add line$LineCounter]
    set frame [tixPanedWindow $newLine.pane -orient horizontal \
        -paneborderwidth 2 -panerelief flat]
    pack $frame -fill both -expand 1
    set subframe [$frame add $window]
    setpackinfo $window  line$LineCounter $frame
    incr LineCounter
    set newLineInserted 1
  }

  if {$newLineInserted} {
    ### configuring all the vertical pane window
    foreach line $Sequence {
      foreach win $line {
        if {[VisibleInLine $win]} {
          set height [ getheight $win ]
          set lineName [ getlinename  $win ]
          if {$lineName != ""} {
            .$mode paneconfigure $lineName -size $height
            break          
          }
        }
      }
    }
  } else {
    ### configuring the size of all the element in this frame

    set brothers [brothers  $window]
    set children [concat [lindex $brothers 0] [lindex $brothers 1]]
    set newWindowFraction [ getfraction $window ]
    set parrent [ getframe $window ]

    ### calculating the fraction, which is visuable, and the sizes
    set visuableFraction 0
    set totalWidth 0
    catch "unset sizes"
    foreach child $children {
      if {[VisibleInLine $child]} {
        fincr visuableFraction [ getfraction $child ]
        set size [$parrent panecget $child -size]
        incr totalWidth $size
        set sizes($child) $size
        
      }
    }
    ### setting the fractions
    set newFractionVisuable [expr $visuableFraction + $newWindowFraction]
    foreach child [array names sizes] {
      set fraction \
          [expr (double($sizes($child))/$totalWidth) * $visuableFraction]
      set newSize \
          [expr int((double($fraction)/$newFractionVisuable)*$totalWidth)]
      
      setfraction $child $fraction
      $parrent paneconfigure $child -size $newSize
    }
    ### configureing the size of the new window
    set fraction [ getfraction $window ]
    set newSize \
        [expr int((double($fraction)/$newFractionVisuable)*$totalWidth)]
    $parrent paneconfigure $window -size $newSize
  }

  ### packing the window
  setSN $mode`$window $subframe

  if {$new} {
    ### creating the window, and restoring the values
    frame [ $window getPath ]
    $window`pack [ $window getPath ]
    $window`load
  }
  raise [ $window getPath ] $subframe
  pack [ $window getPath ] -in $subframe -fill both -expand 1

  
}

############################################################
# This function delete the frame 'win' from the main window
# window - the window to close
# keep   - flag which indicate wether the window shall be
#          destroyed or unpacked
############################################################
body Layout::closeInLine {window keep} {
  global MODE 

  set mode  $Topw
  
  ### calculating the fractions
  set brothers [brothers $window]
  set children [concat [lindex $brothers 0] [lindex $brothers 1] $window]
  set parrent [getframe $window ]
  set totalWidth 0
  set visualFraction 0
  catch "unset sizes"
  foreach child $children {
    if {[VisibleInLine $child]} {
      set size [$parrent panecget $child -size]
      set sizes($child) $size
      incr totalWidth $size
      fincr visualFraction [ getfraction $child ]
    }
  }
  
  ### setting the fractions and resizing the windows
  set outFraction [ getfraction $window ]
  set newVisualFraction [expr $visualFraction - $outFraction]
  
  foreach child [array names sizes] {
    set fraction [expr (double($sizes($child))/$totalWidth) * $visualFraction]
    setfraction $child $fraction 
    
    if {$child != $window} {
      set newWidth [expr int((double($fraction)/$newVisualFraction) * \
          $totalWidth)]
      $parrent paneconfigure $child -size $newWidth
    }
  }
  

  ### removing the window
  if {$keep} {
    pack forget [ $window  getPath ] 
  } else {
    destroy [ $window getPath ] 
  }

  ### removing the frame from the main page

  set lineName [ getlinename $window ]
  set frame [ getframe $window ]
  $frame delete $window
  
  ### destroying the frame if it is the last one
  set last 1
  set brothers [brothers $window]
  foreach win [concat [lindex $brothers 0] [lindex $brothers 1]] {
    if {[VisibleInLine $win]} {
      set last 0
      break
    }
  }
  if {$last} {
    .$mode.root delete $lineName
    ### updateing the size of the window "."
    updateGeometry - $window
  }
  packInfo`clear $window

}

############################################################
# This function is called when the outer pane (the lines)
# resize
############################################################
body Layout::paneResize {sizes} {
  set index 0
  set sizes [lindex $sizes 0]
  # puts "paneResize ($sizes); Seq=$Sequence; Len=[llength $sizes]"
  foreach line $Sequence {
    set allClosed 1
    foreach win $line {
      if {[VisibleInLine $win]} {
	set allClosed 0
	break
      }
    }
    if {!$allClosed} {
      set height [lindex $sizes $index]
      foreach win $line {
	set fraction [ getfraction $win ]
        # puts "height=$height, Size($win)=$Size($win)"
	set Size($win) [list $height $fraction]
        # puts "Size($win)=$Size($win)"
      }
      incr index
    }
  }
}

############################################################
# This functions initialize the movement of a window
# window - the name of the window to move
# return - nothing
############################################################
body Layout::move window {
  global BITMAPDIR SN
  toplevel .move
  grab set .move

  frame .move.f1
  frame .move.f2
  frame .move.f3
  pack .move.f1 .move.f2 .move.f3 -fill both -expand 1
  
  button .move.f1.up -bitmap bm`Up \
      -command "$this moveUpDown $window up"
  pack .move.f1.up -expand 1

  button .move.f2.left -bitmap bm`Left \
      -command "$this moveLeftRight $window left"
  button .move.f2.ok -text OK -command "$this closeMove $window"
  button .move.f2.right -bitmap bm`Right \
      -command "$this moveLeftRight $window right"
  pack .move.f2.left -side left -expand 1
  pack .move.f2.ok -fill both -side left  -expand 1
  pack .move.f2.right -side left  -expand 1
  
  button .move.f3.down -bitmap bm`Down \
      -command "$this moveUpDown $window down"
  pack .move.f3.down -expand 1
  wm protocol .move WM_DELETE_WINDOW "$this closeMove $window"

  ### hilighting the label on the window
  general`hilight $window on
}

############################################################
# This function is called when the OK button is pressed
# in the "move" window
# window - the name of the window which have been moved
# return - nothing
############################################################
body Layout::closeMove window {
  general`hilight $window off
  destroy .move
  grab release .move
}

############################################################
# This function is called when the "Left" button is
# pressed in the "move" window
# window - then window to move
# return nothing
############################################################
body Layout::moveLeftRight {window direction} {

  set MODE $Topw
  
  ### disable the geometry update
  set bind [bind . <Configure>]
  bind . <Configure> ""

  ### first find the index of the window
  set index [util`dlsearch $Sequence $window]
  set x [lindex $index 0]
  set y [lindex $index 1]
  set line [lindex $Sequence $y]
  

  ### setting the direction up
  if {$direction == "left"} {
    set lineEnd 0
    set frameEnd 0
    set lineIncr -1
  } else {
    set lineEnd [expr [llength $line]-1]
    set frameEnd [expr [llength $Sequence]-1]
    set lineIncr 1
  }

  set nextIndex [nextWindowIndex $x $y $direction]
  set nextX [lindex $nextIndex 0]
  set nextY [lindex $nextIndex 1]
  if {$y != $nextY} {
    ### the window shall move to the end of the previous or the start
    ### of the next line
    close $window
    
    ### update the index of the windows
    set line [lreplace $line $x $x]
    set toLine [lindex $Sequence $nextY]
    set origToLine $toLine
    if {$direction == "left"} {
      lappend toLine $window
    } else {
      set toLine [concat $window $toLine]
    }
    set Sequence [lreplace $Sequence $nextY $nextY $toLine]
    if {$line != ""} {
      set Sequence [lreplace $Sequence $y $y $line]
    } else {
      # just remove the element from the list
      set Sequence [lreplace Sequence $y $y]
    }

    ### updating the sizes
    removeWindowFromLine $line $window
    insertWindowOnLine $origToLine $window
    reopen $window
  } else {
    ### the window shall move  on this line
    set prevNextWin [lindex $line $nextX]
    close $window
    
    ### update the index of the windows and the pack info
    set line [lreplace $line $x $x]
    set line [linsert $line $nextX $window]
    set Sequence [lreplace $Sequence $y $y $line]
    reopen $window
  }
  
  ### enable the geometry update
  bind . <Configure> $bind
}

############################################################
# This function is called when the "Up" button is
# pressed in the "move" window
# window - the window to move
# return - nothing
############################################################
body Layout::moveUpDown {window direction} {
  global MODE  

  ### disable the geometry update
  set bind [bind . <Configure>]
  bind . <Configure> ""

  ### first find the index of the window
  set index [util`dlsearch $Sequence $window]
  set x [lindex $index 0]
  set y [lindex $index 1]
  set line [lindex $Sequence $y]

  if {$direction == "up"} {
    set frameEnd -1
    set incr 0
  } else {
    set frameEnd [llength Sequence]
    set incr 1
  }
  
  set newY [unClosedLine $y $direction $window]
  
  if {[llength $line] > 1} {

    ### test to see if the window is the only one visuable on the line
    set allClosed 1
    foreach win $line {
      if {$win != $window && [VisibleInLine $win]} {
        set allClosed 0
        break
      }
    }

    if {$newY == $frameEnd && $allClosed} {
      runtime`userError "The window cannot move $direction" \
	  errors`move$direction
    }

    close $window
    set line [lreplace $line $x $x]
    removeWindowFromLine $line $window

    set Sequence  [lreplace $Sequence $y $y $line]
    if {!$allClosed} {
      set Sequence [linsert $Sequence [expr $y +$incr] $window]
    } else {
      set Sequence [linsert $Sequence $newY $window]
    }

    setfraction $window 1
    packInfo`clear $window
    reopen $window

  } else {
    ### the window is on a line for itself
    if {$newY == $frameEnd} {
      runtime`userError "The window cannot move $direction" \
	  errors`move$direction
    }
    
    close $window
    set toLine [lindex $Sequence $newY]
    set Sequence [lreplace $Sequence $y $y]
    set Sequence [linsert $Sequence $newY $window]
    reopen $window
  }
  ### enable the geometry update
  bind . <Configure> $bind
}

############################################################
# This function updates the size fraction for all windows
# in 'winlist', so that the fraction for 'window' is
# moved to them.
# winlist - windows which shall resize
# window  - a window which the size shall come from
# return  - nothing
############################################################
body Layout::removeWindowFromLine {winlist window} {
  set outFraction [ getfraction $window ]

  foreach win $winlist {
    set fraction [ getfraction $win ]
    set newFraction [expr $fraction*(1/double(1-$outFraction))]
    setfraction $win $newFraction
  }
}

############################################################
# This function return the index of the prev/next window
# which is neither a toplevel or closed (searching on the
# lines). If there is no prev/next window, an error is
# raised
# offsetX   - the x coordinate to start from
# offsetY   - the y coordinate to start from
# direction - the direction to search (either left or right)
# return    - a list: {x y}
############################################################
body Layout::nextWindowIndex {offsetX offsetY direction} {
  global MODE 

  set window [lindex [lindex $Sequence $offsetY] $offsetX]
  if {$direction == "left"} {
    set endOfFrame -1
    set incr -1
  } else {
    set endOfFrame [llength $Sequence]
    set incr 1
  }

  for {set y $offsetY} {$y != $endOfFrame} {incr y $incr} {

    if {$direction == "left"} {
      set endOfLine "-1"
    } else {
      set endOfLine "[llength [lindex $Sequence $y]]"
    }

    for {set x $offsetX} {$x != $endOfLine} {incr x $incr} {
      set win [lindex [lindex $Sequence $y] $x]
      if {$win != $window && [VisibleInLine $win]} {
        return [list $x $y]
      }
    }
    if {$direction == "left"} {
      if {$y == 0} {
        runtime`userError "The windows cannot move left" errors`moveleft
      }
      set offsetX [expr [llength [lindex $Sequence [expr $y-1]]] -1]
    } else {
      if {$y == [expr [llength $Sequence]-1]} {
        runtime`userError "The windows cannot move right" errors`moveright
      }
      set offsetX 0
    }
  }
}

############################################################
# this function return the index of the first unclosed line
# (ie. a line with an open window) searching in the
# 'direction' direction. If no unclosed line exists, the
# end index is returned
#offsetX   - the x coordinate to start from
# offsetY   - the y coordinate to start from
# direction - the direction to search (either left or right)
# return    - an index (see above)
############################################################
body Layout::unClosedLine {offsetY direction window} {
  global MODE 
  if {$direction == "up"} {
    set end -1
    set incr -1
  } else {
    set end [llength $Sequence]
    set incr 1
  }

  for {set y $offsetY} {$y != $end} {incr y $incr} {
    foreach win [lindex $Sequence $y] {
      if {$win != $window && [VisibleInLine $win]} {
        return $y
      }
    }
  }
  return $y
}

############################################################
# This function updates the sizes for all the windows in
# 'winlist' and for 'window' so that their window fractions
# is changed to include 'window' on the line with windows
# winlist - windows which allready is on the line
# window  - the window which shall be added on the line
# return  - nothing
############################################################
body Layout::insertWindowOnLine {winlist window} {

  set count [expr [llength $winlist] +1]
  foreach win $winlist {
    set fraction [ getfraction $win ]
    set newFraction [expr $fraction -$fraction/double($count)]
    setfraction $win $newFraction
  }
  setfraction  $window [expr 1/double($count)]
}

############################################################
# This function returns a list of window names which is on
# the same line as the 'win' in mode 'mode'
############################################################
body Layout::brothers {win} {
  foreach seq $Sequence {
    set index [lsearch -exact $seq $win]
    if {$index != -1} {
      return [list [lrange $seq 0 [expr $index-1]] \
		  [lrange $seq [expr $index+1] end]]
    }
  }
  runtime`internalError "\"$win\" not in window list"
}

############################################################
# This functions return the part of $Sequence
# which is located after the line which includes 'win'
# mode   - the actual mode
# win    - the window to search for
# return - nothing
############################################################
body Layout::linesAfter {win} {
  set i 0
  foreach seq $Sequence {
    set index [lsearch -exact $seq $win]
    if {$index != -1} {
      return [lrange $Sequence [expr $i+1] end]
    }
    incr i
  }
  runtime`internalError "\"$win\" not in window list"
}


############################################################
# This function save the layout of the interface
############################################################
body Layout::saveLayout {} {

  ### updating the fraction for each window
  foreach mode {maintool debug} {
    foreach line $Sequence {
      set totalWidth 0
      set visualFraction 0
      foreach win $line {
	if {[VisibleInLine $win]} {
	  set fraction [ getfraction $win ]
	  set parrent [ getframe $win ]  
	  set width [$parrent panecget $win -size]
	  incr totalWidth $width
	  fincr visualFraction $fraction
	}
      }
      foreach win $line {
	if {[VisibleInLine $win]} {
	  set parrent [ getframe $win ]          
	  set width [$parrent panecget $win -size]
	  set fraction [expr (double($width)/$totalWidth)/$visualFraction]
	  setfraction $win $fraction 
	}
      }
    }
  }
	
		       
  set text {}
  foreach var {Sequence size geometry} {
    foreach key [array names $var] {
      lappend text "set ${var}($key) \"[options`escape [set ${var}($key)]]\""
    }
  }
  lappend text "set toplevels \"${toplevels}\""
  lappend text "set closed \"${closed}\""
  functions`writeOptions tk-layout $text
}

############################################################
# This function load the layout of the interdace
############################################################
body Layout::loadLayout {} {
  set text [functions`readOptions tk-layout]
  foreach line $text {
    uplevel \#0 $line
  }
}

############################################################
# This window close a toplevel window, and remember
# it's possition and size
# win - the name of the window (ie. debug_trace, modules or error)
# return - nothing
############################################################
body Layout::hideWin win {
  set geometry(.$win) [winfo geometry .$win]
  wm withdraw .$win
}

############################################################
# this function remap a window which have been closed with
# windows´hideWin, and place it the same place and with the
# same size as it had when it was closed.
# win - the name of the window (ie. debug_trace, modules or error)
# return - nothing
############################################################
body Layout::showWin win {  
  if {[info exist geometry(.$win)]} {
    wm geometry .$win $geometry(.$win)
  }
  wm deiconify .$win
}

############################################################
# This function is called when a window is removed
# or inserted in the main window
############################################################

## Workaround for bug in Tix 4.00 which makes window updating
## very slow when moving windows up/down. 
set plusminus -1

body Layout::updateGeometry {opr window} {
  global  MODE windows`geometry plusminus
  set mode $Topw
  set size [set windows`geometry($mode,.) ]
  set found [regexp {^([0-9]+)x([0-9]+)([+-]-?[0-9]+)([+-]-?[0-9]+)$} \
      $size all width height dx dy]
  if {!$found} {
    runtime`internalError "Could not parse geometry. Geometry was: $size"
  }
  set winHeight [getheight $window ]
  set newHeight [expr $height $opr $winHeight]
  if {$opr == "+"} {
    incr newHeight $plusminus
    set plusminus [expr 0 - $plusminus ]
  }
  set windows`geometry($mode,.) "${width}x$newHeight$dx$dy"

  if {$mode == $MODE} {
    wm geometry . ${width}x$newHeight
  } 
}

body Layout::maybesetwmgeo {} {
  global  windows`geometry plusminus
  set w .$Topw
  if {[info exists windows`geometry($w) ]} {
    set size [set windows`geometry($w) ]
    wm geometry $w $size
  } 
}

############################################################
# windows`Busy on/off
############################################################

proc windows`Busy {onoff} {
  global windows`toplevels
  tixBusy .interptool    $onoff
  tixBusy .errortool $onoff
  tixBusy .maintool $onoff
  set l [ itcl::find objects -isa FrameWin ]
  # puts "FrameWins = $l"
  foreach w $l {
    if {[$w isToplevel] && ![$w isClosed]} {
      tixBusy [$w getPath] $onoff
    }
  }
  update idletasks
}

proc windows`gettitle {w} {
  return [ $w getTitle ]
}

proc windows`getroot {tool} {
  switch $tool {
    "maintool" { return ".maintool" }
    "interptool" { return ".interptool" }
    "errortool" { return ".errortool" }
    "dependencytool" { return ".dependencytool" }
    "inhtreetool" { return ".inhtreetool" }
  }
}

proc windows`winroot {win} {
  switch $win {
    "error_editor"  { return ".error.$win" } 
    "debug_editor" - 
    "command" { return ".interptool.$win" }
    - { return ".$win" }
  }
}

proc windows`register_root {root child} {
  global window_root_of
  set window_root_of($child) $root
  # puts "register $child -> $root"
}

proc windows`gettoolroot {win} {
  global window_root_of
  # parray window_root_of
  return $window_root_of($win)
}

    
    
