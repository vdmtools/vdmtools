/***
*  * WHAT
*  *    Subclass for ofstream, where you can check if the stream is open.
*  * ID
*  *    $Id: pofstream.h,v 1.8 2001/06/12 15:04:36 paulm Exp $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef POFSTREAM
#define POFSTREAM

#include "tb_port.h"

#include <fstream.h>
#ifndef HAS_ISTRING
#include "tb_string.h"
#else
#include <i0string.hpp>
#define String I0String
#endif
// #include L"stringext.h"


class pofstream : public ofstream {
  int isopen;
public:
  pofstream();
  ~pofstream();

  int is_open();
  void popen(wstring);
  void pclose();
};

#endif //POFSTREAM
