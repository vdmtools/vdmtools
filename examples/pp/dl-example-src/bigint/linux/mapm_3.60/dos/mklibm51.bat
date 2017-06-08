rem *
rem *
rem *   library build file for Micro$oft 5.1  (16 bit)
rem *
rem *
del lib_mapm.lib
del mapm*.obj
rem *
rem *
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_add.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_mul.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_div.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_set.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_log.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_exp.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_sin.c
cl /c /AL /NT MAPM_TEXT /Oi /Gs /Zl mapmutil.c
cl /c /AL /NT MAPM_TEXT /Oi /Gs /Zl mapmfmul.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmasin.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmfact.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmrsin.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm5sin.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmsqrt.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmcbrt.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmcnst.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmstck.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmgues.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmistr.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmipwr.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_rnd.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmhsin.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapmhasn.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_pow.c
cl /c /AL /NT MAPM_TEXT /Gs /Zl mapm_fft.c
lib @mapm.rsp
del mapm*.obj
