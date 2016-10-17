#include "StreamTokenizer.h"
#include "tb_wstring.h"

const wchar_t* StreamTokenizer::name = L"JavaIoStreamTokenizer";

const int StreamTokenizer::ctype_length = 256;
const int StreamTokenizer::CT_WHITESPACE = 1;
const int StreamTokenizer::CT_DIGIT = 2;
const int StreamTokenizer::CT_ALPHA = 4;
const int StreamTokenizer::CT_QUOTE = 8;
const int StreamTokenizer::CT_COMMENT = 16;
const int StreamTokenizer::TT_NOTHING = -4;
const int StreamTokenizer::TT_EOF = -1;
const int StreamTokenizer::TT_EOL = '\n';
const int StreamTokenizer::TT_NUMBER = -2;
const int StreamTokenizer::TT_WORD = -3;
const int StreamTokenizer::NEED_CHAR = 0x7fffffff;
const int StreamTokenizer::SKIP_LF =  0x7ffffffe;


StreamTokenizer* StreamTokenizer::GetDLPtr(const DLObject &obj) {
  StreamTokenizer *objPtr = (StreamTokenizer*) obj.GetPtr();
  return objPtr;
}

Generic StreamTokenizer::DlMethodCall (const wchar_t* name, 
				       const Sequence &p) {
  Generic res;
  if (!wcscmp(name, L"JavaIoStreamTokenizer'")) {
    if (p.Length()==1) 
      *this=StreamTokenizer(Sequence(p.Hd()));
  }
  else if (!wcscmp(name, L"lineno") && p.Length() == 0)
    res = this->lineno();
  else if (!wcscmp(name, L"pushBack") && p.Length() == 0)
    this->pushBack();
  else if (!wcscmp(name, L"toString") && p.Length() == 0)
    res = this->toString();
  else if (!wcscmp(name, L"nextToken") && p.Length() == 0)
    res = this->nextToken();
  else if (!wcscmp(name, L"quoteChar") && p.Length() == 1)
    this->quoteChar(Char((wchar_t)Real(p.Hd()).GetValue()));
  else if (!wcscmp(name, L"resetSyntax") && p.Length() == 0)
    this->resetSyntax();
  else if (!wcscmp(name, L"commentChar") && p.Length() == 1)
    this->commentChar(Char((wchar_t)Real(p.Hd()).GetValue()));
  else if (!wcscmp(name, L"parseNumbers") && p.Length() == 0)
    this->parseNumbers();
  else if (!wcscmp(name, L"wordChars") && p.Length() == 2)
    this->wordChars(Int(p[1]), Int(p[2]));
  else if (!wcscmp(name, L"ordinaryChar") && p.Length() == 1)
    this->ordinaryChar(Int(p.Hd()));
  else if (!wcscmp(name, L"lowerCaseMode") && p.Length() == 1)
    this->lowerCaseMode(Bool(p.Hd()));
  else if (!wcscmp(name, L"ordinaryChars") && p.Length() == 2)
    this->ordinaryChars(Int(p[1]), Int(p[2]));
  else if (!wcscmp(name, L"eolIsSignificant") && p.Length() == 1)
    this->eolIsSignificant(Bool(p.Hd()));
  else if (!wcscmp(name, L"slashStarComments") && p.Length() == 1)
    this->slashStarComments(Bool(p.Hd()));
  else if (!wcscmp(name, L"whitespaceChars") && p.Length() == 2)
    this->whitespaceChars(Int(p[1]), Int(p[2]));
  else if (!wcscmp(name, L"slashSlashComments") && p.Length() == 1)
    this->slashSlashComments(Bool(p.Hd()));
  else if (!wcscmp(name, L"getTtype") && p.Length() == 0)
    res = Int(ttype);
  else if (!wcscmp(name, L"getNval") && p.Length() == 0)
    res = Real(nval);
  else if (!wcscmp(name, L"getSval") && p.Length() == 0)
    res = Sequence(TBWSTR::string2wstring(sval));
  else
    throw 1;

  return res;
}

StreamTokenizer::StreamTokenizer() {
  resetSyntax();
  LINENO = 1;
  peekc = NEED_CHAR;
  eolIsSignificantP = false;
  slashSlashCommentsP = false;
  slashStarCommentsP = false;
  ttype = TT_NOTHING;
  wordChars(Int('a'), Int('z'));
  wordChars(Int('A'), Int('Z'));
  wordChars(Int(128 + 32), Int(255));
  whitespaceChars(Int(0), Int(' '));
  commentChar(Char('/'));
  quoteChar(Char('"'));
  quoteChar(Char('\''));
  parseNumbers();
  pushedBack = false;
  forceLower = false;
}

StreamTokenizer::StreamTokenizer(const Sequence& p_Input) {
  resetSyntax();
  LINENO = 1;
  peekc = NEED_CHAR;
  eolIsSignificantP = false;
  slashSlashCommentsP = false;
  slashStarCommentsP = false;
  ttype = TT_NOTHING;
  wordChars(Int('a'), Int('z'));
  wordChars(Int('A'), Int('Z'));
  wordChars(Int(128 + 32), Int(255));
  whitespaceChars(Int(0), Int(' '));
  commentChar(Char('/'));
  quoteChar(Char('"'));
  quoteChar(Char('\''));
  parseNumbers();
  input = p_Input;
  currentIndex = 1;
  pushedBack = false;
  forceLower = false;
}

void StreamTokenizer::resetSyntax() {
  for (int i = ctype_length; --i >= 0;)
    ctypes[i] = 0;
}

void StreamTokenizer::wordChars(Int low_, Int hi_) {
  int low = low_.GetValue(); int hi = hi_.GetValue();
  if (low < 0)
    low = 0;
  if (hi >= ctype_length)
    hi = ctype_length - 1;
  while (low <= hi)
    ctypes[low++] |= CT_ALPHA;
}

void StreamTokenizer::whitespaceChars(Int low_, Int hi_) {
  int low = low_.GetValue(); int hi = hi_.GetValue();
  if (low < 0)
    low = 0;
  if (hi >= ctype_length)
    hi = ctype_length - 1;
  while (low <= hi)
    ctypes[low++] = CT_WHITESPACE;
}

void StreamTokenizer::ordinaryChars(Int low_, Int hi_) {
  int low = low_.GetValue(); int hi = hi_.GetValue();
  if (low < 0)
    low = 0;
  if (hi >= ctype_length)
    hi = ctype_length - 1;
  while (low <= hi)
    ctypes[low++] = 0;
}

void StreamTokenizer::ordinaryChar(Int ch_) {
  int ch = ch_.GetValue();
  if (ch >= 0 && ch < ctype_length)
    ctypes[ch] = 0;
}

void StreamTokenizer::commentChar(Char ch_) {
  int ch = ch_.GetValue();
  if (ch >= 0 && ch < ctype_length)
    ctypes[ch] = CT_COMMENT;
}

void StreamTokenizer::quoteChar(Char ch_) {
  int ch = ch_.GetValue();
  if (ch >= 0 && ch < ctype_length)
    ctypes[ch] = CT_QUOTE;
}

void StreamTokenizer::parseNumbers() {
  for (int i = '0'; i <= '9'; i++)
    ctypes[i] |= CT_DIGIT;
  ctypes['.'] |= CT_DIGIT;
  ctypes['-'] |= CT_DIGIT;
}

void StreamTokenizer::eolIsSignificant(Bool flag) {
  eolIsSignificantP = flag.GetValue();
}

void StreamTokenizer::slashStarComments(Bool flag) {
  slashStarCommentsP = flag.GetValue();
}

void StreamTokenizer::slashSlashComments(Bool flag) {
  slashSlashCommentsP = flag.GetValue();
}

void StreamTokenizer::lowerCaseMode(Bool fl) {
  forceLower = fl.GetValue();
}

int StreamTokenizer::read() {
  if (currentIndex <= input.Length())
    return Char(input[currentIndex++]).GetValue();
  else
    return -1;
}

Int StreamTokenizer::nextToken() {
  buf = Sequence();
  if (pushedBack) {
    pushedBack = false;
    return Int(ttype);
  }
  sval = "";

  int c = peekc;
  if (c < 0)
    c = NEED_CHAR;
  if (c == SKIP_LF) {
    c = read();
    if (c < 0){
      return Int(ttype = TT_EOF);
    }
    if (c == '\n')
      c = NEED_CHAR;
  }
  if (c == NEED_CHAR) {
    c = read();
    if (c < 0){
      return Int(ttype = TT_EOF);
    }
  }
  ttype = c;		/* Just to be safe */

  /* Set peekc so that the next invocation of nextToken will read
   * another character unless peekc is reset in this invocation
   */
  peekc = NEED_CHAR;

  int ctype = c < 256 ? ctypes[c] : CT_ALPHA;
  while ((ctype & CT_WHITESPACE) != 0) {
    if (c == '\r') {
      LINENO++;
      if (eolIsSignificantP) {
	peekc = SKIP_LF;
	return Int(ttype = TT_EOL);
      }
      c = read();
      if (c == '\n')
	c = read();
    } else {
      if (c == '\n') {
	LINENO++;
	if (eolIsSignificantP) {
	  return Int(ttype = TT_EOL);
	}
      }
      c = read();
    }
    if (c < 0){
      return Int(ttype = TT_EOF);
    }
    ctype = c < 256 ? ctypes[c] : CT_ALPHA;
  }
  
  if ((ctype & CT_DIGIT) != 0) {
    bool neg = false;
    if (c == '-') {
      c = read();
      if (c != '.' && (c < '0' || c > '9')) {
	peekc = c;
	return Int(ttype = '-');
      }
      neg = true;
    }
    double v = 0;
    int decexp = 0;
    int seendot = 0;
    while (true) {
      if (c == '.' && seendot == 0)
	seendot = 1;
      else if ('0' <= c && c <= '9') {
	v = v * 10 + (c - '0');
	decexp += seendot;
      } else
	break;
      c = read();
    }
    peekc = c;
    if (decexp != 0) {
      double denom = 10;
      decexp--;
      while (decexp > 0) {
	denom *= 10;
	decexp--;
      }
      /* Do one division of a likely-to-be-more-accurate number */
      v = v / denom;
    }
    nval = neg ? -v : v;
    return Int(ttype = TT_NUMBER);
  }
  
  if ((ctype & CT_ALPHA) != 0) {
    int i = 0;
    do {
      buf.ImpAppend(Char((char) c));
      c = read();
      ctype = c < 0 ? CT_WHITESPACE : c < 256 ? ctypes[c] : CT_ALPHA;
    } while ((ctype & (CT_ALPHA | CT_DIGIT)) != 0);
    peekc = c;
    wstring tempStr;
    buf.GetString(tempStr);
    sval = TBWSTR::wstring2string(tempStr);
    if (forceLower)
      toLowerCase();
    return Int(ttype = TT_WORD);
  }

  if ((ctype & CT_QUOTE) != 0) {
    ttype = c;
    int i = 0;
    /* Invariants (because \Octal needs a lookahead):
     *   (i)  c contains char value
     *   (ii) d contains the lookahead
     */
    int d = read();
    while (d >= 0 && d != ttype && d != '\n' && d != '\r') {
      if (d == '\\') {
	c = read();
	int first = c;   /* To allow \377, but not \477 */
	if (c >= '0' && c <= '7') {
	  c = c - '0';
	  int c2 = read();
	  if ('0' <= c2 && c2 <= '7') {
	    c = (c << 3) + (c2 - '0');
	    c2 = read();
	    if ('0' <= c2 && c2 <= '7' && first <= '3') {
	      c = (c << 3) + (c2 - '0');
	      d = read();
	    } else
	      d = c2;
	  } else
	    d = c2;
	} else {
	  switch (c) {
	  case 'a':
	    c = 0x7;
	    break;
	  case 'b':
	    c = '\b';
	    break;
	  case 'f':
	    c = 0xC;
	    break;
	  case 'n':
	    c = '\n';
	    break;
	  case 'r':
	    c = '\r';
	    break;
	  case 't':
	    c = '\t';
	    break;
	  case 'v':
	    c = 0xB;
	    break;
	  }
	  d = read();
	}
      } else {
	c = d;
	d = read();
      }
      buf.ImpAppend(Char((char)c));
    }

    /* If we broke out of the loop because we found a matching quote
     * character then arrange to read a new character next time
     * around; otherwise, save the character.
     */
    peekc = (d == ttype) ? NEED_CHAR : d;
    
    wstring tempStr;
    buf.GetString(tempStr);
    sval = TBWSTR::wstring2string(tempStr);
    return Int(ttype);
  }

  if (c == '/' && (slashSlashCommentsP || slashStarCommentsP)) {
    c = read();
    if (c == '*' && slashStarCommentsP) {
      int prevc = 0;
      while ((c = read()) != '/' || prevc != '*') {
	if (c == '\r') {
	  LINENO++;
	  c = read();
	  if (c == '\n') {
	    c = read();
	  }
	} else {
	  if (c == '\n') {
	    LINENO++;
	    c = read();
	  }
	}
	if (c < 0){
	  return Int(ttype = TT_EOF);
	}
	prevc = c;
      }
      return nextToken();
    } else if (c == '/' && slashSlashCommentsP) {
      while ((c = read()) != '\n' && c != '\r' && c >= 0);
      peekc = c;
      return nextToken();
    } else {
      /* Now see if it is still a single line comment */
      if ((ctypes['/'] & CT_COMMENT) != 0) {
	while ((c = read()) != '\n' && c != '\r' && c >= 0);
	peekc = c;
	return nextToken();
      } else {
	peekc = c;
	return Int(ttype = '/');
      }
    }
  }
  
  if ((ctype & CT_COMMENT) != 0) {
    while ((c = read()) != '\n' && c != '\r' && c >= 0);
    peekc = c;
    return nextToken();
  }
  return Int(ttype = c);
}

void StreamTokenizer::pushBack() {
  if (ttype != TT_NOTHING)   /* No-op if nextToken() not called */
    pushedBack = true;
}

Int StreamTokenizer::lineno() {
  return Int(LINENO);
}

Sequence StreamTokenizer::toString() {
  string ret;
  switch (ttype) {
  case TT_EOF:
    ret = "EOF";
    break;
  case TT_EOL:
    ret = "EOL";
    break;
  case TT_WORD:
    ret = sval;
    break;
  case TT_NUMBER:
    ret = "n=";
    break;
  case TT_NOTHING:
    ret = "NOTHING";
    break;
  default: {
      /* 
       * ttype is the first character of either a quoted string or
       * is an ordinary character. ttype can definitely not be less
       * than 0, since those are reserved values used in the previous
       * case statements
       */
      if (ttype < 256 && 
	  ((ctypes[ttype] & CT_QUOTE) != 0)) {
	ret = sval;
	break;
      }
      
      char s[3];
      s[0] = s[2] = '\'';
      s[1] = (char) ttype;
      ret = string(s);
      break;
    }
  }
  ret =  "Token[" + ret + "], line ";
  return Sequence(TBWSTR::string2wstring(ret));
}

void StreamTokenizer::toLowerCase()
{
}
