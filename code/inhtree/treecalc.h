//
// Product:	VPP
// File:	tree.h
// Description:	Tree calculations
// Rev.:	1.2
// Last update:	94/06/30
// Author:	Arjen Duursma
//
// Copyright Cap Volmac, Division Professional & Service Development
// P.O. Box 2575
// 3500 GN Utrecht
// The Netherlands
// Tel: +33-3404-35411
//
// The copyright to the computer program(s) herein is the property
// of Cap Volmac, the Netherlands. The program(s) may be used and/
// or copied only with the written permission of Cap Volmac or in
// accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//

#ifndef tree_h_
#define tree_h_

static const wchar_t _sccs_id_tree[] = L"@(#)tree.h	1.2";

#include <iostream>
#include "contain.hP"

class Node;
class Shape;

typedef Node *	NodeP;
typedef const wchar_t *	charP;

///////////////////////////////////////////////////////////////////
//// Class: Shape.
//// Description: This class describes the graphical representation of the
////              inheritance tree, that is, the horizontal position.
//////////////////////////////////////////////////////////////////

class Shape {
public:
  Shape & operator+=(int);
  Shape & operator+=(Shape&);
  Shape(void) : offset(0) {}
  int Head(void) const { return lower.empty()?0:lower.head(); }
  int Offset(void) const { return offset; }
  const List<int>& Upper(void) const { return upper; }
	const List<int>& Lower(void) const { return lower; }
protected:
  List<int> upper; // A list of the highest position of a node relatively
                   // to the offset.
  List<int> lower; // A list of the lowest position of a node relatively
                   // to the offset.
  int offset;      // The offset of the node
};


///////////////////////////////////////////////////////////////////////
///// CLASS: Tree
///// Description:
/////        The tree class describes the inheritance tree, both
/////        with respect to topology and the graphical representation.
/////
///////////////////////////////////////////////////////////////////////

class Tree {
public:
  Tree(int u = 10) : shape(NULL), unit(u)  {}
  virtual ~Tree();
  
  // Adds a reletion between children and a parent.
  Tree& Add(charP, const List<charP> &);
  
  // Adds a relation between a child and a parent.
  Tree& Add(charP, charP);
  
  // Compute the layout (graphical representation) of the inheritance
  // tree
  void Layout();
  
  // The following functions returns the values of the tree
  // instance variables.
  int Unit() const { return unit; }
  const List<NodeP>& Nodes(void) const { return nodes; }
  const Shape* theShape(void) const { return shape; }
protected:
  friend class Node;
  
  // Add a relation between child and a parent.
  Tree& Add(charP, NodeP);
private:
  // Find a node of a name, and if the name is not in the inheritance
  // add it to the inheritance tree.
  NodeP Find(charP);

  // A list of all the nodes in the tree.
  List<NodeP> nodes;
  
  // The shape of the tree.
  Shape * shape;

  // The unit of the graphical distance.
  int unit;
};

/////////////////////////////////////////////////////////////////////////////
///// Class: Node
///// Description: This class represents a node in the inheritance tree.
/////              A node is described by a position (pos) and an offset.
/////              The position describes the vertical position, and the
/////              offset describes the horisontally position.
/////              Ex:
/////                     B
/////                   /
/////                 A - C
/////                   \      
/////                     D - E
/////                 A: pos= 0, offset = 60
/////                 B: pos= 1, offset = 0
/////                 C: pos= 1, offset = 60
/////                 D: pos= 1, offset = 120
/////                 E: pos= 2, offset = 180.
///////////////////////////////////////////////////////////////////////////

class Node {
public:

  // vertical position in the inheritance tree.
  int pos;
  // horistinal position in the inheritance tree.
  int offset;
  operator charP() const { return name; }
  const List<NodeP>& Children(void) const { return children; }
  const List<NodeP>& Parents(void) const { return parents; }

protected:
  friend class Tree;
  Node(charP _name) : pos(0), offset(0),
//    name(strdup(_name)), parent(0), shape(NULL), active(0) { }
    name(wcsdup(_name)), parent(0), shape(NULL), active(0) { }
  virtual ~Node() { delete shape; delete name; }
  Shape& Layout(int unit);
  void AddParent(NodeP);
  
  void Position(int,const NodeP); // vertical
  void Position(int);		  // horizontal
  int Root(void) const { return pos == 0; }
  const NodeP Parent(void) const { return parent; }
private:
  wchar_t* name;    // name of the class
  NodeP parent;  // the closest parent in the tree.
  Shape * shape; // The shape of the node (Layout informations)
  List<NodeP> children;  // A list of the children of the node.
  List<NodeP> parents;   // A list of all the direct parents of the node.
  int active;            // ???
};

ostream& operator<<(ostream&,const Shape&);
ostream& operator<<(ostream&,const Tree&);
ostream& operator<<(ostream&,const Node&);

#endif // tree_h_
