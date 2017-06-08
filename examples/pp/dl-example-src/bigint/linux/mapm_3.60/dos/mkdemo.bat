rem *
rem *
rem *   executable build file for Micro$oft 5.1 / 8.00c  (16 bit)
rem *
rem *
cl /c /AL /Gs calc.c
link /ST:4096 /NOI /CPARM:1 /MAP /FARCA /PACKCO calc,,,lib_mapm.lib;
del calc.obj
rem *
rem *
cl /c /AL /Gs validate.c
link /ST:4096 /NOI /CPARM:1 /MAP /FARCA /PACKCO validate,,,lib_mapm.lib;
del validate.obj
rem *
rem *
cl /c /AL /Gs primenum.c
link /ST:4096 /NOI /CPARM:1 /MAP /FARCA /PACKCO primenum,,,lib_mapm.lib;
del primenum.obj
rem *
rem *
