; VDMTools Installer by NSIS

!include MUI2.nsh

!ifdef WIN64
!include x64.nsh
!include LogicLib.nsh
!endif

;!define TOOLNAME "${TOOLTYPE} Toolbox"
!define LONGNAME "${TOOLNAME} v${VERSION}"
!define PACKAGE "The ${LONGNAME}"
;!define APPDIR "./distdir"
!define KYUSHU_UNIVERSITY "Kyushu University"
!define VDM_URL "http://fmvdm.org/"
!define GROUPNAME "VDMTools"

!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall"

Name "${PACKAGE}"
OutFile "${SETUPNAME}"

!ifdef WIN64
InstallDir "$PROGRAMFILES64\${PACKAGE}"
!else
InstallDir "$PROGRAMFILES\${PACKAGE}"
!endif
InstallDirRegKey HKLM "SOFTWARE\${PACKAGE}" ""

RequestExecutionLevel admin
ShowInstDetails show
ShowUninstDetails show
VIProductVersion ${VERSIONL}
VIAddVersionKey "ProductName" "${PACKAGE}"
VIAddVersionKey "CompanyName" "${KYUSHU_UNIVERSITY}"
VIAddVersionKey "ProductVersion" "${VERSION}"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "FileDescription" "${PACKAGE}"
VIAddVersionKey "LegalCopyright" "${KYUSHU_UNIVERSITY}"

!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
 
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-uninstall-r.bmp"
 
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
;!insertmacro MUI_PAGE_LICENSE $(license)
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Japanese"

LicenseLangString license ${LANG_JAPANESE} "gplv3.rtf"
LicenseLangString license ${LANG_ENGLISH} "gplv3.rtf"

Section
  SectionIn RO

  SetShellVarContext all

  SetOutPath $INSTDIR
  #File /r "${APPDIR}\dll"
  #Rename "$INSTDIR\dll" "$INSTDIR\bin"

  File /r "${APPDIR}\bin"
  File /r "${APPDIR}\emacs"
  File /r "${APPDIR}\examples"
  File /r "${APPDIR}\latex"
  File /r "${APPDIR}\stdlib"
  File /r "${APPDIR}\word"

  !ifdef VDMSL
    File /r "${APPDIR}\cg"
    File /r "${APPDIR}\api"
  !endif

  !ifdef VDMPP
    File /r "${APPDIR}\cg"
    File /r "${APPDIR}\api"
    File /r "${APPDIR}\java2vdm"
    File /r "${APPDIR}\javacg"
    File /r "${APPDIR}\uml"
  !endif

  !ifdef VICE
    File /r "${APPDIR}\cg"
    File /r "${APPDIR}\api"
    File /r "${APPDIR}\java2vdm"
    File /r "${APPDIR}\javacg"
    File /r "${APPDIR}\uml"
  !endif

  SetShellVarContext all

  CreateDirectory "$SMPROGRAMS\${GROUPNAME}"
  CreateShortCut "$SMPROGRAMS\${GROUPNAME}\${LONGNAME}.lnk" "$INSTDIR\bin\${GUITOOL}"
  CreateShortCut "$SMPROGRAMS\${GROUPNAME}\${LONGNAME} command line.lnk" "$INSTDIR\bin\${CLITOOL}"
  CreateShortCut "$SMPROGRAMS\${GROUPNAME}\${LONGNAME} examples.lnk" "$INSTDIR\examples"
  CreateShortCut "$SMPROGRAMS\${GROUPNAME}\${LONGNAME} Uninstall.lnk" "$INSTDIR\uninstall.exe"

  CreateShortCut "$DESKTOP\${TOOLNAME}.lnk" "$INSTDIR\bin\${GUITOOL}" ""

  WriteRegStr HKLM "SOFTWARE\${PACKAGE}" "" "$INSTDIR"
  WriteRegStr HKLM "${UNINSTALL_KEY}\${PACKAGE}" "DisplayName" "${PACKAGE} (remove only)"
  WriteRegStr HKLM "${UNINSTALL_KEY}\${PACKAGE}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${UNINSTALL_KEY}\${PACKAGE}" "Publisher" "${KYUSHU_UNIVERSITY}"
  WriteRegStr HKLM "${UNINSTALL_KEY}\${PACKAGE}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${UNINSTALL_KEY}\${PACKAGE}" "HelpLink" "${VDM_URL}"

  !ifdef VDMSL
    WriteRegStr HKCR ".vdm" "" "VDM-SL.File"
    WriteRegStr HKCR ".vdmsl" "" "VDM-SL.File"
    WriteRegStr HKCR "VDM-SL.File" "" "VDM-SL Specification file"
    WriteRegStr HKCR "VDM-SL.File\DefaultIcon" "" "$INSTDIR\bin\${GUITOOL},1"
  !endif

  !ifdef VDMPP
    WriteRegStr HKCR ".vpp" "" "VDM++.File"
    WriteRegStr HKCR ".vdmpp" "" "VDM++.File"
    WriteRegStr HKCR "VDM++.File" "" "VDM++ Specification file"
    WriteRegStr HKCR "VDM++.File\DefaultIcon" "" "$INSTDIR\bin\${GUITOOL},1"
  !endif
  
  !ifdef VICE
    WriteRegStr HKCR ".vice" "" "VICE.File"
    WriteRegStr HKCR ".vdmrt" "" "VICE.File"
    WriteRegStr HKCR "VICE.File" "" "VDM++ VICE Specification file"
    WriteRegStr HKCR "VICE.File\DefaultIcon" "" "$INSTDIR\bin\${GUITOOL},1"
  !endif
  
  WriteUninstaller "uninstall.exe"

  #IfFileExists "$INSTDIR\bin\omni*416*.dll" +2 0
  #  Delete "$INSTDIR\bin\omni*416*.dll"

  #IfFileExists "$INSTDIR\dll\omni*416*.dll" +2 0
  #  Delete "$INSTDIR\dll\omni*416*.dll"

SectionEnd

Section Uninstall
  DeleteRegKey HKLM "${UNINSTALL_KEY}\${PACKAGE}"
  DeleteRegKey HKLM "SOFTWARE\${PACKAGE}"

  !ifdef VDMSL
    DeleteRegKey HKCR ".vdm"
    DeleteRegKey HKCR ".vdmsl"
    DeleteRegKey HKCR "VDM-SL.File"
  !endif

  !ifdef VDMPP
    DeleteRegKey HKCR ".vpp"
    DeleteRegKey HKCR ".vdmpp"
    DeleteRegKey HKCR "VDM++.File"
  !endif

  !ifdef VICE
    DeleteRegKey HKCR ".vice"
    DeleteRegKey HKCR ".vdmrt"
    DeleteRegKey HKCR "VICE.File"
  !endif

  SetShellVarContext all
  
  Delete "$DESKTOP\${TOOLNAME}.lnk"
  RMDir /r "$INSTDIR"
  Delete "$SMPROGRAMS\${GROUPNAME}\${LONGNAME}.lnk"
  Delete "$SMPROGRAMS\${GROUPNAME}\${LONGNAME} command line.lnk"
  Delete "$SMPROGRAMS\${GROUPNAME}\${LONGNAME} examples.lnk"
  Delete "$SMPROGRAMS\${GROUPNAME}\${LONGNAME} Uninstall.lnk"
  
  IfFileExists "$SMPROGRAMS\${GROUPNAME}\*.lnk" +2 0
    RMDir /r "$SMPROGRAMS\${GROUPNAME}"

SectionEnd

Function .onInit
  ; for debug
;  StrCpy $LANGUAGE 1033
;  StrCpy $LANGUAGE 1041
FunctionEnd
