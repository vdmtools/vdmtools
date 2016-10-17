
proc debug`init {} {
  # windows`Busy on
  debug`menucmd "init" 
  # windows`Busy off
}

proc debug`continue {} {
  # windows`Busy on
  debug`menucmd "cont"  
  # windows`Busy off
}

proc debug`step {} {
  # windows`Busy on
  debug`menucmd "step" 
  # windows`Busy off
}

proc debug`singlestep {} {
  # windows`Busy on
  debug`menucmd "singlestep" 
  # windows`Busy off
}

proc debug`stepin {} {
  # windows`Busy on
  debug`menucmd "stepin" 
  # windows`Busy off
}

proc debug`finish {} {
  # windows`Busy on
  debug`menucmd "finish" 
  # windows`Busy off
}

proc debug`up {} {
  debug`menucmd "up" 
}

proc debug`down {} {
  debug`menucmd "down" 
}

proc debug`menucmd {cmd} {
  windows`Busy on

  command`removePrompt
  # debuglog`addmsg "(menu: $cmd)\n"
  tkif_SetEditorName debug_editor
  tkif_executeCommandPrompt $cmd commandlog`addmsg
#  set printprompt [ tkif_executeCommand $cmd commandlog`addmsg ]
#  if {$printprompt} {
#    command`insertPrompt
#  }

  windows`Busy off
}
