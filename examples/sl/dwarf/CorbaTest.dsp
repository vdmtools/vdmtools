# Microsoft Developer Studio Project File - Name="CorbaTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CorbaTest - Win32 Rel
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CorbaTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CorbaTest.mak" CFG="CorbaTest - Win32 Rel"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CorbaTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CorbaTest - Win32 Rel" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CorbaTest - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /Gm /GX /ZI /Od /I "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I ".\ToolboxAPI" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 omniORB25_rt.lib omnithread2_rt.lib wsock32.lib advapi32.lib vdm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/DwarfSignal.exe" /pdbtype:sept /libpath:"C:\Program Files\The VDM++ Toolbox v6.1\cg\lib" /libpath:"\\Hermes\local\share\lib\omniORB_2.5.0\lib\x86_win32"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "CorbaTest - Win32 Rel"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CorbaTes"
# PROP BASE Intermediate_Dir "CorbaTes"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /Gm /GX /Zi /Od /I "C:\Program Files\The VDM++UML Toolbox v6.0.2d\cg\include" /I "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I "C:\Program Files\The VDM++UML Toolbox v6.0.2d\api\corba" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /GX /O1 /I "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I "." /I "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I ".\ToolboxAPI" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D __OSVERSION__=4 /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 omniORB25_rt.lib omnithread2_rt.lib wsock32.lib advapi32.lib vdm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"C:\Program Files\The VDM++UML Toolbox v6.0.2d\cg\lib" /libpath:"\\Hermes\local\share\lib\omniORB_2.5.0\lib\x86_win32"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 omniORB25_rt.lib omnithread2_rt.lib vdm.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"Release/DwarfSignal.exe" /libpath:"C:\Program Files\The VDM++ Toolbox v6.1\cg\lib" /libpath:"\\Hermes\local\share\lib\omniORB_2.5.0\lib\x86_win32"
# SUBTRACT LINK32 /verbose /nodefaultlib

!ENDIF 

# Begin Target

# Name "CorbaTest - Win32 Debug"
# Name "CorbaTest - Win32 Rel"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ToolboxAPI\corba_apiSK.cc

!IF  "$(CFG)" == "CorbaTest - Win32 Debug"

# Begin Custom Build
InputPath=.\ToolboxAPI\corba_apiSK.cc
InputName=corba_apiSK

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c /MTd /GX /Od /I "." /I   "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I   "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I       "ToolboxAPI" /D   "WIN32"  /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D         __OSVERSION__=4 -Tp$(InputName).cc

# End Custom Build

!ELSEIF  "$(CFG)" == "CorbaTest - Win32 Rel"

# Begin Custom Build
InputPath=.\ToolboxAPI\corba_apiSK.cc
InputName=corba_apiSK

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c /MT /GX /Od /I "." /I   "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I   "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I       "ToolboxAPI" /D   "WIN32"  /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D         __OSVERSION__=4 -TpToolboxAPI\$(InputName).cc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ToolboxAPI\corba_client.cc

!IF  "$(CFG)" == "CorbaTest - Win32 Debug"

# Begin Custom Build
InputPath=.\ToolboxAPI\corba_client.cc
InputName=corba_client

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c /MTd /GX /Od /I         "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I         "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I   "." /I ".\ToolboxAPI" /D   "WIN32" /D       "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__"   /D       __OSVERSION__=4 -Tp$(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "CorbaTest - Win32 Rel"

# Begin Custom Build
InputPath=.\ToolboxAPI\corba_client.cc
InputName=corba_client

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c /MT /GX /Od /I "." /I   "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I   "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I       "ToolboxAPI" /D   "WIN32"  /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D         __OSVERSION__=4 -TpToolboxAPI\$(InputName).cc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CorbaTest.cpp
# End Source File
# Begin Source File

SOURCE=.\CorbaTest.rc
# End Source File
# Begin Source File

SOURCE=.\CorbaTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolboxAPI\metaiv_idlSK.cc

!IF  "$(CFG)" == "CorbaTest - Win32 Debug"

# Begin Custom Build
InputPath=.\ToolboxAPI\metaiv_idlSK.cc
InputName=metaiv_idlSK

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c /MTd /GX /Od /I "." /I   "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I   "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I       "ToolboxAPI" /D   "WIN32"  /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D         __OSVERSION__=4 -Tp$(InputName).cc

# End Custom Build

!ELSEIF  "$(CFG)" == "CorbaTest - Win32 Rel"

# Begin Custom Build
InputPath=.\ToolboxAPI\metaiv_idlSK.cc
InputName=metaiv_idlSK

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl -c /MT /GX /Od /I "." /I   "C:\Program Files\The VDM++ Toolbox v6.1\cg\include" /I   "\\Hermes\local\share\lib\omniORB_2.5.0\include" /I       "ToolboxAPI" /D   "WIN32"  /D "_WINDOWS" /D "__WIN32__" /D "__x86__" /D "__NT__" /D         __OSVERSION__=4 -TpToolboxAPI\$(InputName).cc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CorbaTest.h
# End Source File
# Begin Source File

SOURCE=.\CorbaTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\ToolboxAPI\fstream.h
# End Source File
# Begin Source File

SOURCE=.\ToolboxAPI\iostream.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolboxAPI\strstream.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\allon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CorbaTest.ico
# End Source File
# Begin Source File

SOURCE=.\res\CorbaTest.rc2
# End Source File
# Begin Source File

SOURCE=.\res\dark.bmp
# End Source File
# Begin Source File

SOURCE=.\res\drive.bmp
# End Source File
# Begin Source File

SOURCE=.\res\l1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\l2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\l3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\warning.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
