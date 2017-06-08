rem *
rem *
rem *   Build file for GCC under DOS / Win NT/9x
rem *
rem *
del lib_mapm.a
del mapm*.o
rem *
gcc -c -Wall -O mapm*.c
rem *
del mapmutil.o
del mapmfmul.o
rem *
gcc -c -Wall -O3 mapmutil.c
gcc -c -Wall -O3 mapmfmul.c
rem *
ar rc lib_mapm.a mapm*.o
rem *
del mapm*.o
rem *
gcc -Wall -O calc.c lib_mapm.a -s -o calc.exe -lm
gcc -Wall -O validate.c lib_mapm.a -s -o validate.exe -lm
gcc -Wall -O primenum.c lib_mapm.a -s -o primenum.exe -lm
rem *
rem *
rem *         to compile and link the C++ demo program
rem *         using the C++ MAPM wrapper class.
rem *         (default is commented out)
rem *
rem * gxx cpp_demo.cpp lib_mapm.a -s -o cpp_demo.exe -lm
rem *
