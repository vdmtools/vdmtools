#ifndef X_FSTREAM_H
#define X_FSTREAM_H

#ifdef __GNUC__
#error Wrong fstream.h included! This is a local fake one. 
#endif

// Dummy default fstream.h for VC++

#if !defined(_MSC_VER) && !defined(__alpha)
#error Expected only to be used with VC++ 
#endif

#if defined(_MSC_VER) || defined(__alpha)
#if defined(__alpha)
#  include <unistd.h>
#endif
#include <fstream>
using namespace std; 
#endif

#endif
