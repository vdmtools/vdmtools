#ifndef X_IOMANIP_H
#define X_IOMANIP_H

#ifdef __GNUC__
#error Wrong iomanip.h included! This is a local fake one. 
#endif

// Dummy default iomanip.h for VC++

#if !defined(_MSC_VER) && !defined(__alpha)
#error Expected only to be used with VC++ 
#endif

#if defined(_MSC_VER) || defined(__alpha)
#include <iomanip>
using namespace std; 
#endif

#endif
