@echo off
rem *
rem *   Build file for MAPM library using Micro$oft Visual CPP
rem *   under Win NT/9x
rem *
rem *   Note: run VCVARS32.bat in /bin directory of VCPP first to set
rem *         the environment variables.
rem *

echo.
echo Running build file for MAPM library with Visual CPP under Win NT/9x ...
echo.

if exist lib_mapm.lib del lib_mapm.lib
if exist mapm*.obj del mapm*.obj

cl -c /nologo /O2 /DMSDOS mapm*.c

echo Building library lib_mapm.lib...
lib /nologo /out:lib_mapm.lib mapm*.obj

if exist mapm*.obj del mapm*.obj

echo Building program calc.exe...
cl /nologo /O2 calc.c lib_mapm.lib

echo Building program validate.exe...
cl /nologo /O2 validate.c lib_mapm.lib

echo Building program primenum.exe...
cl /nologo /O2 primenum.c lib_mapm.lib

echo Building program cpp_demo.exe...
cl /nologo cpp_demo.cpp lib_mapm.lib

echo.
echo Build file done.

