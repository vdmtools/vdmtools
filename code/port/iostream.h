#ifndef X_IOSTREAM_H
#define X_IOSTREAM_H

#ifdef __GNUC__
#error Wrong iostream.h included! This is a local fake one. 
#endif

// Dummy default iostream.h for VC++

#if !defined(_MSC_VER) && !defined(__alpha)
#error Expected only to be used with VC++ or alpha cxx 
#endif


#if defined(_MSC_VER) || defined(__alpha)
#include <iostream>
using namespace std; 
#endif

#endif
