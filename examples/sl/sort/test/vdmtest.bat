@echo off
rem -- Runs a VDM test example for one argument file

rem -- Output the argument to stdout (for redirect) and
rem -- "con" (for user feedback)
echo VDM Test: '%1' > con
echo VDM Test: '%1'
set SPEC=..\sort.rtf 

vdmde -i -R vdm.tc -O %1.res %1 %SPEC%

rem -- Check for difference between result of execution
rem -- and expected result.
fc /w %1.res %1.exp

:end
