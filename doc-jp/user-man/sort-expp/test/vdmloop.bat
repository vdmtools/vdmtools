@echo off
rem -- Runs a collection of VDM++ test examples for the
rem -- sorting example
set SPEC1=..\DoSort.rtf ..\SortMachine.rtf ..\ExplSort.rtf
set SPEC2=..\ImplSort.rtf ..\Sorter.rtf ..\MergeSort.rtf 

vppde -p -R vdm.tc %SPEC1% %SPEC2%
for /R %%f in (*.arg) do call vdmtest "%%f"
