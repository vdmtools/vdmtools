@echo off
rem MSVC50OPTS.BAT
rem
rem    Compile and link options used for building MEX-files
rem    using the Microsoft Visual C++ compiler version 5.0 
rem
rem    $Revision: 1.2 $  $Date: 1999/03/22 15:27:34 $
rem
rem ********************************************************************
rem General parameters
rem ********************************************************************

set MATLAB=%MATLAB%
set MSVC_ROOT="D:\Program Files\DevStudio"
set TBDIR="C:\Program Files\The IFAD VDM-SL Toolbox V3.3"
set OMNIDIR=\\hermes\local\share\lib\omniORB_2.6.1
set CORBADIR=..\corba
set PATH=%MSVC_ROOT%\VC\BIN;%MSVC_ROOT%\sharedIDE\bin;%PATH%
set INCLUDE=%MSVC_ROOT%\VC\INCLUDE;%MSVC_ROOT%\VC\MFC\INCLUDE;%MSVC_ROOT%\VC\ATL\INCLUDE;%INCLUDE%
set LIB=%MSVC_ROOT%\VC\LIB;%MSVC_ROOT%\VC\MFC\LIB;%TBDIR%\cg\lib;%OMNIDIR%\lib\x86_win32;%LIB%

rem ********************************************************************
rem Compiler parameters
rem ********************************************************************
set INCDIR=-I. -I%TBDIR%\CG\INCLUDE -I%TBDIR%\API\CORBA -I.\VDM -I%OMNIDIR%\include -I%CORBADIR%
set COMPILER=cl
set COMPFLAGS=-c -Zp8 -G5 -W0 -GX -MT -D__WIN32__ -D__x86__ -D__NT__ -D__OSVERSION__=4 -DMATLAB_MEX_FILE %INCDIR%
set OPTIMFLAGS=-O2
set DEBUGFLAGS=
rem -Zi

rem ********************************************************************
rem Library creation command
rem ********************************************************************
set PRELINK_CMDS=lib /def:"%MATLAB%\extern\include\matlab.def" /machine:ix86 /OUT:%LIB_NAME%1.lib 
set PRELINK_DLLS=lib /def:"%MATLAB%\extern\include\%DLL_NAME%.def" /machine:ix86 /OUT:%DLL_NAME%.lib	

rem ********************************************************************
rem Linker parameters
rem ********************************************************************
set LINKER=link
set LINKFLAGS=/dll /export:mexFunction %LIB_NAME%1.lib /implib:%LIB_NAME%l.lib omniORB260_rtd.lib omnithread2_rtd.lib wsock32.lib advapi32.lib
rem set LINKFLAGS=/dll /export:mexFunction %LIB_NAME%1.lib /implib:%LIB_NAME%l.lib /NODEFAULTLIB libcpmt.lib libcmt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib omniORB260_rtd.lib omnithread2_rtd.lib wsock32.lib advapi32.lib
set LINKOPTIMFLAGS=
set LINKDEBUGFLAGS=
rem /debug
set LINK_FILE=
set LINK_LIB=
set NAME_OUTPUT=/out:%MEX_NAME%.dll

rem ********************************************************************
rem Resource compiler parameters
rem ********************************************************************
set RC_COMPILER=rc /fo mexversion.res
set RC_LINKER= 
