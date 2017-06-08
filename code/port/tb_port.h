
#ifndef TB_PORT_H
#define TB_PORT_H

#ifdef _MSC_VER
//#include <yvals.h>

// streamsize is defined in iosfwd but we don't
// include it because it can only used with the new templatized 
// <iostream> includes, which we don't use (yet). 

// typedef size_t streamsize; 
#endif // _MSV_VER

#endif 
