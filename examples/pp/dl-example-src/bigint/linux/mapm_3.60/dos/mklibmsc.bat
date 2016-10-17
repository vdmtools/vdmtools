rem *
rem *
rem *   library build file for Micro$oft 8.00c  (16 bit)
rem *
rem *
del lib_mapm.lib
del mapm*.obj
rem *
rem *
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_add.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_mul.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_div.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_set.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_log.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_exp.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_sin.c
cl /c /AL /NT MAPM_TEXT /Ocei /Gs /Zl mapmutil.c
cl /c /AL /NT MAPM_TEXT /Ocei /Gs /Zl mapmfmul.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmasin.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmfact.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmrsin.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm5sin.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmsqrt.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmcbrt.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmcnst.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmistr.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmstck.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmgues.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmipwr.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_rnd.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmhsin.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapmhasn.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_pow.c
cl /c /AL /NT MAPM_TEXT /Oce /Gs /Zl mapm_fft.c
lib @mapm.rsp
del mapm*.obj
