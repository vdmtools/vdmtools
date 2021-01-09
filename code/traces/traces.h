#ifndef __traces_h__
#define __traces_h__

#include "metaiv.h"
#include "AS.h"
#include "SEM.h"

class EXPANDED
{
public:
  static Map ExpandSpecTraces(const TYPE_AS_Document & spec, const SET<TYPE_AS_Name> & clmod_s,
                              wostream & wos);

private:
#ifdef VDMSL
  static Map ExpandModuleTraces(const TYPE_AS_Module & mod, wostream & wos);
#endif // VDMSL
#ifdef VDMPP
  static Map ExpandClassTraces(const TYPE_AS_Class & cl, wostream & wos);
#endif // VDMPP
  static Map ExpandTraceDefs(const TYPE_AS_Name & clid, const MAP<TYPE_AS_NameList,
                             const TYPE_AS_TraceDefList> & def_m, wostream & wos);
  static Set ExpandTraceDefList(const TYPE_AS_TraceDefList & tdef_l, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandTraceDef(const TYPE_AS_TraceDefTerm & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandTraceApplyExpr(const TYPE_AS_TraceApplyExpr & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandTraceBracketedExpr(const TYPE_AS_TraceBracketedExpr & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandTraceConcurrentExpr(const TYPE_AS_TraceConcurrentExpr & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandQualifiedTrace(const TYPE_AS_QualifiedTrace & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandRepeatTrace(const TYPE_AS_RepeatTrace & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandQualifiedRepeatTrace(const TYPE_AS_QualifiedRepeatTrace & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandTraceDefAltn(const TYPE_AS_TraceDefAltn & tdef, const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static SET<TYPE_SEM_BlkEnv> ExpandTraceBind(const SEQ<TYPE_AS_TraceBind> & bind_l,
                                              const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set ExpandTraceRepeatPat(const TYPE_AS_TraceRepeatPattern & regexpr, const Set & expr_l_s);
  static Set ExpandRepeatFixed(const TYPE_AS_RepeatFixed & regexpr, const Set & expr_l_s);
  static Set ExpandRepeatInterval(const TYPE_AS_RepeatInterval & regexpr, const Set & expr_l_s);
  static SET<TYPE_SEM_BlkEnv> ExpandLocalBinding(const TYPE_AS_LocalTraceBind & bind,
                                                 const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static Set RepeatCombine(const Set & expr_l_s, int low, int high);
  static SET<TYPE_SEM_BlkEnv> ExpandLetBinding(const TYPE_AS_LetTraceBind & lbbind,
                                               const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static SET<TYPE_SEM_BlkEnv> ExpandLetBeBinding(const TYPE_AS_LetBeTraceBind & lbbind,
                                                 const SET<TYPE_SEM_BlkEnv> & ctx_s);
  static TYPE_SEM_VAL evaluateExpression(const TYPE_AS_Expr & expr, const TYPE_SEM_BlkEnv & cxt);
  static TYPE_SEM_BlkEnv MergeContextSet(const SET<TYPE_SEM_BlkEnv> & c_s);
  static SET<TYPE_SEM_BlkEnv> CombineContext(const SET<TYPE_SEM_BlkEnv> & c_s, const SET<TYPE_SEM_BlkEnv> & c_s2);
  static Set AddContextToExpr(const TYPE_AS_Expr & expr, const SET<TYPE_SEM_BlkEnv> & cxt_s);
  static Set AddContextToExprList(const SEQ<TYPE_AS_Expr> & e_l, const SET<TYPE_SEM_BlkEnv> & cxt_s);
  static Set CombineTraces(const Set & e_l_s1, const Set & e_l_s2);
  static SEQ<TYPE_AS_ValueDef> Context2ValShapeL(const MAP<TYPE_AS_Name, TYPE_SEM_ValTp> & id_m);
  static void PushCxt(const SET<TYPE_SEM_BlkEnv> & cxt_s);
  static void PopCxt();
  static Generic GetCurClass();

  static Tuple TypeBindToSetBind(const SEQ<TYPE_AS_MultBind> & bind_l);
  static Tuple TypeToSet(const TYPE_AS_Type & tp);

  static Map ConvertTestCases(const Map & cases, wostream & wos);
  static Set ConvertTestExprs(const Set & expr_l_s);

private:
  static int zeroOrMoreMax;
  static int oneOrMoreMax;
  static SEQ<TYPE_AS_Id> seqOfNames;
  static Generic curcl;         // [AS`Name]
  static Sequence cxt_s_stack;  // seq of set of SEM`BlkEnv
#ifdef VDMPP
  static Generic curobj;
#endif // VDMPP
};

#endif // __traces_h__

