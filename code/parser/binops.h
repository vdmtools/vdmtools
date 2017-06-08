/*
 *  WHAT
 *     Module: Byacc parser recovery.
 *     Map tokens to strings.
 *  IDENTIFICATION
 *     $Id: binops.h,v 1.18 2006/01/30 08:46:06 vdmtools Exp $
 *  AUTHOR
 *     ETN
 *  COPYRIGHT
 *     (C) Kyushu University
 */

#ifndef __binops_h__
#define __binops_h__

#include <string>

enum Token_Type { TT_COMMENT     = 1,
                  TT_DEF         = 2,
                  TT_RESERVE     = 3,
                  TT_OPERATOR    = 4,
                  TT_PARENTHESIS = 5,
                  TT_LITERAL     = 6,
                  TT_CHARLITERAL = 7,
                  TT_TEXBREAK    = 8,
                  TT_IDENT       = 9,
                  TT_DEFAULT     = 10,
                  TT_POLY        = 11};

class BINOPS
{
public:
  static std::wstring id2str( int id, const std::wstring & text );
  static Token_Type id2gtype( int id );
  static bool isIdentifier( int id );
  static bool isClassOrModule( int id );
  static std::wstring getLexText( int id, const std::wstring & text );
};

// 
#endif // __binops_h__

