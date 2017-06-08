/***
*  * WHAT
*  *   $Locker:  $ $RCSfile: valuescanner.h,v $
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/valuescanner.h,v $
*  * VERSION
*  *    $Revision: 1.20 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * FORMAT
*  *    $State: Exp $
*  * AUTHOR
*  *    Author: Erik T. Nielsen, Michael Andersen + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) 1991-93 The Institute of Applied Computer Science
***/

#ifndef __valuescanner_h__
#define __valuescanner_h__

#include "metaiv.h"

#include <iostream>
#include <fstream>
#include <string>

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size)\
{\
 int c = VLLEXINPUT.getchars(buf, max_size);\
 result = (c==0) ? YY_NULL : c;\
}

class VLLXINPUT {
public:
  VLLXINPUT() : current_getchars(&VLLXINPUT::EMPTY_getchars), current_reread(&VLLXINPUT::EMPTY_reread) {}

  int getchars(char*p, int maxsz) { return (this->*current_getchars)(p, maxsz); }
  void reread () { (this->*current_reread)(); }

  void ReadFrom(const std::string & s)
  {
    st = s;
    s_index = 0;
    current_getchars = &VLLXINPUT::String_getchars;
    current_reread = &VLLXINPUT::String_reread;
  }
  void ReadFrom(istream & s)
  {
    is = &s;
    current_getchars = &VLLXINPUT::Stream_getchars;
    current_reread = &VLLXINPUT::Stream_reread;
  }

private: 
  int (VLLXINPUT::*current_getchars)(char*, int); // Points to currently used getchars function. 
  void (VLLXINPUT::*current_reread)(); // Points to currently used reread function. 

  void String_reread () { s_index = 0; } 
  void Stream_reread () { is->clear(); is->seekg(0); } 

  int String_getchars(char*p, int maxsize)
  {
    if (s_index == (int) st.length())
      return 0;
    else {
      int rest = st.length() - s_index;
      int m = rest < maxsize ? rest : maxsize;
      memcpy(p, &st[s_index], m);
      s_index += m;
      return m;
    }
  }

  int Stream_getchars(char*p, int maxsize)
  {
    if (! is->fail()) {
      is->read(p, maxsize);
      return is->gcount();
    } else {
      return 0;
    }
  }

  int EMPTY_getchars(char*,int) { return 0; }
  void EMPTY_reread() {}

  std::string st; 
  int s_index; 
  istream *is; 
}; 

extern VLLXINPUT VLLEXINPUT;

class VALUEPARSER
{
public:
  static string valueptextstring;
  static Generic * exprs;
  static Map * RecordNameToTag;
  static int ConvRealLit(const wchar_t*,double&);
};

#endif // __valuescanner_h__
//
