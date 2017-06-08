@echo off
rem -- Runs a collection of VDM++ test examples for
rem -- the sorting example
set SPEC=..\sort.rtf 

vdmde -p -R vdm.tc %SPEC%

for /R %%f in (*.arg) do call vdmtest "%%f"
