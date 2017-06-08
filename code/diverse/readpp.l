%{
#include "tag.h"
#include "val_tag.h"

#ifdef _MSC_VER
#include <io.h>
#endif

char* ASCIItag(int);
%}

/* option main means flex provides a main routine.
 * this option implies option noyywrap */
%option main

%%
[0-9]+:		{ /* do not pass the ref count through the filter */ }
mk_unknown[0-9]+	{ int i, n;

		  for ( i = 0; (yytext[i] = yytext[i+10]) != '\0'; i++); 
		  /* remove 'mk_unknown' */

		  n = atoi (yytext);

		  printf ("mk_%s", ASCIItag(n));
		}
%%

char* ASCIItag (int tag)
{ switch (tag)
    { 
    case -2: return "PI"; break; 
#ifdef VDMSL
    case Module: return "AS`Module"; break;
    case DLModule: return "AS`ImplModule"; break;
    case DLInterface: return "AS`ImplInterface"; break;
    case DLImportSig: return "AS`ImplImportSig"; break;
    case DLExportSig: return "AS`ImplExportSig"; break;
    case Interface: return "AS`Interface"; break;
#endif //VDMSL
    case Definitions: return "AS`Definitions"; break; 
    case TypeDef: return "AS`TypeDef"; break; 
    case BasicType: return "AS`BasicType"; break; 
    case QuoteType: return "AS`QuoteType"; break; 
    case CompositeType: return "AS`CompositeType"; break; 
    case Field: return "AS`Field"; break; 
    case UnionType: return "AS`UnionType"; break; 
    case ProductType: return "AS`ProductType"; break; 
    case OptionalType: return "AS`OptionalType"; break; 
    case SetType: return "AS`SetType"; break; 
    case AllType: return "AS`AllType";
    case Seq0Type: return "AS`Seq0Type"; break; 
    case Seq1Type: return "AS`Seq1Type"; break; 
    case GeneralMapType: return "AS`GeneralMapType"; break; 
    case InjectiveMapType: return "AS`InjectiveMapType"; break; 
    case TypeName: return "AS`TypeName"; break; 
    case TypeVar: return "AS`TypeVar"; break; 
#ifdef VDMSL
    case StateDef: return "AS`StateDef"; break; 
    case StateInit: return "AS`StateInit"; break;
#endif //VDMSL
    case Invariant: return "AS`Invariant"; break;
    case DefStmt: return "AS`DefStmt"; break; 
    case LetStmt: return "AS`LetStmt"; break; 
    case LetBeSTStmt: return "AS`LetBeSTStmt"; break; 
    case AssignStmt: return "AS`AssignStmt"; break; 
    case SetForLoopStmt: return "AS`SetForLoopStmt"; break; 
    case IndexForLoopStmt: return "AS`IndexForLoopStmt"; break; 
    case WhileLoopStmt: return "AS`WhileLoopStmt"; break; 
    case CallStmt: return "AS`CallStmt"; break; 
    case ReturnStmt: return "AS`ReturnStmt"; break; 
    case IfStmt: return "AS`IfStmt"; break; 
    case CasesStmt: return "AS`CasesStmt"; break; 
    case CasesStmtAltn: return "AS`CasesStmtAltn"; break; 
    case ErrorStmt: return "AS`ErrorStmt"; break; 
    case DefExpr: return "AS`DefExpr"; break; 
    case LetExpr: return "AS`LetExpr"; break; 
    case LetBeSTExpr: return "AS`LetBeSTExpr"; break; 
    case IfExpr: return "AS`IfExpr"; break; 
    case CasesExpr: return "AS`CasesExpr"; break; 
    case CaseAltn: return "AS`CaseAltn"; break; 
    case PrefixExpr: return "AS`PrefixExpr"; break;
    case BinaryExpr: return "AS`BinaryExpr"; break; 
    case AllOrExistsExpr: return "AS`AllOrExistsExpr"; break; 
    case ExistsUniqueExpr: return "AS`ExistsUniqueExpr"; break; 
    case SetEnumerationExpr: return "AS`SetEnumerationExpr"; break; 
    case SetComprehensionExpr: return "AS`SetComprehensionExpr"; break; 
    case SetRangeExpr: return "AS`SetRangeExpr"; break; 
    case SeqEnumerationExpr: return "AS`SeqEnumerationExpr"; break; 
    case SeqComprehensionExpr: return "AS`SeqComprehensionExpr"; break; 
    case SubSequenceExpr: return "AS`SubSequenceExpr"; break; 
    case SeqModifyMapOverrideExpr: return "AS`SeqModifyMapOverrideExpr"; break; 
    case MapEnumerationExpr: return "AS`MapEnumerationExpr"; break; 
    case Maplet: return "AS`Maplet"; break; 
    case MapComprehensionExpr: return "AS`MapComprehensionExpr"; break; 
    case TupleConstructorExpr: return "AS`TupleConstructorExpr"; break; 
    case RecordConstructorExpr: return "AS`RecordConstructorExpr"; break; 
    case TokenConstructorExpr: return "AS`TokenConstructorExpr"; break;
    case RecordModifierExpr: return "AS`RecordModifierExpr"; break; 
    case RecordModification: return "AS`RecordModification"; break; 
    case ApplyExpr: return "AS`ApplyExpr"; break; 
    case FieldSelectExpr: return "AS`FieldSelectExpr"; break; 
    case IsExpr: return "AS`IsExpr"; break; 
    case BoolLit: return "AS`BoolLit"; break; 
    case RealLit: return "AS`RealLit"; break; 
    case CharLit: return "AS`CharLit"; break; 
    case TextLit: return "AS`TextLit"; break; 
    case QuoteLit: return "AS`QuoteLit"; break; 
    case NilLit: return "AS`NilLit"; break; 
    case Name: return "AS`Name"; break; 
    case OldName: return "AS`OldName"; break; 
    case UndefinedExpr: return "AS`UndefinedExpr"; break; 
    case MapOrSeqRef: return "AS`MapOrSeqRef"; break; 
    case FieldRef: return "AS`FieldRef"; break; 
    case PatternName: return "AS`PatternName"; break; 
    case MatchVal: return "AS`MatchVal"; break; 
    case SetEnumPattern: return "AS`SetEnumPattern"; break; 
    case SetUnionPattern: return "AS`SetUnionPattern"; break; 
    case SeqEnumPattern: return "AS`SeqEnumPattern"; break; 
    case SeqConcPattern: return "AS`SeqConcPattern"; break; 
    case RecordPattern: return "AS`RecordPattern"; break; 
    case TuplePattern: return "AS`TuplePattern"; break; 
    case SetBind: return "AS`SetBind"; break; 
    case MultSetBind: return "AS`MultSetBind"; break; 
    case SeqForLoopStmt: return "AS`SeqForLoopStmt"; break; 

      /* Semantic Values */
    case BOOL_V: return "SEM`BOOL"; break;
    case NUM_V: return "SEM`NUM"; break;
    case CHAR_V: return "SEM`CHAR"; break;
    case QUOTE_V: return "SEM`QUOTE"; break;
    case NIL_V: return "SEM`NIL"; break;
    case UNDEF_V: return "SEM`UNDEF"; break;
    case FN_V: return "SEM`FN"; break;
    case OP_V: return "SEM`OP"; break;
    case ExplFN_V: return "SEM`ExplFN"; break;
    case ImplFN_V: return "SEM`ImplFN"; break;
    case ExplOP_V: return "SEM`ExplOP"; break;
    case ImplOP_V: return "SEM`ImplOP"; break;
    case ExplPOLY_V: return "SEM`ExplPOLY"; break;
    case ImplPOLY_V: return "SEM`ImplPOLY"; break;
    case SEQ_V: return "SEM`SEQ"; break;
    case SET_V: return "SEM`SET"; break;
    case MAP_V: return "SEM`MAP"; break;
    case TUPLE_V: return "SEM`TUPLE"; break;
    case REC_V: return "SEM`REC"; break;
    case TOKEN_V: return "SEM`TOKEN"; break;
    case EXIT_V: return "SEM`EXIT"; break;
    case CONT_V: return "SEM`CONT"; break;
    case BlkEnv: return "SEM`BlkEnv"; break;
#ifdef VDMPP
    case OBJ_Ref_V : return "SEM`OBJ_Ref"; break;
    case OBJ_V : return "SEM`OBJ"; break;
    case OBJ_Desc : return "GLOBAL`OBJ_Desc";
#endif //VDMPP
    case ValTp: return "SEM`ValTp";break;

      /* Extensions to ISO/VDM-SL (full syntax) */

    case AlwaysStmt: return "AS`AlwaysStmt"; break; 
    case TrapStmt: return "AS`TrapStmt"; break; 
    case RecTrapStmt: return "AS`RecTrapStmt"; break; /* TixeStmt */
    case ExitStmt: return "AS`ExitStmt"; break; 
    case IdentStmt: return "AS`IdentStmt"; break; 
    case BlockStmt: return "AS`BlockStmt"; break; 
    case TypeBind: return "AS`TypeBind"; break; 
    case MultTypeBind: return "AS`MultTypeBind"; break; 
    case MapInverseExpr: return "AS`MapInverseExpr"; break; 
    case IotaExpr: return "AS`IotaExpr"; break; 


    case ExplFnDef: return "AS`ExplFnDef"; break; 
    case ImplFnDef: return "AS`ImplFnDef"; break; 
    case ExtExplFnDef: return "AS`ExtExplFnDef"; break;
    case ExplOpDef: return "AS`ExplOpDef"; break; 
    case ImplOpDef: return "AS`ImplOpDef"; break; 
    case ExtExplOpDef: return "AS`ExtExplOpDef"; break;
    case NameType: return "AS`NameType"; break; 
    case PatTypePair: return "AS`PatTypePair"; break; 
    case OpType: return "AS`OpType"; break; 
    case TAG_TYPE_AS_OpBody: return "AS`OpBody"; break; 
    case TAG_TYPE_AS_FnBody: return "AS`FnBody"; break; 
    case ExtInf: return "AS`ExtInf"; break; 
    case Error: return "AS`Error"; break; 
    
    case BracketedExpr: return "AS`BracketedExpr"; break ;
    case FctTypeInstExpr: return "AS`FctTypeInstExpr"; break ;
    
    case NonDetStmt: return "AS`NonDetStmt"; break; 
    case AssignDef: return "AS`AssignDef"; break ;
    case Trap: return "AS`Trap"; break ;
    case ElseifStmt: return "AS`ElseifStmt"; break ;
    case ElseifExpr: return "AS`ElseifExpr"; break ;

    case State: return "SEM`State"; break;

    case TotalFnType: return "AS`TotalFnType"; break; 
    case PartialFnType: return "AS`PartialFnType"; break; 
    case LambdaFnType: return "AS`LambdaFnType"; break;

      /* Record tags for modules */

#ifdef VDMSL
    case ParameterSig: return "AS`ParameterSig"; break;
    case ImportSig: return "AS`ImportSig"; break;
    case ExportSig: return "AS`ExportSig"; break;
    case InstantSig: return "AS`InstantSig"; break;

    case SigmaMO: return "AS`SigmaMO"; break;
    case SigmaDEFS: return "AS`SigmaDEFS"; break;
    case SigmaPMO: return "AS`SigmaPMO"; break;
#endif //VDMSL

      /* Record tags for latex generator */

    case ValueDef: return "AS`ValueDef"; break; 
    case BracketedType: return "AS`BracketedType"; break; 

      /* Record tags for static semantics */

    case TypeRepElem: return "REP`TypeRepElem"; break; 
    case TagRepElem: return "REP`TagRepElem"; break; 
    case TagRepEnv: return "REP`TagRepEnv"; break; 
/*    case ClassTypeRep: return "REP`ClassTypeRep"; break;*/
    case NameInfo: return "AS`NameInfo"; break;

      /* Record tags for error reporting */

    case SyntaxError: return "AS`SyntaxError"; break; 
    case MSCall: return "AS`MSCall"; break; 
    case ErrMsg: return "AS`ErrMsg"; break; 

      /* Record tags for Afrodite constructs */

#ifdef VDMPP
    case TAG_TYPE_AS_Class: return "AS`Class"; break;
    case InstanceInv: return "AS`InstanceInv"; break;
    case DelayStmt: return "AS`DelayStmt"; break;
    case StartStmt: return "AS`StartStmt"; break;
    case TAG_TYPE_AS_NewExpr: return "AS`NewExpr"; break;
    case SelfExpr: return "AS`SelfExpr"; break;
    case IsOfClassExpr: return "AS`IsOfClassExpr"; break;
    case IsOfBaseClassExpr: return "AS`IsOfBaseClassExpr"; break;
    case SpecificationStmt: return "AS`SpecificationStmt"; break;
    case StartListStmt: return "AS`StartListStmt"; break;
    case TAG_TYPE_AS_Permission: return "AS`Permission"; break;
    case Trace: return "AS`Trace"; break;
    case AlphabetExpr: return "AS`AlphabetExpr"; break;
    case SelectStmt: return "AS`SelectStmt"; break;
    case TraceExpr: return "AS`TraceExpr"; break;
    case CTerm: return "AS`CTerm"; break;
    case CTerm0: return "AS`CTerm0"; break;
    case CTerm1: return "AS`CTerm1"; break;
    case ProjTrace: return "AS`ProjTrace"; break;
    case PrefTrace: return "AS`PrefTrace"; break;
    case DelTrace: return "AS`DelTrace"; break;
    case WeaveTrace: return "AS`WeaveTrace"; break;
    case SyncTrace: return "AS`SyncTrace"; break;
    case QsyncTrace: return "AS`QsyncTrace"; break;
    case PerObl: return "AS`PerObl"; break;
    case GuardedCommand: return "AS`GuardedCommand"; break;
    case AnswerStmt: return "AS`AnswerStmt"; break;
    case AlphaTrace: return "AS`AlphaTrace"; break;

    case ActExpr: return "AS`ActExpr"; break;
    case FinExpr: return "AS`FinExpr"; break;
    case ActiveExpr: return "AS`ActiveExpr"; break;
    case WaitingExpr: return "AS`WaitingExpr"; break; 
    case ReqExpr: return "AS`ReqExpr"; break;

    case SameClassExpr: return "AS`SameClassExpr"; break;
    case SameBaseClassExpr: return "AS`SameBaseClassExpr"; break;

    case TimeVarDecl: return "AS`TimeVarDecl";break;
    case Assumption : return "AS`Assumption";break;
    case Effect : return "AS`Effect";break;

#endif //VDMPP

      /* Record tags Afrodite TypeRep's */


    case InvTypeRep: return "REP`InvTypeRep"; break;            
    case BasicTypeRep: return "REP`BasicTypeRep"; break;        
    case QuoteTypeRep: return "REP`QuoteTypeRep"; break;        
    case CompositeTypeRep: return "REP`CompositeTypeRep"; break;
    case FieldRep: return "REP`FieldRep"; break;                
    case UnionTypeRep: return "REP`UnionTypeRep"; break;        
    case ProductTypeRep: return "REP`ProductTypeRep"; break;    
    case SetTypeRep: return "REP`SetTypeRep"; break;            
    case SeqTypeRep: return "REP`SeqTypeRep"; break;            
    case GeneralMapTypeRep: return "REP`GeneralMapTypeRep"; break;
    case InjectiveMapTypeRep: return "REP`InjectiveMapTypeRep"; break;
    case TypeNameRep: return "REP`TypeNameRep"; break;           
    case PartialFnTypeRep: return "REP`PartialFnTypeRep"; break; 
    case TotalFnTypeRep: return "REP`TotalFnTypeRep"; break;     
#ifdef VDMPP
    case ObjRefTypeRep: return "REP`ObjRefTypeRep"; break;      
#endif //VDMPP
    case AllTypeRep: return "REP`AllTypeRep"; break;            
    case UnitTypeRep: return "REP`UnitTypeRep"; break;          
    case NilTypeRep: return "REP`NilTypeRep"; break;            
    case EmptySetTypeRep: return "REP`EmptySetTypeRep"; break;  
    case EmptySeqTypeRep: return "REP`EmptySeqTypeRep"; break;  
    case EmptyMapTypeRep: return "REP`EmptyMapTypeRep"; break;  

    case OpTypeRep: return "REP`OpTypeRep"; break; 
    case TypeVarRep: return "REP`TypeVarRep"; break; 
    case TypeParRep: return "REP`TypeParRep"; break; 

    case ModuleRep: return "REP`ModuleRep"; break; 
    case ParSigRep: return "REP`ParSigRep"; break; 
    case ExpSigRep: return "REP`ExpSigRep"; break; 
    case PolyTypeRep: return "REP`PolyTypeRep"; break; 
    case PolyTypeRepElem: return "REP`PolyTypeRepElem"; break; 

#if 0
    case Sequent: return "PM`Sequent"; break; 
    case PrfRule: return "PM`PrfRule"; break; 
    case TypeJudgement: return "PM`TypeJudgement"; break; 
    case TypeAbbrevExpr: return "PM`TypeAbbrevExpr"; break; 
    case AbbrevExpr: return "PM`AbbrevExpr"; break; 
    case Subtype: return "PM`Subtype"; break; 
    case More: return "PM`More"; break; 
    case Comment: return "PM`Comment"; break; 
    case TupleSelectExpr: return "PM`TupleSelectExpr"; break; 
    case TheoremDef: return "PM`TheoremDef"; 
#endif //0

    default: 
      printf("(%s)->", yytext);
      return "UNKNOWN";
    }
}


