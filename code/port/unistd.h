// Fake unistd.h for OS/2
//
#ifdef __GNUC__
#error Wrong unistd.h included! This is a local fake one. 
#endif

// Dummy default unistd.h for VC++ / alpha

#if !defined(_MSC_VER) && !defined(__alpha)
#error Expected only to be used with VC++ 
#endif
