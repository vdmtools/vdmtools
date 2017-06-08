// -*- mode: c++; -*-
/*
 *  WHAT
 *     Module: Byacc parser recovery.
 *
 *  IDENTIFICATION
 *     $Id: recover.h,v 1.25 2006/06/14 06:15:53 vdmtools Exp $
 *  AUTHOR
 *     ETN
 *  COPYRIGHT
 *     (C) Kyushu University
 */

#ifndef __recover_h__
#define __recover_h__

#include "metaiv.h"
#include "scanner.h"
#include "Errors.h"
#include <exception>
#include <vector>
#include <map>
#include <limits.h>

class TokenTableEntry {
public:
  wstring stringRepr;   // textual token value
  int validRepair;      // Can token be used as a repair token?
  long lexTokenGroups;  // TokenGroup flag.
  long flag;            // Sum of values of enum TokenTableFlags

  TokenTableEntry(const wchar_t * str, int vr, long ltg, long f)
    : stringRepr(str), validRepair(vr), lexTokenGroups(ltg), flag(f) {}

  enum TokenTableValidFlags {
    IS_VALID = 1,  // = (1<<0);
    TMP_VALID = 2 // = (1<<1);
  };
};

class TokenGroup {
public:
  long tGroup;
  wstring groupName;
  int override;
  int count;
  int actCount;

  TokenGroup(long tg, const wchar_t * gn, long ov)
    : tGroup(tg), groupName(gn), override(ov), count(0), actCount(0) {}
};

enum TokenTableFlags {
  TTF_None = 0,
  TTF_NoQuote = 1
};

class RecoverToken {
public:
  int token;
  wstring yytext;
  int line, column;
public:
  RecoverToken(const RecoverToken & t)
   : token(t.token), yytext(t.yytext), line(t.line), column(t.column) {}; 
  RecoverToken(int token, const wstring & yytext, int line, int column)
   : token(token), yytext(yytext), line(line), column(column) {}; 
};

typedef vector<int> NumTokenArray;

class recovery_data {
public:
  recovery_data() : BadLength(0), BestRepairDist(SHRT_MAX), Best_count(0), Best_offset(0) {}

  void init();

  NumTokenArray Best_Repair_Array;
  int BadLength;                // Length of bad subwstring of Input
  int BestRepairDist;
  int Best_count;
  int Best_offset;
};

class ParserRecover {
protected:
  typedef vector<RecoverToken> TokenSeq;
  typedef map<int, TokenTableEntry> TokenTableType;

public:
  typedef short * shortp;
  typedef const short * cshortp;
  typedef const unsigned short * cushortp;
  typedef const unsigned char * cucharp;

#ifdef USE_BISON128
  typedef cshortp CUSHORTP;
  typedef cshortp CUCHARP;
#else
  typedef cushortp CUSHORTP;
  typedef cucharp CUCHARP;
#endif // USE_BISON2

private:
  enum ParsStat {PARS_OK, PARS_FINAL, PARS_ERROR};

  recovery_data Best;

  TokenSeq ExpectedTokens;
  TokenSeq Input_Sequence;

  NumTokenArray Continuation_Array;
  NumTokenArray Input_Num_Tokens;

  wstring EditOps;              // After call to EvaluateContinuation
                                // with true 3rd parameter this
                                // contains edit operations.
  int Count_Inserts, Count_Deletes, Count_Replaces, Count_Kept;

  void EnableTokens(NumTokenArray::iterator, int length);

  struct EXC_YACCSTACK_OVERFLOW : public exception {};
  void parse_stack_check(shortp base, register shortp stack) {
    if (stack >= base+YYSTACKSIZE - 5) {
      throw EXC_YACCSTACK_OVERFLOW();
    }
  }

  bool ShouldQuote(int token);
  void RestoreInput(TokenSeq & RepairStr, TokenSeq & Input);
  short * CopyStack(const short * base, const short * stack, short * stack_copy);
  void OutputBetterRepair(int len, int offset, int min, int count, int better);
  void EvaluateContinuation(cshortp base, cshortp sstp, int offset);
  wstring TextifyTokenSeq(TokenSeq::const_iterator b, TokenSeq::const_iterator e, bool quote_each, int maxelems=-1);
  int MinDist(NumTokenArray::const_iterator, int inp_len, NumTokenArray::const_iterator, int cont_len, bool getedit);
  wstring StripSeq(const TokenSeq & Seq);
  int InSeq(const RecoverToken & tok, const TokenSeq & Cont);
  int Cost(int, int);
  void GenerateRepairs(const short *sstp_base, const short *sstp, int cont_len, int depth, int offset);
  int Reparse(short *base, short * &sstp, int start, int end);
  int recover(bool preparse, shortp yyss, shortp yyssp, int bad_token, int yystacksize);

  RecoverToken lex();

protected:
  wstring TextifyToken(const RecoverToken& r, bool quote);
  int ParseOneChar(short *sstp_base, short * &sstp /* is modified */ , int yychar);

protected:
  // map of token to pointers to TokenTableEntry
  TokenTableType repair_tokens;

  int YYSTACKSIZE;
  int YYLAST; //YYTABLESIZE;
  int YYFINAL;
  short YYFLAG;
  int YYNTBASE;
  int yyINITDEPTH;
  int (*YYTRANSLATE)(int);

  CUSHORTP yyr1;        // yylhs
  CUCHARP yyr2;         // yyyylen
  CUSHORTP yydefact;    // yydefred

  cshortp yydefgoto;    // yydgoto
  cshortp yypact;       // yysindex
  cshortp yypgoto;      // yygindex
  cshortp yytable;      // yytable
  cshortp yycheck;      // yycheck

public:
  ParserRecover() {InitRepairTokens();};
  ParserRecover(int yylast, //tablesize,
                int final,  //final
                int yyflag,
                int yyntbase,
                int (*yytranslate)(int),
                CUSHORTP yyr1, // lhs,
                CUCHARP yyr2, // len,
                CUSHORTP yydefact, //defred,
                cshortp yydefgoto, //dgoto,
                cshortp yypact, //sindex,
                cshortp yypgoto, //gindex,
                cshortp yytable, //table,
                cshortp yycheck, //check,
                int yyinitdepth,
                int yymaxutok);
  virtual ~ParserRecover() { this->repair_tokens.clear(); }

  virtual void Report(int, int, const type_cLL & msgs); // Interface.
  virtual wstring GroupExpected(const TokenSeq &, TokenTableType &);
  void InitRepairTokens();
  void AddToken(int token, const wchar_t * str, int vr, long grp, long flag);
  virtual int Lex() = 0;
  virtual int GetLexLine() = 0;
  virtual int GetLexColumn() = 0;
  virtual wstring GetLexText(int token) = 0;

  virtual wstring GetString(int token);
  void RecoverStart(shortp yyss, shortp yyssp, int token, int yystacksize);
  int PreParse();
  virtual SEQ<Char> GetErrMessage(int num);

protected:
  static bool less_token(const RecoverToken & t1, const RecoverToken & t2);
  static bool eq_token(const RecoverToken & t1, const RecoverToken & t2);

private:
  long debugflag;
};

class VDM_Recover : public ParserRecover {
private:
  typedef vector<TokenGroup> TokenGroupArray;
public:
  VDM_Recover() {};
  VDM_Recover(int yylast,   //tablesize
              int yyfinal,  //final
              int yyflag,
              int yyntbase,
              int (*yytranslate)(int),
              CUSHORTP yyr1, // lhs,
              CUCHARP yyr2, // len,            
              CUSHORTP yydefact, //defred,
              cshortp yydefgoto, //dgoto, 
              cshortp yypact, //sindex,  
              cshortp yypgoto, //gindex,
              cshortp yytable, //table, 
              cshortp yycheck, //check,
              int yyinitdepth,
              int yymaxutok)
    : ParserRecover(yylast,
                    yyfinal,
                    yyflag,
                    yyntbase,
                    yytranslate,
                    yyr1,
                    yyr2,
                    yydefact,
                    yydefgoto,
                    yypact,
                    yypgoto,
                    yytable,
                    yycheck,
                    yyinitdepth,
                    yymaxutok) { InitTokenGroupArray(); };
  virtual ~VDM_Recover() { this->tokenGroupArray.clear(); }

public:
  void SetFirstPos(const YYLTYPE& f_pos) { this->pos = f_pos; }
  virtual wstring GetString(int token);

  void SetFileId(const TYPE_CI_FileId & fid) { this->FileId = fid; }
  void SetErrs(vdm_Errors * errs) { this->Errs = errs; }
  void SetEofText(const wstring & text) { this->EofText = text; }

private:
  TYPE_CI_FileId FileId;                   // What a load of hack!
  wstring EofText;
  vdm_Errors * Errs;
  TokenGroupArray tokenGroupArray;

  virtual void Report(int, int, const type_cLL & msgs);
  virtual wstring GroupExpected(const TokenSeq &, TokenTableType & RepToks);
  void InitTokenGroupArray();
  void AddTokenGroup(long grp, const wchar_t * name, long over);

  YYLTYPE pos;

  virtual int Lex();
  virtual int GetLexLine() { return this->pos.first_line; }
  virtual int GetLexColumn() { return this->pos.first_column; }
  virtual wstring GetLexText(int token);
};
//

#endif // __recover_h__
