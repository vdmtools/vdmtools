/*
 *  WHAT
 *     Module: Bison parser recovery.
 *  IDENTIFICATION
 *     $Id: recover.cc,v 1.53 2006/05/17 08:18:31 vdmtools Exp $
 *  AUTHOR
 *     ETN
 *  COPYRIGHT
 *     (C) Kyushu University
 */

#include "metaiv.h"
#include "recover.h"
#include "tb_wstring.h"
#include "tbutils.h"
#include "settings.h"
#include "UTIL.h"

#include <stdlib.h>             // getenv()
#include <algorithm>            // unique(), sort()
#include <limits.h>

#define SYNTAXERR_NO_REPAIR         1
#define SYNTAXERR_EXPECTED_BEFORE   2
#define SYNTAXERR_ASSUMED_BEFORE    3
#define SYNTAXERR_IGNORED           4
#define SYNTAXERR_ASSUMED_INSTED_OF 5
#define SYNTAXERR_STOPPING          6

static unsigned int CONT_LEN = 2;        /* Default continuation length */

// FIXME2:
//const int YYEOF = 0;
//const int YYEMPTY = -2;
#define YYEMPTY         (-2)
#define YYEOF           0

//long debugflag = 0;

#define DEBUG_TRY_TOKENS           1  // 1 << 0
#define DEBUG_GOOD_CONT            2  // 1 << 1
#define DEBUG_ALL_CONT             4  // 1 << 2
#define DEBUG_CONFLICT_CONT        8  // 1 << 3
#define DEBUG_CHECK_TOKENLIST     16  // 1 << 4
#define DEBUG_EXPECT_NO_GROUPING  32  // 1 << 5
#define DEBUG_PREPARSE            64  // 1 << 6
#define DEBUG_PARSE_ONE          128  // 1 << 7
#define DEBUG_CHECK_COUNT        256  // 1 << 8
#define DEBUG_ALL_ERRORS         512  // 1 << 9
 
void recovery_data::init()
{
  this->Best_Repair_Array.erase(Best_Repair_Array.begin(), Best_Repair_Array.end());
  this->BadLength = 0;
  this->BestRepairDist = SHRT_MAX;
  this->Best_count = 0;
  this->Best_offset = 0;
}

ParserRecover::ParserRecover(int yylast,      //tablesize,
                             int yyfinal,     //final
                             int yyflag,
                             int yyntbase,
                             int (*yytranslate)(int),
                             CUSHORTP r1,     // lhs,
                             CUCHARP r2,      // len,
                             CUSHORTP defact, //defred,
                             cshortp defgoto, //dgoto,
                             cshortp pact,    //sindex,
                             cshortp pgoto,   //gindex,
                             cshortp table,   //table,
                             cshortp check,   //check,
                             int yyinitdepth,
                             int yymaxutok)
{
  this->YYLAST = yylast;
  this->YYFINAL = yyfinal;
  this->YYFLAG = yyflag;
  this->YYNTBASE = yyntbase;
  this->YYTRANSLATE = yytranslate;
  this->yyr1 = r1;
  this->yyr2 = r2;
  this->yydefact = defact;
  this->yydefgoto = defgoto;
  this->yypact = pact;
  this->yypgoto = pgoto;
  this->yytable = table;
  this->yycheck = check;

  yyINITDEPTH = yyinitdepth;

  InitRepairTokens();

  this->debugflag = 0;

  const char * E;
  if ((E = getenv("RECOVERDEBUG"))) {
    this->debugflag = atol(E);
    wcerr << L"Recoverdebugflag = " << this->debugflag << endl;
  }

  if (this->debugflag & DEBUG_CHECK_TOKENLIST)
  {
    wofstream log("recover.log", ios::out|ios::ate);
    log << L"beginning of table" << endl;
    log << L"TABSIZE=" << this->repair_tokens.size() << " YYMAXUTOK=" << yymaxutok << endl;
    for (size_t i = 0; i <= (size_t)yymaxutok; i++)
    {
      TokenTableType::iterator mitr (this->repair_tokens.find(i));
      if (mitr == this->repair_tokens.end())
      {
        log << L"Potential missing token number = " << i << endl;
      }
    }
  }
}

/** --------------------------------------------------**/
/** The following functions are functions used by the **/
/** error recovery algorithm **/

// Default GetString routine.

// GetString
wstring ParserRecover::GetString (int candidate)
{
  TokenTableType::iterator mitr (this->repair_tokens.find(candidate));
  if (mitr != this->repair_tokens.end())
  {
    wstring res (mitr->second.stringRepr);
    if (!res.empty())
      return res;
  }
  return L"<?" + Int(candidate).ascii() + L">";
}

// lex
RecoverToken ParserRecover::lex()
{
  int token = Lex();
  //cerr << L"read token=" << token << endl;
  return RecoverToken(token, GetLexText(token), GetLexLine(), GetLexColumn());
}

// less_token
bool ParserRecover::less_token(const RecoverToken & t1, const RecoverToken & t2)
{
  return t1.token < t2.token;
}

// eq_token
bool ParserRecover::eq_token(const RecoverToken & t1, const RecoverToken & t2)
{
  return t1.token == t2.token;
}

// StripSeq
wstring ParserRecover::StripSeq(const TokenSeq & Seq)
{
  TokenSeq seq2 (Seq);
  sort(seq2.begin(), seq2.end(), ParserRecover::less_token);
  TokenSeq::iterator last = unique(seq2.begin(), seq2.end(), ParserRecover::eq_token);
  wstring seqstr = L"";
  for (TokenSeq::const_iterator i = seq2.begin(); i != last; ++i ) {
    RecoverToken token (*i);
    if (seqstr.length() > 0)
      seqstr = seqstr + L", ";
    seqstr += GetString(token.token);
  }
  return seqstr;
}

// Min
static int Min(int n1, int n2, int n3)
{
  return (n1 <= n2) ? (n1 <= n3 ? n1 : n3) : (n2 <= n3 ? n2 : n3);
}

#define COST_DELETE              2
#define COST_INSERT              1
#define COST_REPLACE             3
#define COST_INSERT_EOF        100
#define COST_DELETE_EOF        100
#define COST_REPLACE_TO_EOF    100
#define COST_REPLACE_FROM_EOF  100

// Cost
// +-----------+-----------------------+-----------------------+-----------------------+
// | tok1\tok2 |         -1            |          0            |       y(> 0)          |
// +-----------+-----------------------+-----------------------+-----------------------+
// |    -1     | (COST_DELETE)         | COST_INSERT_EOF       | COST_INSERT           |
// +-----------+-----------------------+-----------------------+-----------------------+
// |     0     | COST_DELETE_EOF       | 0                     | COST_REPLACE_FROM_EOF |
// +-----------+-----------------------+-----------------------+-----------------------+
// | x(> 0)    | COST_DELETE           | COST_REPLACE_TO_EOF   | 0 or COST_REPLACE     |
// +-----------+-----------------------+-----------------------+-----------------------+
int ParserRecover::Cost(int tok1, int tok2)
{
  if (tok2 == -1) {
    // Delete tok1
//    if (tok1 == 0)                // Delete EOF?
//      return COST_DELETE_EOF;   // Expensive to delete EOF
//    else
//      return COST_DELETE;
    return ((tok1 == 0) ? COST_DELETE_EOF : COST_DELETE);
  }
  else if (tok1 == -1) {
    // Insert tok2
//    if (tok2 == 0)                // Insert EOF?
//      return COST_INSERT_EOF;   // Expensive to insert EOF
//    else
//      return COST_INSERT;
    return ((tok2 == 0) ? COST_INSERT_EOF : COST_INSERT);
  }
  else if ((tok1 == 0) || (tok2 == 0)) {
//    if (tok1 == 0)
//      return tok2 == 0 ? 0 : COST_REPLACE_FROM_EOF; // Expensive delete of EOF
//    else
//      return COST_REPLACE_TO_EOF; // Replace tok1 != 0, with tok2==EOF
    return ((tok1 == 0) ? ((tok2 == 0) ? 0 : COST_REPLACE_FROM_EOF) : COST_REPLACE_TO_EOF);
  }
//  else if ( tok1 == tok2 )
//    return 0;
//  else
//    return COST_REPLACE;        // ( tok1 != tok2 )
  else
  {
    return ((tok1 == tok2) ? 0 : COST_REPLACE);
  }
}

int ParserRecover::MinDist(NumTokenArray::const_iterator Inp,
                           int Inp_Len,
                           NumTokenArray::const_iterator Cont,
                           int Cont_Len,
                           bool getedit)
{
  int dist[100][100]; //[Inp_Len+1][Cont_Len+1];
  int inp[100];       // [Inp_Len+1];
  int cont[100];      // [Cont_Len+1];

  for (int i = 0; i < Inp_Len; i++)
    inp[i+1] = Inp[i];

  for (int j = 0; j < Cont_Len; j++)
    cont[j+1] = Cont[j];

  dist[0][0] = 0;
  for (int k = 1 ; k <= Inp_Len ; k++ )
    dist[k][0] = dist[k-1][0] + Cost(inp[k], -1); /* delete input */

  for (int l = 1 ; l <= Cont_Len ; l++)
    dist[0][l] = dist[0][l-1] + Cost(-1, cont[l]); /* insert in input */

  for (int m = 1 ; m <= Inp_Len ; m++) {
    for (int n = 1; n <= Cont_Len ; n++) {
      int cost = Cost(inp[m], cont[n]);
      //if ( cost == 0 )
      //  Cont.ImpModify(j,Inp[i]);
      int min1 = dist[m-1][n-1] + cost; /* replace */
      int min2 = dist[m-1][n] + Cost(inp[m], -1);   /* delete */
      int min3 = dist[m][n-1] + Cost(-1, cont[n]);   /* insert */
      dist[m][n] = Min(min1, min2, min3);
    }
  }

  if (getedit) {
    this->EditOps = L"";
    this->Count_Inserts = 0;
    this->Count_Deletes = 0;
    this->Count_Replaces = 0;
    this->Count_Kept = 0;
    int i = Inp_Len;
    int j = Cont_Len;
    while ((i > 0) && (j > 0)) {
      if (dist[i][j] == (dist[i-1][j] + Cost(inp[i], -1))) {
        i--;
        EditOps.insert(0, L"d");
        this->Count_Deletes ++;
      } else if (dist[i][j] == (dist[i][j-1] + Cost(-1, cont[j]))) {
        j--;
        this->EditOps.insert(0, L"i");
        this->Count_Inserts ++;
      } else {
        i--;
        j--;
        if (Cost(inp[i], cont[j]) == 0) {
          this->EditOps.insert(0, L"k");
          this->Count_Kept++;
        } else {
          this->EditOps.insert(0, L"r");
          this->Count_Replaces++;
        }
      }
    }
    for (; i > 0; i--) {
      this->EditOps.insert(0, L"d");
      this->Count_Deletes ++;
    }
    for (; j > 0; j--) {
      this->EditOps.insert(0, L"i");
      this->Count_Inserts ++;
    }
  }

  return dist[Inp_Len][Cont_Len];
}

// Default does not do any grouping.
// virtual method
wstring ParserRecover::GroupExpected(const TokenSeq & seq, TokenTableType & /*RepToks*/ )
{
  return TextifyTokenSeq(seq.begin(), seq.end(), true);
}

bool ParserRecover::ShouldQuote(int token)
{
  TokenTableType::iterator mitr (this->repair_tokens.find(token));
  if (mitr != this->repair_tokens.end())
  {
    return (mitr->second.flag & TTF_NoQuote);
  }
  return false;
}

wstring ParserRecover::TextifyToken(const RecoverToken & tup, bool quote)
{
  int token = tup.token;
  wstring txt (tup.yytext);
  if (txt == L"") {
    txt = GetString(token);
  }
  if (quote && ShouldQuote(token)) {
    txt.insert(0, L"`");
    txt.append(L"'");
  }
  return txt;
}

wstring ParserRecover::TextifyTokenSeq(TokenSeq::const_iterator b, TokenSeq::const_iterator e,
                                       bool quote_each, int maxelems)
{
  wstring Res;
  for (int i = 0; (b != e) && (maxelems < 0 || i < maxelems); i++, ++b)
  {
    wstring txt (TextifyToken(*b, quote_each));
    if (Res.length() > 0)
      Res += L" ";
    Res += txt;
  }
  return Res;
}

int ParserRecover::PreParse()
{
  int errors = 0;
  try {
    errors = recover(true, NULL, NULL, 0, 0);
  }
  catch (EXC_YACCSTACK_OVERFLOW & e) {
    Sequence msgs (mk_sequence(SEQ<Char>(L"Parse stack limit reached. No more repairs can be found. ")));
    Report(-1, -1, msgs);
    errors++;
  }
  return errors;
}

SEQ<Char> ParserRecover::GetErrMessage(int num)
{
  if (TBUTILS::isJapanese() && Settings.UseTranslation())
  {
    string msg;
    switch(num) {
      case SYNTAXERR_NO_REPAIR: {
        msg = "\\u3053\\u3053\\u3067\\u306F\\u4FEE\\u5FA9\\u65B9\\u6CD5\\u304C\\u898B\\u3064\\u304B\\u308A\\u307E\\u305B\\u3093\\u3002" ;
        break;
      }
      case SYNTAXERR_EXPECTED_BEFORE: {
        msg = "`%2'\\u306E\\u524D\\u306B`%1'\\u304C\\u3042\\u308B\\u306F\\u305A\\u3067\\u3059\\u3002";
        break;
      }
      case SYNTAXERR_ASSUMED_BEFORE: {
        msg = "`%2'\\u306E\\u524D\\u306B`%1'\\u304C\\u3042\\u308B\\u3068\\u4EEE\\u5B9A\\u3057\\u307E\\u3059\\u3002";
        break;
      }
      case SYNTAXERR_IGNORED: {
        msg = "`%1'\\u3092\\u7121\\u8996\\u3057\\u307E\\u3059\\u3002";
        break;
      }
      case SYNTAXERR_ASSUMED_INSTED_OF: {
        msg = "`%2'\\u306E\\u4EE3\\u308F\\u308A\\u306B\\u3001`%1'\\u3092\\u4EEE\\u5B9A\\u3057\\u307E\\u3059\\u3002";
        break;
      }
      case SYNTAXERR_STOPPING: {
        msg = "%1\\u69CB\\u6587\\u30A8\\u30E9\\u30FC\\u306E\\u5F8C\\u3067\\u505C\\u6B62\\u3057\\u307E\\u3059\\u3002";
        break;
      }
    }
    return SEQ<Char>(TBWSTR::string2wstring(msg));
  }
  else
  {
    switch(num) {
      case SYNTAXERR_NO_REPAIR:
        return SEQ<Char>(L"No repair was found at this point.");
      case SYNTAXERR_EXPECTED_BEFORE:
        return SEQ<Char>(L"Expected: `%1' before `%2'");
      case SYNTAXERR_ASSUMED_BEFORE:
        return SEQ<Char>(L"Assumed: `%1' before `%2'");
      case SYNTAXERR_IGNORED:
        return SEQ<Char>(L"Ignored: `%1'");
      case SYNTAXERR_ASSUMED_INSTED_OF:
        return SEQ<Char>(L"Assumed: `%1' instead of `%2'");
      case SYNTAXERR_STOPPING:
        return SEQ<Char>(L"Stopping after %1 syntax errors.");
      default:
        return SEQ<Char>(L"");
    }
  }
}

void ParserRecover::RecoverStart(shortp yyss, shortp yyssp, int bad_token, int yystacksize)
{
  try {
    recover(false, yyss, yyssp, bad_token, yystacksize);
  }
  catch (EXC_YACCSTACK_OVERFLOW & e) {
    Sequence msgs (mk_sequence(SEQ<Char>(L"Parse stack limit reached. No more repairs can be found. ")));
    Report(-1, -1, msgs);
    return;
  }
}

int ParserRecover::recover(bool preparse, shortp yyss, shortp yyssp, int bad_token, int yystacksize)
{
  char *E;
  if ((E = getenv("CONTLEN"))) {
    CONT_LEN = atol(E);
    wcerr << L"Contlen = " << CONT_LEN << endl;
  }
  if ((E = getenv("RECOVERDEBUG"))) {
    this->debugflag = atol(E);
    wcerr << L"Recoverdebugflag = " << this->debugflag << endl;
  }
  wstring Error;

  int safedist = 20;
  if ((E = getenv("VDMDESAFE"))) {
    safedist = atol(E);
    wcerr << L"Safedist = " << safedist << endl;
  }

  if (preparse) {
    this->YYSTACKSIZE = yyINITDEPTH;
  } else {
    this->YYSTACKSIZE = yystacksize;
  }
  short *stack0 = new short[this->YYSTACKSIZE];
  short *stack1 = new short[this->YYSTACKSIZE];
  short *stack2 = new short[this->YYSTACKSIZE];
  short *bases[3] = {stack0, stack1, stack2};
  short *sstps[3] = {stack0, stack1, stack2};

  this->Input_Num_Tokens.erase(this->Input_Num_Tokens.begin(), this->Input_Num_Tokens.end());
  this->Input_Sequence.erase(this->Input_Sequence.begin(), this->Input_Sequence.end());

  if (preparse) {
    *stack0 = *stack1 = *stack2 = 0;
  }
  else {
    // 2 == cur-1 mod 3
    sstps[0] = CopyStack(yyss, yyssp, bases[0]);
    sstps[1] = CopyStack(yyss, yyssp, bases[1]);
    sstps[2] = CopyStack(yyss, yyssp, bases[2]);
    // Push the bad token on the input vector.
    RecoverToken tokrec (bad_token, GetLexText(bad_token), GetLexLine(), GetLexColumn());
    this->Input_Sequence.push_back(tokrec);
    this->Input_Num_Tokens.push_back(tokrec.token);

    if (this->debugflag & DEBUG_PREPARSE) {
      wcerr << L"stacksize= "<< yyssp-yyss << endl << endl;
    }
  }

  static const int max_best_count = 20;
  unsigned int max_input = 2 * safedist + max_best_count;

  int ParseErrors = 0;
  int MaxErrorCount = 100;
  int input_offset = 0;
  unsigned int cur = 0;
  for (;;) {
    // Fill up buffer.
    for (; this->Input_Sequence.size() < max_input &&
           !(this->Input_Sequence.size() > 0 &&
           this->Input_Sequence.back().token == YYEOF);)
    {
//      RecoverToken tokrec = lex();
      RecoverToken tokrec (lex());
      this->Input_Sequence.push_back(tokrec);
      this->Input_Num_Tokens.push_back(tokrec.token);
      if (tokrec.token == 0) {
        break;
      }
    }

    int parse_res = Reparse(bases[cur], sstps[cur], input_offset, input_offset + safedist);

    if (this->debugflag & DEBUG_PREPARSE) {
      wcerr << L"cur=" << cur
            << L" offset=" << input_offset
            << L" size=" << (sstps[cur] - bases[cur])
            << L" state=" << *(sstps[cur]);
      wstring inp = TextifyTokenSeq(this->Input_Sequence.begin(), this->Input_Sequence.end(), false);
      wcerr << L" input: " << inp.c_str();
    }

    if (parse_res == input_offset + safedist)
    {
      // Advance to next stage.
      if (input_offset != 0)
      {
        // Remove block 0 from input buffer.
        this->Input_Sequence.erase(this->Input_Sequence.begin(), this->Input_Sequence.begin() + safedist);
        this->Input_Num_Tokens.erase(this->Input_Num_Tokens.begin(), this->Input_Num_Tokens.begin() + safedist);
        // we remove safedist tokens from input, so adjust input_offset
      }

      input_offset = safedist;
      unsigned int next = (cur+1) % 3;
      sstps[next] = CopyStack(bases[cur], sstps[cur], bases[next]);
      cur = next;
      if (this->debugflag & DEBUG_PREPARSE) {
        wcerr << L"ADVANCE: cur=" << cur
              << L" offset=" << input_offset
              << L" state=" << *(sstps[cur]) << endl << flush;
      }
    }
    else if (parse_res < 0) {
      // EOF was reached, so we are done
      break;
    }
    else {
      // Parse error
      ParseErrors++;

      // parse_res contains number of tokens successfully accepted.
      // Begin repairing.
      // Restore to previous state

      int old = (cur-2+3) % 3;
      int next = (cur-1+3) % 3;
      sstps[cur] = CopyStack(bases[old], sstps[old], bases[cur]);
      if (this->debugflag & DEBUG_PREPARSE) {
        wcerr << L"Error in token " << parse_res
              << L" cur=" << cur
              << L" old=" << old
              << L" oldstate=" << *sstps[old] << endl << flush;
      }

      // Reset recover data.
      this->Best.init();

      EnableTokens(this->Input_Num_Tokens.begin(), this->Input_Num_Tokens.size());
      for (int off = 0; off <= parse_res; off++) {
        // Find continuations starting at offset off.
        GenerateRepairs(bases[cur], sstps[cur], CONT_LEN, 0, off);
        // Advance to next state. This is safe until we reach off==parse_res
        int ret = Reparse(bases[cur], sstps[cur], off, off+1);
        if (this->debugflag & DEBUG_PREPARSE) {
          wcerr << L"Reparse off=" << off << L" ret=" << ret << endl;
        }
      }

      // Now we have the best continuation.
      RecoverToken t (this->Input_Sequence[this->Best.Best_offset]);
      int LineNo = t.line;
      int ColumnNo = t.column;

      TokenSeq best_repair_seq;

      for (size_t i = 0; i < this->Best.Best_Repair_Array.size(); i++) {
        best_repair_seq.push_back(RecoverToken(this->Best.Best_Repair_Array[i], wstring(L""),0,0));
      }

      TokenSeq::const_iterator itr = this->Input_Sequence.begin() + this->Best.Best_offset;
      wstring ignored (TextifyTokenSeq(itr, itr + this->Best.BadLength, false));
      wstring replaced (TextifyTokenSeq(best_repair_seq.begin(), best_repair_seq.end(), false));
      wstring rest (TextifyTokenSeq(itr + this->Best.BadLength, this->Input_Sequence.end(), false, 4));
      // Find edit operations.
      (void) MinDist(this->Input_Num_Tokens.begin() + this->Best.Best_offset,
                     this->Best.BadLength,
                     this->Best.Best_Repair_Array.begin(),
                     this->Best.Best_Repair_Array.size(),
                     true);

      // Replace the bad tokens with the repair tokens.
      this->Input_Sequence.erase(this->Input_Sequence.begin() + this->Best.Best_offset,
                                 this->Input_Sequence.begin() + this->Best.Best_offset + this->Best.BadLength);
      this->Input_Sequence.insert(this->Input_Sequence.begin() + this->Best.Best_offset,
                                  best_repair_seq.begin(),
                                  best_repair_seq.end());
      this->Input_Num_Tokens.erase(this->Input_Num_Tokens.begin() + this->Best.Best_offset,
                                   this->Input_Num_Tokens.begin() + this->Best.Best_offset + this->Best.BadLength);
      this->Input_Num_Tokens.insert(this->Input_Num_Tokens.begin() + this->Best.Best_offset,
                                    this->Best.Best_Repair_Array.begin(),
                                    this->Best.Best_Repair_Array.end());

      int end_of_repair = this->Best.Best_offset + best_repair_seq.size();
      sstps[cur] = CopyStack(bases[old], sstps[old], bases[cur]);
      (void) Reparse(bases[cur], sstps[cur], 0, end_of_repair);

      this->Input_Sequence.erase(this->Input_Sequence.begin(), this->Input_Sequence.begin()+end_of_repair);
      this->Input_Num_Tokens.erase(this->Input_Num_Tokens.begin(), this->Input_Num_Tokens.begin()+end_of_repair);

      input_offset = 0;
      sstps[old]  = CopyStack(bases[cur], sstps[cur], bases[old]);
      sstps[next] = CopyStack(bases[cur], sstps[cur], bases[next]);

      Sequence msgs;

      // Give up, if we can't parse further with the continuation
      if (this->Best.Best_count <= parse_res) {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_NO_REPAIR));
        msgs.ImpAppend(SEQ<Char>(msg.GetString()));
        Report(LineNo, ColumnNo, msgs);
        if (this->debugflag > 0) {
          ParserRecover::Report(LineNo, ColumnNo, msgs);
        }
        break;
      }

      // vdm_log << LineNo << L"," << ColumnNo << ":
      if (this->debugflag & DEBUG_EXPECT_NO_GROUPING)
      {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_EXPECTED_BEFORE));
        SEQ< SEQ<Char> > txts;
        txts.ImpAppend(SEQ<Char>(StripSeq(this->ExpectedTokens)));
        txts.ImpAppend(SEQ<Char>(TBWSTR::hexquadwstring2wstring(rest)));
        msgs.ImpAppend(SEQ<Char>(UTIL::ReplacePercent(msg, txts).GetString()));
      } else {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_EXPECTED_BEFORE));
        SEQ< SEQ<Char> > txts;
        txts.ImpAppend(SEQ<Char>(GroupExpected(this->ExpectedTokens, this->repair_tokens)));
        txts.ImpAppend(SEQ<Char>(TBWSTR::hexquadwstring2wstring(rest)));
        msgs.ImpAppend(SEQ<Char>(UTIL::ReplacePercent(msg, txts).GetString()));
      }

      if ((this->Count_Kept == 0) && (this->Count_Replaces == 0) &&
          (this->Count_Inserts > 0) && (this->Count_Deletes == 0))
      {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_ASSUMED_BEFORE));
        SEQ< SEQ<Char> > txts;
        txts.ImpAppend(SEQ<Char>(replaced));
        txts.ImpAppend(SEQ<Char>(TBWSTR::hexquadwstring2wstring(rest)));
        msgs.ImpAppend(SEQ<Char>(UTIL::ReplacePercent(msg, txts).GetString()));
      }
      else if ((this->Count_Kept == 0) && (this->Count_Replaces == 0) &&
               (this->Count_Inserts == 0) && (this->Count_Deletes > 0))
      {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_IGNORED));
        SEQ< SEQ<Char> > txts;
        txts.ImpAppend(SEQ<Char>(TBWSTR::hexquadwstring2wstring(ignored)));
        msgs.ImpAppend(SEQ<Char>(UTIL::ReplacePercent(msg, txts).GetString()));
      }
      else {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_ASSUMED_INSTED_OF));
        SEQ< SEQ<Char> > txts;
        txts.ImpAppend(SEQ<Char>(replaced));
        txts.ImpAppend(SEQ<Char>(TBWSTR::hexquadwstring2wstring(ignored)));
        msgs.ImpAppend(SEQ<Char>(UTIL::ReplacePercent(msg, txts).GetString()));
      }

      if ((ParseErrors == MaxErrorCount) && (this->debugflag & DEBUG_ALL_ERRORS) == 0)
      {
        SEQ<Char> msg (GetErrMessage(SYNTAXERR_STOPPING));
        SEQ< SEQ<Char> > txts;
        txts.ImpAppend(SEQ<Char>(Int(MaxErrorCount).ascii()));
        msgs.ImpAppend(SEQ<Char>(UTIL::ReplacePercent(msg, txts).GetString()));
        Report(LineNo, ColumnNo, msgs);
        break;
      }

      Report(LineNo, ColumnNo, msgs);
      if (this->debugflag > 0) {
        ParserRecover::Report(LineNo, ColumnNo, msgs);
      }

      //cur = old;
      // vdm_log << L"Change: " << this->EditOps.ascii() << endl;
    }

  }
  if (this->debugflag & DEBUG_PREPARSE) {
    wcerr << endl << L"preparse done errors= " << ParseErrors << endl;
  }
  delete[] stack0;
  delete[] stack1;
  delete[] stack2;
  return ParseErrors;
}

short * ParserRecover::CopyStack(const short * stack_base, const short * stack_top, short * stack_copy)
{
  int size = stack_top + 1 - stack_base;
  memcpy((wchar_t*)stack_copy, stack_base, size * sizeof(short));
  return stack_copy + size - 1;
}

int ParserRecover::ParseOneChar(short *sstp_base, short * & sstp /* is modified */, int yychar)
{
  // Return PARS_OK, PARS_FINAL, PARS_ERROR
  short yystate = *sstp;

  short yychar1;
  if (yychar <= 0) {
    yychar1 = 0;
    yychar = YYEOF;
  } else {
    yychar1 = this->YYTRANSLATE(yychar);
  }

  short yylen; // must be before label:

label:
  if (this->debugflag & DEBUG_PARSE_ONE) {
    wcerr << L"ParseOne: "<<  GetString((int) yychar).c_str()
          << L"'" << (short) (yychar < 256 ? yychar : '?') << L"'"
          << L" state=" << (short)(*sstp)
          << L" stack=" << (void*)sstp;
  }

  short yyn = this->yypact[yystate];
  if (yyn == this->YYFLAG)
    goto yydefault;

  // get yychar (above ^^^)

  yyn += yychar1;
  if (yyn < 0 || yyn > this->YYLAST || this->yycheck[yyn] != yychar1)
    goto yydefault;

  /* Should we shift? */
  yyn = this->yytable[yyn];
  if (yyn < 0)
  {
    if (yyn == this->YYFLAG)
      goto yyerrlab;
    yyn = -yyn;
    goto yyreduce;
  }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == this->YYFINAL) {
    if (this->debugflag & DEBUG_PARSE_ONE) {
      wcerr << L" parse_final" << endl;
    }
    return PARS_FINAL;
  }

  // Shift the lookahead token

  // Push new state
  yystate = yyn;
  *++sstp = yystate;
  if (this->debugflag & DEBUG_PARSE_ONE) {
    wcerr <<  L" parse_ok" << endl;
  }
  if (yychar == YYEOF)
    goto label;
  return PARS_OK;

yydefault:
  yyn = this->yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

yyreduce:
  yylen = this->yyr2[yyn];
  sstp -= yylen;
  yyn = this->yyr1[yyn];
  yystate = this->yypgoto[yyn - this->YYNTBASE] + *sstp;
  if (yystate >= 0 && yystate <= this->YYLAST && this->yycheck[yystate] == *sstp)
    yystate = this->yytable[yystate];
  else
    yystate = this->yydefgoto[yyn - this->YYNTBASE];
  // Happens in yynewstate:
  *++sstp = yystate;

  //vdm_log << L"After reducing go to state: " << state << endl;

  parse_stack_check(sstp_base, sstp);
  if (this->debugflag & DEBUG_PARSE_ONE) {
    wcerr << L" reduce to state: " << yystate << endl;
  }
  goto label;

yyerrlab:
  if (this->debugflag & DEBUG_PARSE_ONE) {
    wcerr << L" error" << endl;
  }
  return PARS_ERROR;
}

//virtual method
void ParserRecover::Report(int line, int column, const type_cLL & msgs)
{
  // This default Report outputs to stdout.
  vdm_log << L"l." << line << L" c." << column << L":" << endl;
  size_t len_msgs = msgs.Length();
  for (size_t i = 1; i <= len_msgs; i++) {
    vdm_log << L"  " << msgs[i].GetString() << endl;
  }
}

// Return:
// C in [begin, end] if we succeded to parse token [begin, C-1]
// before syntax error.  C =-n if we parsed EOF after reading token
// number n in [begin, end]

int ParserRecover::Reparse(short *sstp_base, short * & sstp, int begin, int end)
{
  for (int index = begin; index != end; index++)
  {
    short token = this->Input_Num_Tokens[index];
    int pars_stat = ParseOneChar(sstp_base, sstp, token);

    switch(pars_stat) {
      case PARS_ERROR: return index;
      case PARS_FINAL: return -(index++);
    }
  }
  return end;
}

void ParserRecover::OutputBetterRepair(int skip, int offset, int min, int count,int better)
{
//  unsigned int i;
  wcerr << L"C = " << count << L" dist = " << min << L" O = " << offset
        << L" del = " << skip << L" bet = " << better;
  wcerr << L"  -{";
  for (int i = 0; i < skip; i++) {
    wcerr << GetString(this->Input_Num_Tokens[i + offset]).c_str();
  }

  wcerr << L" } +{";
  for (unsigned int j = 0; j < this->Continuation_Array.size(); j++) {
    wcerr << " " <<  GetString(this->Continuation_Array[j]).c_str();
  }

  wcerr << L" } ";
  for (int k = skip; k < skip + 4; k++) {
    wcerr << GetString(this->Input_Num_Tokens[k + offset]).c_str();
    if (this->Input_Num_Tokens[k + offset] == 0)
      break;
  }

  wcerr << endl;
}

void ParserRecover::EvaluateContinuation(const short *sstp_base, const short *sstp_top, int offset)
{
  int maxremove = (this->Input_Num_Tokens.size()-offset < CONT_LEN)
                         ? this->Input_Num_Tokens.size()-offset : CONT_LEN;

  short *copy_base = new short[this->YYSTACKSIZE];

  for (int remove = 0; remove <= maxremove ; remove++)
  {
    short * copy_top = CopyStack(sstp_base, sstp_top, copy_base);
    int count = Reparse(copy_base, copy_top, offset + remove, this->Input_Num_Tokens.size());
    if (count < 0) count = -count; // Make count absolute.

    int good = 0;
    int min = SHRT_MAX;
    if (count >= this->Best.Best_count) {
      good = -1;
      min = MinDist(this->Input_Num_Tokens.begin() + offset,
                    remove,
                    this->Continuation_Array.begin(),
                    this->Continuation_Array.size(),
                    false);

      if ((count > this->Best.Best_count) ||
          (min < this->Best.BestRepairDist) ||
          ((min == this->Best.BestRepairDist) && (offset > this->Best.Best_offset)))
      {
        this->Best.Best_count = count;
        this->Best.Best_offset = offset;
        this->Best.Best_Repair_Array = this->Continuation_Array;
        this->Best.BestRepairDist = min;
        this->Best.BadLength = remove;
        good = 1;
      }
    }
    if (((good > 0) && (this->debugflag & DEBUG_GOOD_CONT)) ||
        ((good != 0) && (this->debugflag & DEBUG_CONFLICT_CONT)) ||
        (this->debugflag & DEBUG_ALL_CONT))
    {
      OutputBetterRepair(remove, offset, min, count, good);
    }
    if (this->Input_Num_Tokens[offset+remove] == 0) {
      // We have reached the last token in the input, so lets break;
      break;
    }
  }
  delete[] copy_base;
}

// Prepare repair_tokens by temporary enabling the tokens
// which are the first `length' of toks.

void ParserRecover::EnableTokens(NumTokenArray::iterator toks, int length)
{
  for (TokenTableType::iterator iter(this->repair_tokens.begin()); iter != this->repair_tokens.end(); iter++)
  {
    // Valid repair token ?
    iter->second.validRepair &= TokenTableEntry::IS_VALID;
  }

  for(int i = 0; i < length; i++, ++toks) {
    int tkidx = *toks;
    TokenTableType::iterator mitr (this->repair_tokens.find(tkidx));
    if (mitr != this->repair_tokens.end())
      mitr->second.validRepair |= TokenTableEntry::TMP_VALID;
  }
}

void ParserRecover::GenerateRepairs(const short *sstp_base, const short *sstp, int cont_length, int depth, int offset)
{
  if (depth == 0) {
    this->Continuation_Array.erase(this->Continuation_Array.begin(), this->Continuation_Array.end());
    this->ExpectedTokens.erase(this->ExpectedTokens.begin(), this->ExpectedTokens.end());
  }

  EvaluateContinuation(sstp_base, sstp, offset);
  if (this->Continuation_Array.size() > 0 && this->Continuation_Array.back() == 0)
    return;                     // We are in a state after EOF.

  if (depth < cont_length)
  {
    short *stack = new short[this->YYSTACKSIZE];
    for (TokenTableType::const_iterator iter(this->repair_tokens.begin()); iter != this->repair_tokens.end(); iter++)
    {
      // Valid repair token ?
      if (iter->second.validRepair == 0) continue;

      int token = iter->first;
      short *stack_copy = CopyStack(sstp_base, sstp, stack);

      int pars_stat = ParseOneChar(stack, stack_copy, token);
      switch(pars_stat) {
        case PARS_OK:
        case PARS_FINAL: {

//          if (this->debugflag & DEBUG_TRY_TOKENS) {
//            fwprintf(stderr, L"%*sTry token: %s, state=%d, stack_copy=%p\n",
//                    depth*4, L"",
//                    GetString((int)token).c_str(), (int)*stack_copy,
//                    (void*)stack_copy);
//          }

          parse_stack_check(stack, stack_copy);

          if (depth == 0) {
            if (token != 0)     // Don't expect EOF
              this->ExpectedTokens.push_back(RecoverToken(token, wstring(L""), 0, 0));
          }
          this->Continuation_Array.push_back(token);
          GenerateRepairs(stack, stack_copy, cont_length, depth + 1, offset);
          this->Continuation_Array.pop_back();
          break;
        }
        case PARS_ERROR: {
        // We never got to shift token. Try the next.
          break;
        }
      }
    }
    delete[] stack;
  }
}

