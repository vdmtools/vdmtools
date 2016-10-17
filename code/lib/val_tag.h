/***
*  * WHAT
*  *    Separate tag definitions for VAL's and module GLOBAL
*  *    Implementation of
*  *             sem.vdm r1.18
*  *             global.vdm r1.31
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/val_tag.h,v $
*  * VERSION
*  *    $Revision: 1.18 $
*  * DATE
*  *    $Date: 2005/07/18 23:45:50 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/* Record tags for dynamic semantics and abstract syntax     */

/* VAL tags defined separately in val_tag.h as the must also */
/* be used in metaiv.cc for ConstructVAL() and val2generic() */

//#include "main_tag.h"

//#define BOOL_V TAG_TYPE_DYNSEM_SEM_BOOL
//#define NUM_V TAG_TYPE_DYNSEM_SEM_NUM
//#define CHAR_V TAG_TYPE_DYNSEM_SEM_CHAR
//#define QUOTE_V TAG_TYPE_DYNSEM_SEM_QUOTE 
//#define NIL_V TAG_TYPE_DYNSEM_SEM_NIL
//#define UNDEF_V TAG_TYPE_DYNSEM_SEM_UNDEF
//#define SEQ_V TAG_TYPE_DYNSEM_SEM_SEQ
//#define SET_V TAG_TYPE_DYNSEM_SEM_SET
//#define MAP_V TAG_TYPE_DYNSEM_SEM_MAP
//#define TUPLE_V TAG_TYPE_DYNSEM_SEM_TUPLE
//#define REC_V TAG_TYPE_DYNSEM_SEM_REC
//#define ExplFN_V TAG_TYPE_DYNSEM_SEM_ExplFN
//#define ImplFN_V TAG_TYPE_DYNSEM_SEM_ImplFN
//#define ExplOP_V TAG_TYPE_DYNSEM_SEM_ExplOP
//#define ImplOP_V TAG_TYPE_DYNSEM_SEM_ImplOP
//#define ExplPOLY_V TAG_TYPE_DYNSEM_SEM_ExplPOLY
//#define ImplPOLY_V TAG_TYPE_DYNSEM_SEM_ImplPOLY
//#define EXIT_V TAG_TYPE_DYNSEM_SEM_EXIT
//#define CONT_V TAG_TYPE_DYNSEM_SEM_CONT
//#define TOKEN_V TAG_TYPE_DYNSEM_SEM_TOKEN

//#define DLFN_V TAG_TYPE_DYNSEM_SEM_DLFN     
//#define DLOP_V TAG_TYPE_DYNSEM_SEM_DLOP
//#define OverOPFN_V TAG_TYPE_DYNSEM_SEM_OverOPFN
//#define CompExplFN_V TAG_TYPE_DYNSEM_SEM_CompExplFN

/* Record tags which should have been in the above */
//#define RETURN_V TAG_TYPE_DYNSEM_SEM_RETURN  

/* Record tags corresponding to the VDM++ part */
//#define OBJ_V TAG_TYPE_DYNSEM_SEM_OBJ
//#define OBJ_Ref_V TAG_TYPE_DYNSEM_SEM_OBJ_Ref
//#define OBJ_Ref_V_PP TAG_TYPE_DYNSEM_SEM_OBJ_Ref_PP // used to be -5:
                                                    // tag to pretty print.
//#define TOKEN_V_PP TAG_TYPE_DYNSEM_SEM_TOKEN_PP     // used to be -3
                                                    // tag to pretty print.

/* index numbers for SigmaClass */
// Updated from eval_global.vdm, 1.16, 1999-10-13 /etn
/*
#ifdef VICE
#define C_size         28
#else
#define C_size         25
#endif // VICE

#define C_inhcon        1
#define C_instvars      2
#define C_instvars_tp  3
#define C_inst_inv      4
#define C_inst_init_val 5
#define C_vls_def       6
#define C_vls_init      7
#define C_explfns       8
#define C_implfns       9
#define C_explops      10
#define C_implops      11
#define C_explpolys    12
#define C_implpolys    13
#define C_localtps     14     
#define C_recsel       15
#define C_localhchy    16
#define C_isinit       17
#define C_constructor  18
#define C_perm_pred    19
#define C_thread       20
#define C_all_fns_ops_polys 21
#define C_overloaded   22
#define C_statics      23
#define C_defaultcons  24
#define C_statichist   25

#ifdef VICE
#define C_depnms       26
#define C_depops       27
#define C_sys          28
#endif // VICE
*/

/* Enviroment list */
//unused #define BlkEnv TAG_TYPE_DYNSEM_SEM_BlkEnv
//#define ValTp  TAG_TYPE_DYNSEM_SEM_ValTp

