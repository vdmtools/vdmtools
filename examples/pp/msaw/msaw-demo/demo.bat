rem *************************
rem Windows NT demo bat-file
rem *************************

set PATH=c:\tcl\itcl2.2\tk4.2\win;%PATH%
set PATH=c:\tcl\itcl2.2\tcl7.6\win;%PATH%
set PATH=c:\tcl\itcl2.2\itcl\win;%PATH%
set PATH=c:\tcl\itcl2.2\itk\win;%PATH%

set TCL_LIBRARY=c:\tcl\itcl2.2\tcl7.6\library
set TK_LIBRARY=c:\tcl\itcl2.2\tk4.2\library
set ITCL_LIBRARY=c:\tcl\itcl2.2\itcl\library
set ITK_LIBRARY=c:\tcl\itcl2.2\itk\library
msaw-code\msaw -o msaw-data\eiffeltower msaw-data\kl355 msaw-data\orly -f msaw-data\descending -f msaw-data\diagonal_up msaw-data\kl400 > output.log
itkwish msaw-lib/cap-nt
