@echo off
rem -- Runs a collection of VDM++ test examples for the
rem -- sorting example
set PRJDIR=G:\fmerail\casestudy\KLV
set SPEC1=%PRJDIR%\Beacon.rtf %PRJDIR%\CabDisplay.rtf %PRJDIR%\CheckSpeedEvent.rtf %PRJDIR%\EmergencyBreak.rtf 
set SPEC2=%PRJDIR%\Event.rtf %PRJDIR%\FLTV.rtf %PRJDIR%\HeadMeetBeaconEvent.rtf %PRJDIR%\KLV.rtf %PRJDIR%\OnBoardComp.rtf
set SPEC3=%PRJDIR%\TIV_A.rtf %PRJDIR%\TIV_D.rtf %PRJDIR%\TIV_E.rtf %PRJDIR%\TailMeetBeaconEvent.rtf %PRJDIR%\Test.rtf
set SPEC4=%PRJDIR%\KLVStateEvent.rtf %PRJDIR%\NoBeaconMetEvent.rtf %PRJDIR%\MaxSpeedEvent.rtf

vppde -p -R %PRJDIR%\vdm.tc %SPEC1% %SPEC2% %SPEC3% %SPEC4%
for /R %%f in (*.arg) do call vdmtest "%%f"
