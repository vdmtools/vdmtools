/***
*  * WHAT
*  *    A function which converts an Abstract Syntax Tree with Integers into
*  *    an Abstract Syntax Tree with Quotes
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/utils/intconvquotes.cc,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE 
*  *    $Date: 2006/03/14 07:15:23 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    Henrik Voss + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "intconvquotes.h"
#include "asquotes.h"
#include "cgtag.h"
#include "tagmap.h"
#include "astaux.h"
#include "ProjectTypes.h"
#include "INSTRTP.h"
#include "stackeval.h"
#include "tbutils.h"
#include "pattern.h"

TagsTable INT2Q::tag_table;

bool INT2Q::IsCPPRec(const Record & r)
{
  switch (r.GetTag()) {
    case TAG_TYPE_CPP_File:
    case TAG_TYPE_CPP_IdMacroDef:
    case TAG_TYPE_CPP_FctMacroDef:
    case TAG_TYPE_CPP_SquareIncl:
    case TAG_TYPE_CPP_QuoteIncl:
    case TAG_TYPE_CPP_IncludeStmt:
    case TAG_TYPE_CPP_PreIf:
    case TAG_TYPE_CPP_PreIfDef:
    case TAG_TYPE_CPP_PreIfNotDef:
    case TAG_TYPE_CPP_PreDefine:
    case TAG_TYPE_CPP_PreElse:
    case TAG_TYPE_CPP_PreEndIf:
    case TAG_TYPE_CPP_PreMacro:
    case TAG_TYPE_CPP_PackageAndImportDeclarations:
    case TAG_TYPE_CPP_PackageDeclaration:
    case TAG_TYPE_CPP_QualifiedPackageName:
    case TAG_TYPE_CPP_SimplePackageName:
    case TAG_TYPE_CPP_SingleTypeImportDeclaration:
    case TAG_TYPE_CPP_TypeImportOnDemandDeclaration:
    case TAG_TYPE_CPP_IdentDeclaration:
    case TAG_TYPE_CPP_StorageClassSpecifier:
    case TAG_TYPE_CPP_TypeSpecifier:
    case TAG_TYPE_CPP_ArraySpecifier:
    case TAG_TYPE_CPP_FctSpecifier:
    case TAG_TYPE_CPP_Friend:
    case TAG_TYPE_CPP_TypeDef:
    case TAG_TYPE_CPP_Modifier:
    case TAG_TYPE_CPP_EnumSpecifier:
    case TAG_TYPE_CPP_EnumDefaultAssgn:
    case TAG_TYPE_CPP_ElaboratedTypeSpecifier:
    case TAG_TYPE_CPP_InitDeclarator:
    case TAG_TYPE_CPP_AsgnInit:
    case TAG_TYPE_CPP_ObjectInit:
    case TAG_TYPE_CPP_FctDecl:
    case TAG_TYPE_CPP_ArrayDecl:
    case TAG_TYPE_CPP_TypeName:
    case TAG_TYPE_CPP_FctAbsDecl:
    case TAG_TYPE_CPP_ArrayAbsDecl:
    case TAG_TYPE_CPP_ArgumentDeclarationList:
    case TAG_TYPE_CPP_ArgDecl:
    case TAG_TYPE_CPP_DefaultArgDecl:
    case TAG_TYPE_CPP_ArgAbsDecl:
    case TAG_TYPE_CPP_DefaultArgAbsDecl:
    case TAG_TYPE_CPP_EnumInit:
    case TAG_TYPE_CPP_InitializerList:
    case TAG_TYPE_CPP_IndirectionDecl:
    case TAG_TYPE_CPP_RefDecl:
    case TAG_TYPE_CPP_PointerToMember:
    case TAG_TYPE_CPP_BracketedDecl:
    case TAG_TYPE_CPP_RefAbsDecl:
    case TAG_TYPE_CPP_IndirectionAbsDecl:
    case TAG_TYPE_CPP_PointerToMemberAbs:
    case TAG_TYPE_CPP_BracketedAbsDecl:
    case TAG_TYPE_CPP_ClassSpecifier:
    case TAG_TYPE_CPP_ClassHead:
    case TAG_TYPE_CPP_MemberList:
    case TAG_TYPE_CPP_MemberSpecifier:
    case TAG_TYPE_CPP_VirCompleteClassSpecifier:
    case TAG_TYPE_CPP_AccCompleteClassSpecifier:
    case TAG_TYPE_CPP_DeclPureSpec:
    case TAG_TYPE_CPP_StaticInitializer:
    case TAG_TYPE_CPP_TemplateName:
    case TAG_TYPE_CPP_TemplateClassName:
    case TAG_TYPE_CPP_InterfaceSpecifier:
    case TAG_TYPE_CPP_InterfaceHead:
    case TAG_TYPE_CPP_FunctionDefinition:
    case TAG_TYPE_CPP_MemInitializer:
    case TAG_TYPE_CPP_FctBody:
//    case TAG_TYPE_CPP_ClassResScopeQualifiedClassName:
    case TAG_TYPE_CPP_ResScopeQualifiedClassName:
    case TAG_TYPE_CPP_Char:
    case TAG_TYPE_CPP_Short:
    case TAG_TYPE_CPP_Int:
    case TAG_TYPE_CPP_Bool:
    case TAG_TYPE_CPP_Long:
    case TAG_TYPE_CPP_Float:
    case TAG_TYPE_CPP_Double:
    case TAG_TYPE_CPP_Byte:
    case TAG_TYPE_CPP_Void:
    case TAG_TYPE_CPP_EnumName:
    case TAG_TYPE_CPP_Signed:
    case TAG_TYPE_CPP_Unsigned:
    case TAG_TYPE_CPP_TypeDefName:
    case TAG_TYPE_CPP_LabelStmt:
    case TAG_TYPE_CPP_SwitchStmt:
    case TAG_TYPE_CPP_SwitchBlock:
    case TAG_TYPE_CPP_SwitchBlockStmtGrp:
    case TAG_TYPE_CPP_SwitchLabels:
    case TAG_TYPE_CPP_CaseStmt:
    case TAG_TYPE_CPP_DefaultStmt:
    case TAG_TYPE_CPP_ExpressionStmt:
    case TAG_TYPE_CPP_CompoundStmt:
    case TAG_TYPE_CPP_ForStmt:
    case TAG_TYPE_CPP_Break:
    case TAG_TYPE_CPP_Continue:
    case TAG_TYPE_CPP_Return:
    case TAG_TYPE_CPP_DeclarationStmt:
    case TAG_TYPE_CPP_IfStmt:
    case TAG_TYPE_CPP_WhileStmt:
    case TAG_TYPE_CPP_DoWhileStmt:
    case TAG_TYPE_CPP_Goto:
    case TAG_TYPE_CPP_SynchronizedStmt:
    case TAG_TYPE_CPP_EmptyStmt:
    case TAG_TYPE_CPP_AssignExpr:
    case TAG_TYPE_CPP_AssignOp:
    case TAG_TYPE_CPP_ConditionalExpr:
    case TAG_TYPE_CPP_LogicalExpr:
    case TAG_TYPE_CPP_LogOp:
    case TAG_TYPE_CPP_BitExpr:
    case TAG_TYPE_CPP_BitOp:
    case TAG_TYPE_CPP_TypeCompExpr:
    case TAG_TYPE_CPP_TcOp:
    case TAG_TYPE_CPP_EqualityExpr:
    case TAG_TYPE_CPP_EqOp:
    case TAG_TYPE_CPP_RelationalExpr:
    case TAG_TYPE_CPP_RelOp:
    case TAG_TYPE_CPP_ShiftExpr:
    case TAG_TYPE_CPP_ShOp:
    case TAG_TYPE_CPP_BinExpr:
    case TAG_TYPE_CPP_BinOp:
    case TAG_TYPE_CPP_CastExpr:
    case TAG_TYPE_CPP_PreInDecrementExpr:
    case TAG_TYPE_CPP_UnaryOpExpr:
    case TAG_TYPE_CPP_UnaryOp:
    case TAG_TYPE_CPP_PostFixInDecrementExpr:
    case TAG_TYPE_CPP_IntegerLit:
    case TAG_TYPE_CPP_CharacterLit:
    case TAG_TYPE_CPP_WCharacterLit:
    case TAG_TYPE_CPP_FloatingLit:
    case TAG_TYPE_CPP_StringLit:
    case TAG_TYPE_CPP_WStringLit:
    case TAG_TYPE_CPP_BoolLit:
    case TAG_TYPE_CPP_BracketedExpr:
    case TAG_TYPE_CPP_This:
    case TAG_TYPE_CPP_FctCall:
    case TAG_TYPE_CPP_ArrayApply:
    case TAG_TYPE_CPP_ExplTypeConv:
    case TAG_TYPE_CPP_ObjectMemberAccess:
//    case TAG_TYPE_CPP_ScopeResIdentifier:
//    case TAG_TYPE_CPP_ScopeResOpFctName:
//    case TAG_TYPE_CPP_ScopeResQualifiedName:
    case TAG_TYPE_CPP_Identifier:
    case TAG_TYPE_CPP_QualifiedName:
    case TAG_TYPE_CPP_Destructor:
    case TAG_TYPE_CPP_PmExpr:
    case TAG_TYPE_CPP_PmOp:
    case TAG_TYPE_CPP_SizeOfExpr:
    case TAG_TYPE_CPP_SizeOfType:
    case TAG_TYPE_CPP_AllocationTypeExpr:
    case TAG_TYPE_CPP_AllocationNewTypeExpr:
    case TAG_TYPE_CPP_DeallocationExpr:
    case TAG_TYPE_CPP_DeallocationArrayExpr:
    case TAG_TYPE_CPP_NewTypeName:
    case TAG_TYPE_CPP_ArrayCreationExpr:
    case TAG_TYPE_CPP_ArrayInitializer:
    case TAG_TYPE_CPP_CVNewDeclarator:
    case TAG_TYPE_CPP_ClassNewDecl:
    case TAG_TYPE_CPP_ArrayNewDecl:
    case TAG_TYPE_CPP_NewInitializer:
    case TAG_TYPE_CPP_PointerToObjectMemberAccess:
    case TAG_TYPE_CPP_ClassInstanceCreationExpr:
    case TAG_TYPE_CPP_ClassExpr:
#ifdef VDMPP
    case TAG_TYPE_CPP_ConversionFunctionName:
    case TAG_TYPE_CPP_ConversionTypeName:
    case TAG_TYPE_CPP_PointerDecl:
    case TAG_TYPE_CPP_RefTypeDecl:
    case TAG_TYPE_CPP_PointerToMemberDecl:
    case TAG_TYPE_CPP_Super:
#endif // VDMPP
    case TAG_TYPE_CPP_NullLit:
#ifdef VDMPP
    case TAG_TYPE_CPP_OperatorFunctionName:
    case TAG_TYPE_CPP_Operator:
#endif // VDMPP
    case TAG_TYPE_CPP_TryBlock:
    case TAG_TYPE_CPP_Handler:
    case TAG_TYPE_CPP_All:
    case TAG_TYPE_CPP_ExceptionDeclaration:
    case TAG_TYPE_CPP_ThrowExpression:
    case TAG_TYPE_CPP_BlockComments:
    case TAG_TYPE_CPP_SingleLineComments:
    case TAG_TYPE_CPP_TrailingComments:
    case TAG_TYPE_CPP_EndOfLineComments:
    case TAG_TYPE_CPP_DocComments:
      return true;
   default:
     return false;
  }
} 

// h2gAS
// Convert a handwritten AS to a generated AS
Generic INT2Q::h2gAS(const Generic & g)
{
  return TransformIntegers(g);
}

// g2hAS
// Convert a generated AS to a handwritten AS
Generic INT2Q::g2hAS(const Generic & g)
{
  return TransformIntegersBack(g);
}

// Integer2Quote
// Convert a Int to a Quote
Quote INT2Q::Integer2Quote(const Int & I)
{
  Quote q (tag_table.GetKey(I));
  if (q != Quote(L""))
    return q;
#ifdef _MSC_VER
  wcerr << L"Integer2Quote: UNDEFINED QUOTE (" << I << L")" << endl;
#else
  vdm_log << L"Integer2Quote: UNDEFINED QUOTE (" << I << L")" << endl;
#endif // _MSC_VER
  return Quote (L"UNDEFINED");
}

// TransformIntegers
Generic INT2Q::TransformIntegers (const Generic & g)
{
  if (g.IsInt()) {
    if (Int(g).GetValue() < CG_OFFSET)
      return g;
    Quote q (Integer2Quote(g));
    return q;
  }
  else if (g.IsSequence())
  {
    // SEQUENCE CASE
    Sequence Sq (g);
    Sequence Sr;
    Generic Element;
    size_t len_Sq = Sq.Length();
    for (size_t idx = 1; idx <= len_Sq; idx++)
      Sr.ImpAppend (TransformIntegers (Sq[idx]));
    return Sr;
  }
  else if (g.IsRecord())
  {
    // RECORD CASE 
    Record Rc (g);
    int Tag = Rc.GetTag();
      
    switch(Tag) {

      //////////////////
      //  FROM AS
      //////////////////

      // SPECIAL CASES
      case TAG_TYPE_CI_TokenSpan: {
        return g;
      }

      case TAG_TYPE_CI_ContextNodeInfo: {
        TYPE_CI_ContextNodeInfo cni (g);
        TYPE_CI_ContextNodeInfo ni (cni);
        ni.SetField(pos_CI_ContextNodeInfo_typeinfo, TransformIntegers(cni.GetField(pos_CI_ContextNodeInfo_typeinfo)));
        ni.SetField(pos_CI_ContextNodeInfo_pid_um,   TransformIntegers(cni.GetField(pos_CI_ContextNodeInfo_pid_um)));
        return ni;
      }

      case TAG_TYPE_ProjectTypes_ErrMsg:
      case TAG_TYPE_ProjectTypes_WarnMsg: {
        return g;
      }

      case TAG_TYPE_INSTRTP_BR:
      case TAG_TYPE_INSTRTP_CBR:
      case TAG_TYPE_INSTRTP_POP:
      case TAG_TYPE_INSTRTP_CNBR:
      case TAG_TYPE_INSTRTP_FREF:
      case TAG_TYPE_INSTRTP_SIZE:
      case TAG_TYPE_INSTRTP_ATOMIC:
      case TAG_TYPE_INSTRTP_HANDID:
      case TAG_TYPE_INSTRTP_ISTART:
      case TAG_TYPE_INSTRTP_MKEXIT:
      case TAG_TYPE_INSTRTP_PUSHTH:
      case TAG_TYPE_INSTRTP_RECMOD:
      case TAG_TYPE_INSTRTP_CONTEXT:
      case TAG_TYPE_INSTRTP_SEQCONC:
      case TAG_TYPE_INSTRTP_SETUNION:
      case TAG_TYPE_INSTRTP_SELBLKENV:
//      case TAG_TYPE_INSTRTP_POPCONTEXT: obsolete
      case TAG_TYPE_INSTRTP_REMSTACKELEM:
      case TAG_TYPE_INSTRTP_SEQELEMMATCH:
      case TAG_TYPE_INSTRTP_MOSREF:
#ifdef VDMPP
      case TAG_TYPE_INSTRTP_INITCLASS:
      case TAG_TYPE_INSTRTP_RECCONS:
      case TAG_TYPE_INSTRTP_OBJECTPAT:
      case TAG_TYPE_INSTRTP_FIELDPAT:
#endif // VDMPP
      case TAG_TYPE_INSTRTP_SETENUM:
      case TAG_TYPE_INSTRTP_SEQENUM:
      case TAG_TYPE_INSTRTP_MAPENUM:
      case TAG_TYPE_INSTRTP_SETCONS:
      case TAG_TYPE_INSTRTP_SEQCONS:
      case TAG_TYPE_INSTRTP_MAPCONS:
      case TAG_TYPE_INSTRTP_TUPPATCONS:
      case TAG_TYPE_INSTRTP_RECPATCONS:
      case TAG_TYPE_INSTRTP_MAPLET:
        return g;

      case TAG_TYPE_INSTRTP_POSTENV: {
        Rc.SetField(1, TransformIntegers(Rc.GetField(1)));
        return Rc;
      }
 
      case TAG_TYPE_INSTRTP_MULTBINDL: {
        Quote partition (Rc.GetField(2) == Int(DO_PARTITION) ? Quote(L"DO_PARTITION") : Quote(L"DONT_PARTITION"));
        //Rc.SetField(2, TransformIntegers(Rc.GetField(2)));
        Rc.SetField(2, partition);
        return Rc;
      }
 
      //////////////////
      //  FROM CPP
      //////////////////

      case TAG_TYPE_CPP_IntegerLit: {
        return g;
      }

      case TAG_TYPE_STKM_Trap: {
        return g;
      }

      case TAG_TYPE_STKM_CallStackItem: {
        int len = Rc.Length();
        for(int i = 1; i <= len; i++) {
          if (i < 7)
            Rc.SetField(i, TransformIntegers(Rc.GetField(i)));
        }
        return(Rc);
      }

      case TAG_TYPE_STKM_EvaluatorStatus: {
        int len = Rc.Length();
        for(int i = 1; i <= len; i++) {
          switch(i) {
            case pos_TYPE_STKM_EvaluatorStatus_PC:
            case pos_TYPE_STKM_EvaluatorStatus_curCid:
#ifdef VDMPP
            case pos_TYPE_STKM_EvaluatorStatus_instrno:
#ifdef VICE
            case pos_TYPE_STKM_EvaluatorStatus_release_time:
#endif // VICE
#endif // VDMPP
            case pos_TYPE_STKM_EvaluatorStatus_cf:
            {
              break;
            }
            default: {
              Rc.SetField(i, TransformIntegers(Rc.GetField(i)));
              break;
            }
          }
        }
        return(Rc);
      }

      default: {
        int Length;
        if (!ASTAUX::IsASTRec(Rc) && !IsCPPRec(Rc))
          Length = Rc.Length();
        else
          // In this case
          // the last entry is the record is an context identifier
          // and it should not be transformed.
          Length = Rc.Length() -1;

        for(int i = 1; i <= Length; i++) {
          Rc.SetField(i, TransformIntegers(Rc.GetField(i)));
        }
        return(Rc);
      }
    }
  }
  else if (g.IsTuple())
  { 
    // TUPLE CASE
    Tuple Tp(g);
    int Length = Tp.Length();
     
    for (int i = 1; i <= Length; i++) {
      Tp.SetField(i, TransformIntegers(Tp.GetField(i)));
    }
    return(Tp);
  }
  else if (g.IsSet())
  {
    // SET CASE
    Set St(g);
    Set Rs;
    Generic Element;
    for (bool bb = St.First (Element); bb; bb = St.Next (Element))
      Rs.Insert (TransformIntegers (Element));

    return(Rs);
  }
  else if (g.IsMap())
  {
    // MAP CASE
    Map Mp (g);
    Set dom_Mp(Mp.Dom());
    Map Mr;
    Generic Element;
    for (bool bb = dom_Mp.First (Element); bb; bb = dom_Mp.Next (Element))
      Mr.Insert (TransformIntegers (Element), TransformIntegers (Mp [Element]));
    return(Mr);
  }
  else
    return g;
}

//////////////////////////////////////////////////////////////////////////
// T R A N S F O R M    B A C K
//////////////////////////////////////////////////////////////////////////
// In order to handle the strange way in which quotes are handled in the
// handwritten code, a hack is used in order to determine if e.g. the
// quote <LE> originates from AS or from CPP, as these are represented
// differently in the code - really nice

// The hack is to use a record tag value to indicate if we have got a 
// AS or an CPP abstract tree.
//////////////////////////////////

// Quote2Integer
// Convert a Quote to a Int
Int INT2Q::Quote2Integer(const Quote & q)
{
  Int q_val (tag_table.Compute(q));
  if (q_val == Int(0))
#ifdef _MSC_VER
    wcerr << L"Quote2Integer: UNDEFINED QUOTE (" << q << L")" << endl;
#else
    vdm_log << L"Quote2Integer: UNDEFINED QUOTE (" << q << L")" << endl;
#endif // _MSC_VER
  return(Int(q_val));
}

// TransformIntegersBack
Generic INT2Q::TransformIntegersBack (const Generic & g)
{
  if (g.IsQuote())
  {
    return Quote2Integer(g);
  }
  else if (g.IsSequence())
  {
    // SEQUENCE CASE
    Sequence Sq (g);
    Sequence Sr;
    size_t len_Sq = Sq.Length();
    for (size_t idx = 1; idx <= len_Sq; idx++)
      Sr.ImpAppend (TransformIntegersBack (Sq[idx]));
    return Sr;
  }
  else if (g.IsRecord())
  {
    // RECORD CASE 
    Record Rc (g);
    int Length = Rc.Length();
    for(int i = 1; i <= Length; i++)
      Rc.SetField(i, TransformIntegersBack(Rc.GetField(i)));
    return(Rc);
  }
  else if (g.IsTuple())
  {
    // TUPLE CASE
    Tuple Tp (g);
    int Length = Tp.Length();
     
    for (int i = 1; i <= Length; i++) {
      Tp.SetField(i, TransformIntegersBack(Tp.GetField(i)));
    }
    return(Tp);
  }
  else if (g.IsSet())
  {
    // SET CASE
    Set St (g);
    Set Rs;
    Generic Element;

    for (bool bb = St.First (Element); bb; bb = St.Next (Element))
      Rs.Insert (TransformIntegersBack (Element));

    return(Rs);
  }
  else if (g.IsMap())
  {
    // MAP CASE
    Map Mp (g);
    Map Mr;
    Set dom_Mp (Mp.Dom());
    Generic Element;
    for (bool bb = dom_Mp.First (Element); bb; bb = dom_Mp.Next (Element))
      Mr.Insert (TransformIntegersBack (Element), TransformIntegersBack (Mp [Element]));

    return(Mr);
  }
  else
    return g;
}

