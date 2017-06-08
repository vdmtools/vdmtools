@echo off
rem Runs a VDM test example for one argument file.

rem Output the argument to stdout (for redirect) and
rem "con" (for user feedback)
echo VDM Test: '%1' > con
echo VDM Test: '%1'

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

rem Run the specification with argument while collecting
rem test coverage information, and write the result to an
rem output file.
vppde -iDIPQ -R vdm.tc -O %1.res %1 %SPEC%

rem -- Check for difference between result of execution and
rem -- expected result.
fc /w %1.res %1.exp > con
fc /w %1.res %1.exp

:end
