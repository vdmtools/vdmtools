@echo off
rem -- Runs a VDM test example for one argument file

rem -- Output the argument to stdout (for redirect) and
rem -- "con" (for user feedback)
echo VDM Test: '%1' > con
echo VDM Test: '%1'
set SPEC1=..\DoSort.rtf ..\SortMachine.rtf ..\ExplSort.rtf
set SPEC2= ..\ImplSort.rtf ..\Sorter.rtf ..\MergeSort.rtf 

vppde -i -R vdm.tc -O %1.res %1 %SPEC1% %SPEC2%

rem -- Check for difference between result of execution and
rem -- expected result.
fc /w %1.res %1.exp

:end
