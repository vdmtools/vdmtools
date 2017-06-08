@echo off
rem -- Runs a VDM test example for one argument file

rem -- Output the argument to stdout (for redirect) and
rem -- "con" (for user feedback)
echo VDM Test: '%1' > con
echo VDM Test: '%1'
set PRJDIR=G:\fmerail\casestudy\KLV
set SPEC1=%PRJDIR%\Beacon.rtf %PRJDIR%\CabDisplay.rtf %PRJDIR%\CheckSpeedEvent.rtf %PRJDIR%\EmergencyBreak.rtf 
set SPEC2=%PRJDIR%\Event.rtf %PRJDIR%\FLTV.rtf %PRJDIR%\HeadMeetBeaconEvent.rtf %PRJDIR%\KLV.rtf %PRJDIR%\OnBoardComp.rtf
set SPEC3=%PRJDIR%\TIV_A.rtf %PRJDIR%\TIV_D.rtf %PRJDIR%\TIV_E.rtf %PRJDIR%\TailMeetBeaconEvent.rtf %PRJDIR%\Test.rtf
set SPEC4=%PRJDIR%\KLVStateEvent.rtf %PRJDIR%\NoBeaconMetEvent.rtf %PRJDIR%\MaxSpeedEvent.rtf

vppde -i -R %PRJDIR%\vdm.tc -O %1.res %1 %SPEC1% %SPEC2% %SPEC3% %SPEC4%

rem -- Check for difference between result of execution and
rem -- expected result.
fc /w %1.res %1.exp

:end
