/***
*  * WHAT
*  *    Separate tag definitions for VAL's
*  *    Implementation of module SEM r1.13
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cgip/val_tag.h,v $
*  * VERSION
*  *    $Revision: 1.1.1.1 $
*  * DATE
*  *    $Date: 1997/01/15 08:31:22 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: henrik $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/* Record tags for dynamic semantics and abstract syntax     */

/* VAL tags defined separately in val_tag.h as the must also */
/* be used in metaiv.cc for ConstructVAL() and val2generic() */

#define BOOL_V 95
#define NUM_V 96
#define CHAR_V 97
#define QUOTE_V 98 
#define NIL_V 99
#define UNDEF_V 100
#define FN_V 101 /* not used anymore */
#define OP_V 102 /* not used anymore */
#define SEQ_V 103
#define SET_V 104
#define MAP_V 105
#define TUPLE_V 106
#define REC_V 107
#define ExplFN_V 149
#define ImplFN_V 150
#define ExplOP_V 151
#define ImplOP_V 152
#define ExplPOLY_V 153
#define ImplPOLY_V 154
#define EXIT_V 155
#define CONT_V 156
#define TOKEN_V 158

#define ExtFN_V          315
#define ExtOP_V          316

/* Record tags which should have been in the above */
#define RETURN_V      350

/* the tag PosInfo is used to indicate a record with position information */
/* this is used in comparing record values */

#define PosInfo -2
#define CoverageCounter -4
