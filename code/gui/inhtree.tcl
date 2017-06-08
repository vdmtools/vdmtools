######################################################################
# Inheritance Tree Window. 
#
# $Id: inhtree.tcl,v 1.12 1997/09/19 09:30:51 niels Exp $
######################################################################

### Defining background and selection colours and fonts
global inhtree`bgCol 
set inhtree`bgCol [lookUpTixColour light2_bg]
global inhtree`selectCol 
set inhtree`selectCol [lookUpTixColour select_bg]
global inhtree`font
set inhtree`font [lookUpTixColour font]

proc inhtree`pack w {
  global inhtree`bgCol
  setSN inhtree`top $w
  ### create the label
  help`add $w inhtreePage`inhtree
  general`packLabel $w inhtree

  ### creating the inhtree canvas and frame
  scrollbar $w.yscroll \
      -command "$w.inhtreecanvas yview" \
      -relief {sunken}
  scrollbar $w.xscroll \
      -command "$w.inhtreecanvas xview" \
      -orient {horizontal} \
      -relief {sunken} 
  canvas $w.inhtreecanvas \
      -height {0} \
      -borderwidth 1 \
      -width {0} \
      -scrollregion {0c 0c 20c 20c} \
      -background ${inhtree`bgCol} \
      -yscrollcommand "$w.yscroll set" \
      -xscrollcommand "$w.xscroll set"

  pack append $w \
      $w.yscroll {right frame center filly} \
      $w.inhtreecanvas {top frame center expand fill} \
      $w.xscroll {top frame center fillx}
      
  set ihntreebox $w.inhtreecanvas
  setSN modules`inhtreebox $ihntreebox
  AddFocusWin $ihntreebox
}


global maxx
set maxx 0
global maxy
set maxy 0

global canvas_window_count
set canvas_window_count 0
global canvasnode_reqwid
set canvasnode_reqwid 0
## canvasnode_reqwid is set by PutCanvasNode and used by PutCanvasEdge.  It
## is the width of a superclass node, and it is updated by each invokation of
## PutCanvasNode.

global can_node_maxw
button .maxbw -text KKKKKKKKKKKKKKKKK -relief flat -padx 0 -pady 0
set can_node_maxw [ winfo reqwidth .maxbw ]


proc inhtree`reset {} {
  inhtree`clear
}

proc inhtree`clear {} {
  inhtree`clearWindows
  inhtree`clearSelections
}

proc inhtree`clearWindows {} {
  global canvas_window_count
  global maxx
  global maxy
  global SN
  $SN(modules`inhtreebox) delete Node rectNode Edge
  set canvas_window_count 0
  set maxx 0
  set maxy 0
}

proc inhtree`clearSelections {} {
  global SN
  $SN(modules`inhtreebox) dtag selected
  $SN(modules`inhtreebox) dtag selectedtext
}

proc GetCanvasFramePath {count} {
  global SN
  return [ format "%s.frame%d" $SN(modules`inhtreebox) $count ]
}

## PutCanvasNode creates a node for a class.
## PutCanvasEdge creates an edge from a superclass to a subclass.
## The order of invocation of PutCanvasNode and PutCanvasEdge is
## important, it must follow the following pattern:
## First all top-level superclasses must be inserted.  For each class
## all subclasses must be found and edges must be created immediatly
## after.

global button_map

proc PutCanvasNode {x1 y name} {
  global SN
  global can_node_maxw 
  global canvasnode_reqwid canvasnode_reqhei
  global canvas_window_count
  global maxx
  global maxy
  global inhtree`bgCol inhtree`font
  set inhtreebox $SN(modules`inhtreebox)
  incr canvas_window_count 1

  set x [expr $x1 * $can_node_maxw + 40]

  set textid [$inhtreebox create text [expr $x + 17 ] $y \
      -text $name \
      -anchor w \
      -font ${inhtree`font} \
      -tags "Node text$name" ]
  set rectcorners [$inhtreebox bbox "text$name"]
  set rectid [$inhtreebox create rect \
      [lindex $rectcorners 0] [lindex $rectcorners 1] \
      [lindex $rectcorners 2] [lindex $rectcorners 3] \
      -fill ${inhtree`bgCol} \
      -outline ${inhtree`bgCol} \
      -tags "rectNode $name" ]
  $inhtreebox raise "text$name"
  $inhtreebox bind $textid <ButtonRelease-1> \
      "global modules`select; \
       set modules`select inhtree; \
       inhtree`set_selection $name; \
       modules`selectFunctions; \
       set modules`select module"
  $inhtreebox bind $textid <Control-ButtonRelease-1> \
      "global modules`select; \
       set modules`select inhtree; \
       inhtree`append_selection $name; \
       modules`selectFunctions; \
       set modules`select module"

  set canvasnode_reqwid [expr [lindex $rectcorners 2] - [lindex $rectcorners 0]]
  
  set posx [expr $x + $canvasnode_reqwid + 40]
  set posy [expr $y + 20]
  if {$posx > $maxx} {
    set maxx $posx
  }
  if {$posy > $maxy} {
    set maxy $posy
  }
}

proc PutCanvasEdge {x11 y1 x21 y2} {
  global SN
  global can_node_maxw
  global canvasnode_reqwid
  set w [expr $canvasnode_reqwid + 20]

  set x1 [expr $x11 * $can_node_maxw + 40]
  set x2 [expr $x21 * $can_node_maxw + 40]
  $SN(modules`inhtreebox) create line \
      [expr $x1 + $w ]  [expr $y1 ] \
      [expr $x2 ]       [expr $y2 ] -tag Edge
}

proc UpdateInhTree {} {
  global SN
  global inhtree
  global maxx
  global maxy
  update
  if { [winfo ismapped .top1] } {
    inhtree`clear
    tkif_drawInhTree
    $SN(modules`inhtreebox) configure -scrollregion "0 0 $maxx $maxy"
# ETN 950407
# Kommenter ind, saafremt kanter skal gaa ovenpaa tekst. 
# Virker naar Buttons/Label er fjernet og vi kun bruger 
# .c create text og .c create bitmap. Saa virker raise. 
#       [SN TreeCanvas] raise Edge Node 
#       [SN TreeCanvas] raise Edge Bitmap
##    SetView
  }
}

proc inhtreetool_open {} {
  global SN
  set w .inhtreetool

  if {[winfo exist $w]} {
    if {[wm state $w] != "normal"} {
      inhtreetool maybesetwmgeo
    }
    wm deiconify $w
    raise $w
    ### Update Inheritance Tree
    tkif_drawInhTree
    return
  }
  toplevel $w
}


proc IsInhTreeOpen {} {
  if {[string compare [wm state .inhtreetool] normal] == 0} {
    return 1
  } else {
    return 0
  }
}

proc inhtree`resizeWindow {} {
  global SN
  global maxx maxy
  $SN(modules`inhtreebox) configure -scrollregion "0 0 $maxx $maxy"
}

proc inhtree`set_selection name {
  global SN
  set inhtreebox $SN(modules`inhtreebox) 
  $inhtreebox dtag selected
  $inhtreebox dtag selectedtext
  $inhtreebox addtag selected withtag $name
  $inhtreebox addtag selectedtext withtag "text$name"
}

proc inhtree`append_selection name {
  global SN
  set inhtreebox $SN(modules`inhtreebox) 
  set nametags [$inhtreebox gettag $name]
  set isselected [lsearch -exact $nametags selected]
  if {$isselected == -1} {
    $inhtreebox addtag selected withtag $name
    $inhtreebox addtag selectedtext withtag "text$name"
  } else {
    $inhtreebox dtag $name selected
    $inhtreebox dtag "text$name" selectedtext
  }
}

proc inhtree`get_selection {modbox} {
  global SN
  set inhtreebox $SN(modules`inhtreebox)
  set classlist {}
  set items [$inhtreebox find withtag selectedtext]
  foreach classitem $items {
    lappend classlist [$inhtreebox itemcget $classitem -text]
  }
  return $classlist 
}
  
proc inhtree`markSelectedModule {modbox name} {
  global SN inhtree`selectCol 
  set modbox $SN(modules`inhtreebox)
  set items [$modbox find withtag $name]
  $modbox itemconfigure $name -fill ${inhtree`selectCol} \
      -outline ${inhtree`selectCol}
  $modbox addtag selected withtag $name
  $modbox addtag selectedtext withtag "text$name"
}

proc inhtree`markAllSelectedModules {} {
  global SN inhtree`selectCol 
  set modbox $SN(modules`inhtreebox)
  foreach name [modules`selectedModules] {
    $modbox itemconfigure $name -fill ${inhtree`selectCol} \
        -outline ${inhtree`selectCol}
    $modbox addtag selected withtag $name
    $modbox addtag selectedtext withtag "text$name"
  }
}

proc inhtree`clearSelection {modbox} {
  global inhtree`bgCol
  $modbox itemconfigure rectNode -fill ${inhtree`bgCol} \
      -outline ${inhtree`bgCol}
  $modbox dtag selected
  $modbox dtag selectedtext
}
