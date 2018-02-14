/***
*  * WHAT
*  *    Implementation of mod_patmat.vdm
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_patmat.h,v $
*  * VERSION
*  *    $Revision: 1.8 $
*  *    $Revision: 1.8 $
*  * DATE
*  *    $Date: 2006/04/04 00:11:16 $
*  * STATUS
*  *    Under development $State: Exp $
*  * PROJECT
*  *    AFRODITE
*  * AUTHOR
*  *    Poul Boegh Lassen (PBL) + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __mod_patmat_h__
#define __mod_patmat_h__

private:
  bool ContainsNotSupportedConstruct;
  SEQ<TYPE_CPP_Stmt> NotSupportedStmt;

private:
  bool ContainsNotSuppConstruct();
  SEQ<TYPE_CPP_Stmt> GetNotSuppStmt();
  void InitState_PM();
  Tuple CGPatternMatchExcl (const TYPE_AS_Pattern &,
                            const TYPE_CGMAIN_VT &,
                            const Set &,
                            const TYPE_CPP_Name &,
                            const Map &,
                            const Generic &,
                            bool);
  Tuple CGPatternMatch (const TYPE_AS_Pattern &,
                        const TYPE_CGMAIN_VT &,
                        const Set &,
                        const TYPE_CPP_Name &,
                        const Map &,
                        const Generic &,
                        bool);
  Tuple CGMatchPatternName (const TYPE_AS_PatternName &,
                            const TYPE_CGMAIN_VT &,
                            const Set &,
                            const TYPE_CPP_Name &,
                            const Map &,
                            const Generic &,
                            bool);
  Tuple CGMatchVal (const TYPE_AS_MatchVal & p,
                    const TYPE_CGMAIN_VT & rc,
                    const Set &,
                    const TYPE_CPP_Name & succ_v,
                    const Generic &,
                    bool);
  Tuple CGMatchSetEnumPattern (const TYPE_AS_SetEnumPattern &,
                               const TYPE_CGMAIN_VT &,
                               const Set &,
                               const TYPE_CPP_Name &,
                               const Map &,
                               const Generic &,
                               bool);
  SEQ<TYPE_CPP_Stmt> GenValSetToSeq (const TYPE_CPP_Name & tovar_v,
                                     const TYPE_CPP_Name & fromvar_v,
                                     const TYPE_REP_SetTypeRep & str);
  Tuple CGMatchSeqEnumPattern (const TYPE_AS_SeqEnumPattern &,
                               const TYPE_CGMAIN_VT &,
                               const Set &,
                               const TYPE_CPP_Name &,
                               const Map &,
                               const Generic &,
                               bool);
  Tuple CGMatchMapEnumPattern (const TYPE_AS_MapEnumPattern &,
                               const TYPE_CGMAIN_VT &,
                               const Set &,
                               const TYPE_CPP_Name &,
                               const Map &,
                               const Generic &,
                               bool);
  Tuple CGMatchRecordPattern (const TYPE_AS_RecordPattern &,
                              const TYPE_CGMAIN_VT &,
                              const Set &,
                              const TYPE_CPP_Name &,
                              const Map &,
                              const Generic &,
                              bool);
  Tuple CGMatchTuplePattern (const TYPE_AS_TuplePattern &,
                             const TYPE_CGMAIN_VT &,
                             const Set &,
                             const TYPE_CPP_Name &,
                             const Map &,
                             const Generic &,
                             bool);
  Tuple CGMatchSetUnionPattern (const TYPE_AS_SetUnionPattern &,
                                const TYPE_CGMAIN_VT &,
                                const Set &,
                                const TYPE_CPP_Name &,
                                const Map &,
                                const Generic &,
                                bool);
  Tuple CGMatchSeqConcPattern (const TYPE_AS_SeqConcPattern &,
                               const TYPE_CGMAIN_VT &,
                               const Set &,
                               const TYPE_CPP_Name &,
                               const Map &,
                               const Generic &,
                               bool);
  Tuple CGMatchMapMergePattern (const TYPE_AS_MapMergePattern &,
                               const TYPE_CGMAIN_VT &,
                               const Set &,
                               const TYPE_CPP_Name &,
                               const Map &,
                               const Generic &,
                               bool);
#ifdef VDMPP
  Tuple CGMatchObjectPattern (const TYPE_AS_ObjectPattern &,
                              const TYPE_CGMAIN_VT &,
                              const Set &,
                              const TYPE_CPP_Name &,
                              const Map &,
                              const Generic &,
                              bool);
#endif // VDMPP
  Tuple CGMatchList (const SEQ<TYPE_AS_Pattern> &,
                     const TYPE_CGMAIN_VT &,
                     const TYPE_CPP_Name &,
                     const Set &,
                     const Map &,
                     const Generic &,
                     bool);
  TYPE_AS_Pattern CheckSeqConcPattern(const TYPE_AS_Pattern & p);
  TYPE_REP_TypeRep GenEqType (const TYPE_REP_TypeRep & type);
  bool HasComplexPattern(const TYPE_AS_Pattern & p);
  int CountComplexPattern(const TYPE_AS_Pattern & p);
  int CountComplexPatternInBindList(const SEQ<TYPE_AS_MultBind> & bl);

#endif // __mod_patmat_h__
