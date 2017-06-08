#ifndef TkTreeWin_h_
#define TkTreeWin_h_

#include <treecalc.h>
#include "metaiv.h"



///////////////////////////////////////////////////////////////
//// Class: TkTreeWin
//// Description: interface object to tcl code of drawing the
////              inheritance tree.
////////////////////////////////////////////////////////////////
class TkTreeWin : public Tree
{
public:
  TkTreeWin(Map);
  ~TkTreeWin(void) {}
  Sequence Layout(void);
protected:
  Record Line(const Node&,const Node&); 
  Int RealX(int) const;
  Int RealY(int) const;
};

#endif // TkTreeWin_h_
