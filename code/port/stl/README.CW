Note For Metrowerks CodeWarrior Users

----------------------------------------------------------------------------

Known problems

Compiling

   * You may have to edit relative/full path to CodeWarrior <iterator.h> in
     the adaptation<msl_iterator.h>. Including CW version is necessary to
     get streambuf iterators defined.
   * You may have to edit full path to CW <algobase.h> in the
     adaptation<msl_algobase.h>. Including CW version is necessary to import
     auxiliary stuff used by valarrays.
   * To avoid editing the headers mentioned above, Metrowerks users should
     put the "SGI STL" directory in the "Metrowerks Standard Library"
     directory, and add the "SGI STL" directory as the first item in the
     system search paths of the projects that wish to use it.
   * If you want your code to be portable, you should avoid explicit usage
     of reverse_iterator<Iterator,T,T&,T*,Distance>. To be even more
     portable, never include <string>, too (not all compilers support that).
     You may use SGI STL rope<charT,Alloc> class instead of basic_string
     then. To be ulimately portable, never use STL ;).
   * It's better to use coherent set of headers in the project: all
     new-style, including STL ones;

----------------------------------------------------------------------------

Last updated : September 22, 1997.
