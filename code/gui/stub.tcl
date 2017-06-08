######################################################################
# Interface to C layer. 
#
# $Id: stub.tcl,v 1.42 1997/10/28 18:44:38 erik Exp $
######################################################################

  
############################################################
# These two function should take care of reading and
# writing the VDM options. It's purpose is to translate
# from the data structure below to the assosiative array
# options`vdm.
############################################################
proc below`saveOptions filename {
  puts "The stub should save to $filename"
}
proc below`loadOptions {} {
  # NYI puts "The stub should load from file"
}

proc askForSucces {file what} {
  return [tk_dialog .ask "Will it pass??" "Do you think $file will pass $what" questhead 1 no yes]
}
