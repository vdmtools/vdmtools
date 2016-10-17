#!/bin/sh

MAKENSIS="/cygdrive/c/Program Files (x86)/NSIS/makensis"
"$MAKENSIS" /DVERSION=9.0.2 /DVERSIONL=9.0.2.0 /DSETUPNAME=setuppp-9.0.2.exe /DGUITOOL=vppgde.exe /DCLITOOL=vppde.exe /DTOOLNAME=VDM++\ Toolbox /DVDMPP=1 /DWIN64=1 vdmtools.nsi
