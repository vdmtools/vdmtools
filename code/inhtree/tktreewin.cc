#include "tktreewin.h"
extern L"C" {
#include <tcl.h>
}

#include "projectval.h"

char buf[255];

extern Tcl_Interp * Vpp_Interp;
/////////////////////////////////////////////////////////////////////
//// Function: TkTreeWin::TkTreeWin
//// Input   : cl_s: map ModuleName to set of ModuleName.
////                 The cl_s maps every class to its direct subclasses
//// Description: The function is the constructor function of TkTreeWin.
////              The function constructs an inheritance tree containing
////              the topology of the inheritance, but not the lay-out
////              information.
////////////////////////////////////////////////////////////////////

TkTreeWin::TkTreeWin(Map cl_s) : Tree(30)
{
  
  Generic cl_g;
  Generic cl_subs_g;
  for (bool bb = cl_s.First(cl_g, cl_subs_g); bb; bb = cl_s.Next(cl_g, cl_subs_g)) {
    wstring cl_nm_str (Token (cl_g).GetString());
    const wchar_t* cl_nm_p= cl_nm_str.c_str();
    
                      
    //    const wchar_t *cl_nm_p (ExtractModuleName(cl_g));
    Tree::Add(cl_nm_p, List<const wchar_t *>());
    Set cl_subs(cl_subs_g);
    Generic sub_cl_g;
    for (bool cc = cl_subs.First(sub_cl_g); cc; cc = cl_subs.Next(sub_cl_g)) {
      wstring cl_nm_str(Token(sub_cl_g).GetString());
      const wchar_t *cl_nm_ch = cl_nm_str.c_str();
      Tree::Add(cl_nm_p, cl_nm_ch);
    }
  }
}

////////////////////////////////////////////////////////////////////
//// Function: TkTreeWin::Layout
//// Input: none.
//// Output: A set: set of (Node| Line). See also ptypes.vpp
//// Description: Computes the layout of the inheritance tree and returns
////              the graphical representation.
////
/////////////////////////////////////////////////////////////////////

Sequence TkTreeWin::Layout()
{
  /// Compute the layout of the tree (the shapes in the tree).
  Tree::Layout();

  // do the boxes
  List_iter<NodeP>  next(Nodes());
  NodeP * p;

  Sequence res; // res: seq of (Line| Node)
  
  while ((p = next()) != 0) {

    Int xpos(RealX((*p)->pos));
    Int ypos(RealY((*p)->offset));
    Token name ((charP)*(*p));

    res.ImpAppend(mk_Node(xpos, ypos, name));

    List_iter<NodeP> nextch((*p)->Children());
    NodeP * q;
    // write all the child/parent relations.
    while ((q = nextch()) != 0) {
      Record line(Line(*(*p),*(*q)));
      res.ImpAppend(line);
    }
  }
  return res;
}


///////////////////////////////////////////////////////////////////////
/// Function: TkTreeWin::Line
/// Input: x1, x2: the nodes in between a line should be drawn.
/// Output: none.
/// Description: This function prints a line between node x1 and x2
///              on the tcl canvas.
//////////////////////////////////////////////////////////////////////
Record TkTreeWin::Line(const Node& x1, const Node& x2)
{
  Int s1 = RealX(x1.pos);
  Int s2 = RealX(x2.pos);

  Int e1 = RealY(x1.offset);
  Int e2 = RealY(x2.offset);

  return (mk_Line(s1, e1, s2, e2));
}


Int TkTreeWin::RealX(int x) const
{
//  return (120 * x + 40);
  return Int(x);
}

Int TkTreeWin::RealY(int y) const
{
  return Int(y + 20);
}

