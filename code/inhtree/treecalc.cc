//
// Product:	VPP
// File:	tree.cc
// Description:	Tree calculations
// Rev.:	1.1
// Last update:	94/06/29
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

static const wchar_t _sccs_id[] = L"@(#)tree.cc	1.1";

#include "treecalc.h"

// *************** Utilities ************************ //

#ifdef __GNUC__ // the things we do for gnu
void _dummy(ostream&s, List<int>& l) { if (0) s << l; }
#endif //__GNUC__

// ************** Implementation ******************** //


// ************** Tree implementations ************** //

Tree::~Tree()
{
	delete shape;
	List_iter<NodeP> next(nodes);
	NodeP * p;
	while ((p = next()) != 0)
	{
		delete *p;
	}
}

////////////////////////////////////////////////////////////////
//// Function: Tree:Add
//// Description: This function adds the relation parent to a list
////              of children.
//// Input: name: Name of the parent.
////        ch  : The list of children.
//// Output: The inheritance tree.
/////////////////////////////////////////////////////////////////

Tree& Tree::Add(charP name, const List<charP> & ch)
{
	NodeP p = Find(name);
	if (!p)
	{
		cerr << L"Error Tree::add unable to find node "
			<< name << '\n';
		return *this;
	}
	List_iter<charP> next(ch);
	charP * q;
	while ((q = next()) != 0)
	{
		Add(*q,p);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////
///// Function: Tree:Add
///// Description: This function adds the relation child and parent
/////              between name (the parent) and the child.
///// Input   : name: the name of the parent.
/////           child: the name of the child.
///// Output  : The inheritance tree.
///////////////////////////////////////////////////////////////////

Tree& Tree::Add(charP name, charP child)
{
	NodeP p = Find(name);
	if (!p)
	{
		cerr << L"Error Tree::add unable to find node "
			<< name << '\n';
		return *this;
	}
	Add(child,p);
	return *this;
}

///////////////////////////////////////////////////////////////////
///// Function: Tree::Add
///// Input   : name: the name of the node which has L"parent" as parent.
/////           parent: the node of the parent to be added as parent.
///// Output  : the inheritance tree.
///// Description:
/////           This function adds the parent L"parent" to the class "name"
/////           in the inheritance tree.
///////////////////////////////////////////////////////////////////


Tree& Tree::Add(charP name, NodeP parent)
{
	NodeP p = Find(name);
	if (!p)
	{
		cerr << L"Tree::add unable to find node "
			<< name << '\n';
		return *this;
	}
	p->AddParent(parent);
	return *this;
}

///////////////////////////////////////////////////////////////////
////// Function: Tree:Find
////// Input   : name: Name of a node (class).
////// Output  : A pointer to the node in the inheritance tree.
////// Description:
//////           Finds the corresponding node of the name L"name"
//////           in the inheritance tree. If the node does not exists
//////           in the inheritance tree, the function creates a node in
//////           the inheritance tree.
///////////////////////////////////////////////////////////////////
NodeP Tree::Find(charP name) 
{
	List_iter<NodeP> next(nodes);
	NodeP * p;
	while ((p = next()) != 0)
	{
//		if (strcmp((charP)(*(*p)),name) == 0)
		if (wcscmp((charP)(*(*p)),name) == 0)
		{
			return *p;
		}
	}
	NodeP tmp = new Node(name);
	nodes.app(tmp);
	return tmp;
}

///////////////////////////////////////////////////////////////////
///// Function: Tree:Layout
///// Input : none.
///// Output : none.
///// Description: This function computes the horisontal layout of the
/////              inheritance tree.
///////////////////////////////////////////////////////////////////


void Tree::Layout()
{
	// Shape from the roots

	if (shape) delete shape;
	shape = new Shape;

	List<NodeP> roots;
	List_iter<NodeP> next(nodes);
	NodeP * p;
	while ((p = next()) != 0)
	{
		if ((*p)->Root())
		{
			roots.app(*p);
			*shape += (*p)->Layout(unit);
		}
	}

	// position the roots
	next = roots;
	while ((p = next()) != 0)
		(*p)->Position(0);
}

// ************ Node implementations ****************** //


///////////////////////////////////////////////////////////////
///// Function: Node::AddParent
///// Input   : parent: is the node which is the parent to be added.
///// Output  : none.
///// Description: The node L"parent" will be added as parent.
/////
///////////////////////////////////////////////////////////////

void Node::AddParent(NodeP parent)
{
	parents.app(parent);
	NodeP p = this;
	(parent->children).app(p);
        // Set the vertical position of the node and its children.
	Position(parent->pos + 1,parent);
}

/////////////////////////////////////////////////////////////////
//// Function: Node::Position
//// Input   : minpos: The minimum vertical position of the node.
////           parent: the node of the parent.
//// Output  : none.
//// Description: This function sets the position of the node and
////              all its children when adding a parent to the node. 
////
////////////////////////////////////////////////////////////////

void Node::Position(int minpos, const NodeP parent)
{
  if (active) return; // loop detection
  
  active = TRUE;

  if (minpos > pos)
    // if the current vertical position of the node is lower
    // than the new position.
    {
      //The parent is now a L"direct" parent.
      Node::parent = parent;
      // set the new position of the node.
      pos = minpos++;
      // Set the new position of the children of the node.
      List_iter<NodeP> next(children);
      NodeP * p;
      while ((p = next()) != 0)
		{
                  (*p)->Position(minpos,this);
		}
    }
  active = FALSE;
}


///////////////////////////////////////////////////////////////
//// Function: Node::Position
//// Input   : offset the offset of the node.
//// Output  : none.
//// Description: This function computes the horisontical position
////              of the node and its children.
////              The position of a node is based on the head of the lower
////              list + the offset parameter + the offset of the shape of
////              the node.
///////////////////////////////////////////////////////////////

void Node::Position(int offset)
{
  Node::offset = shape->Head() + offset + shape->Offset();
  List_iter<NodeP> next(children);
  NodeP * p;
  while ((p = next()) != 0)
    {
      if (((*p)->Parent() == this) && ((*p)->pos > pos))
        {
          (*p)->Position(offset + shape->Offset());
        }
    }
}

///////////////////////////////////////////////////////////////
//// Function: Node::Layout
//// Input   : unit: The unit of the graphical lenght.
//// Output  : shape of the node.
//// Description: This function computes the layout of the node and
////              and all its children.
////////////////////////////////////////////////////////////////

Shape & Node::Layout(int unit)
{	
	if (shape) delete shape;

	shape = new Shape;

	List_iter<NodeP> next(children);
	NodeP * p;
	while ((p = next()) != 0)
	{
		if (((*p)->Parent() == this) && ((*p)->pos > pos))
		{
			*shape += (*p)->Layout(unit);
		}
	}
	*shape += unit;
	return *shape;
}

// ************* Shape implementations *************** //

//////////////////////////////////////////////////////////////////
//// operator: Shape::operator+=
//// Input   : sh: The shape of a child.
//// Output  : sh: The shape of a child.
//// Description: This function computes the layout of the horisontal
////              values in the inheritance tree. The object itself is
////              the parent of the object sh.
////              
//////////////////////////////////////////////////////////////////


Shape & Shape::operator+=(Shape& sh)
{
  List_iter<int> nextu(upper);
  List_iter<int> nextl(sh.lower);
  int * low;
  int * up;
  int dist = 0;
  
  // determine distance
  // dist = uppper - sh.lower. The distance is the lowest positioned
  // child to the highest positioned child.
  while (((low = nextl()) != 0) && ((up = nextu()) != 0))
    {
      if ((*up - *low) > dist) dist = (*up - *low);
    }
  
  // make new upperbound
  List_iter<int> nexto(sh.upper);
  List_iter<int> nextn(upper);
  int * p;
  int * q;
  while (((p = nexto()) != 0) && ((q = nextn()) != 0))
    // iterates through the upper lists of the two shapes
    // and adds the upper list with the distance dist.
    {
      *q = *p + dist;
    }
  if (p)
    // if len upper < len sh upper.
    // then adjust the upper list to the same length.
    {	
      upper.app(*p + dist);
      while ((p = nexto()) != 0) upper.app(*p + dist);
    }
  
  // make new lowerbound
  nexto = sh.lower;
  nextn = lower;
  while (((p = nexto()) != 0) && ((q = nextn()) != 0));
  if (p)
    {
      lower.app(*p + dist);
      while ((p = nexto()) != 0) lower.app(*p + dist);
    }
  sh.offset = dist; // tell the relative position
  return *this;
}

///////////////////////////////////////////////////////////////
///// operator: Shape::operator+=
///// Input   : unit: the graphical unit of length.
////            shape: the shape
////  Output  : shape.
////  Description: If the shape is a lower and upper list is constructed
////               with 0 and the unit values.
////               Otherwise the lower and upper values for the shape
////               is computed for the shape.
/////////////////////////////////////////////////////////////////


Shape & Shape::operator+=(int unit)
{
	if (lower.empty())
	{
		lower.add(0);
		upper.add(unit);
		return *this;
	}
	lower.add((upper.head() + lower.head() - unit)/2);
	upper.add(lower.head() + unit);
	return *this;
}

// ************** Input/Output routines *************** //

ostream& operator<<(ostream& s,const Node& nd)
{
	return s << (charP) nd << L"(" << nd.pos << ',' << nd.offset << ")";
}

ostream& operator<<(ostream& s,const Shape& sh)
{
	return s << '(' << sh.Offset() << ','
		<< sh.Lower() << ',' << sh.Upper() << L")";
}

ostream& operator<<(ostream& s,const Tree& tr)
{
	NodeP * p;
	List_iter<NodeP> next(tr.Nodes());
	while ((p = next()) != 0)
	{
		s << *(*p) << '\n';
	}
	if (tr.theShape())
		s << *(tr.theShape()) << '\n';
	return s;
}
