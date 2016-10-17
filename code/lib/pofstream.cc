/***
*  * WHAT
*  *    Subclass for ofstream, where you can check if the stream is open
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/pofstream.cc,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:36 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "pofstream.h"

pofstream::pofstream() { isopen = 0;}

pofstream::~pofstream(){}

int pofstream::is_open() { 
  return isopen;
}

void pofstream::popen(wstring st) { 
  isopen = 1; 
  open(st.c_str());
}

void pofstream::pclose() { 
  isopen = 0; 
  close();
}
