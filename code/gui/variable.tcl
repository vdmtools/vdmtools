############################################################
# This function pack the variable frame
# w - the window to pack in
############################################################
proc variable`pack w {
  setSN variable`top $w
  help`add $w mainPage`variable
  general`packLabel $w variable
  set text [tixScrolledText $w.text -scrollbar auto]
  pack $text -fill both -expand 1
  setSN variable`text [$text subwidget text]
}

############################################################
# These two functions save/load the content of the frame
# when it is moved from frame to toplevel or back
############################################################
proc variable`save {} {
  general`saveText variable
}
proc variable`load {} {
  general`loadText variable
}

############################################################
# These two function print/pipe the content of the
# 'variable' buffer
############################################################
proc variable`print {} {
  general`printText variable
}
proc variable`pipe {} {
  general`pipeText variable
}
