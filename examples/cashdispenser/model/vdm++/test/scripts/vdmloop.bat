@echo off
rem Runs a collection of VDM++ test examples for the sorting example.
set MODELDIR=../..
set TESTDIR=..

set MODEL1=%MODELDIR%/Account.rtf %MODELDIR%/Card.rtf %MODELDIR%/CardHolder.rtf  
set MODEL2=%MODELDIR%/CentralResource.rtf %MODELDIR%/Clock.rtf 
set MODEL3=%MODELDIR%/Letter.rtf %MODELDIR%/Letterbox.rtf %MODELDIR%/Till.rtf 


set TEST1=%TESTDIR%/Event.rtf %TESTDIR%/GetBalance.rtf 
set TEST2=%TESTDIR%/InsertCard.rtf %TESTDIR%/ReturnCard.rtf 
set TEST3=%TESTDIR%/SendStatement.rtf %TESTDIR%/System.rtf 
set TEST4=%TESTDIR%/Test.rtf %TESTDIR%/Validate.rtf %TESTDIR%/Withdrawal.rtf 
set TEST5=%TESTDIR%/IllegalCard.rtf"

set SPEC=%MODEL1% %MODEL2% %MODEL3% %TEST1% %TEST2% %TEST3% %TEST4% %TEST5%

rem Generate the test coverage file vdm.tc
vppde -p -R vdm.tc %SPEC%

rem Find all argument files and run them on the specification.
for /R %%f in (*.arg) do call vdmtest "%%f"
