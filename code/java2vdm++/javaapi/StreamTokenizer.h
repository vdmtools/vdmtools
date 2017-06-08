#ifndef __StreamTokenizer_h__
#define __StreamTokenizer_h__

#include "Object.h"
#include <string>
//using namespace std;
#include "metaiv.h"

class StreamTokenizer : public vdm_Object {

private:
    Sequence buf;

    Sequence input;

    int currentIndex;
    int peekc;

    bool pushedBack;
    bool forceLower;

    /** The line number of the last token read */
    int LINENO;

    bool eolIsSignificantP;
    bool slashSlashCommentsP;
    bool slashStarCommentsP;

    int ctypes[256];
    static const int NEED_CHAR;
    static const int SKIP_LF;
    static const int ctype_length;
    static const int CT_WHITESPACE;
    static const int CT_DIGIT;
    static const int CT_ALPHA;
    static const int CT_QUOTE;
    static const int CT_COMMENT;
    static const int TT_NOTHING;

    int read();
    void toLowerCase();

    static StreamTokenizer* GetDLPtr(const DLObject& obj);

public:
    StreamTokenizer();
    Generic DlMethodCall (const wchar_t* name, const Sequence &p);

    int ttype;
    static const wchar_t * name;
    static const int TT_EOF;
    static const int TT_EOL;
    static const int TT_NUMBER;
    static const int TT_WORD;

    string sval;
    double nval;

    StreamTokenizer(const Sequence&);

    void resetSyntax();
    void wordChars(Int low, Int hi);
    void whitespaceChars(Int low, Int hi);
    void ordinaryChars(Int low, Int hi);
    void ordinaryChar(Int ch);
    void commentChar(Char ch);
    void quoteChar(Char ch);
    void parseNumbers();
    void eolIsSignificant(Bool flag);
    void slashStarComments(Bool flag);
    void slashSlashComments(Bool flag);
    void lowerCaseMode(Bool fl);

    Int nextToken();
    void pushBack();
    Int lineno();
    Sequence toString();
};

#endif // __StreamTokenizer_h__
