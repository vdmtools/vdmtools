/***
*  * WHAT
*  *    This file is part of the rtf parser toolset
*  *    that has been downloaded from www
*  *    http://night.primate.wisc.edu:80/software/RTF
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/reader.cc,v $
*  * VERSION
*  *    $Revision: 1.15 $
*  * DATE
*  *    $Date: 2006/06/01 08:45:37 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *   
*  * AUTHOR
*  *    Lars T. Olsen + $Author: vdmtools $
*  * LOG
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
/*
 * - Need to document error code meanings.
 * - Need to do something with \* on destinations.
 * - Make the parameter a long?
 *
 * reader.c - RTF file reader.  Release 1.10.
 *
 * ASCII 10 (\n) and 13 (\r) are ignored and silently discarded.
 * Nulls are also discarded.
 * (although the read hook will still get a look at them.)
 *
 * L"\:" is not a ":", it's a control symbol.  But some versions of
 * Word seem to write L"\:" for ":".  This reader treats "\:" as a
 * plain text L":"
 *
 * 19 Mar 93
 * - Add hack to skip L"{\*\keycode ... }" group in stylesheet.
 * This is probably the wrong thing to do, but it's simple.
 * 13 Jul 93
 * - Add THINK C awareness to malloc() declaration.  Necessary so
 * compiler knows the malloc argument is 4 bytes.  Ugh.
 * 07 Sep 93
 * - Text characters are mapped onto standard codes, which are placed
 * in rtfMinor.
 * - Eliminated use of index() function.
 * 05 Mar 94
 * - Added zillions of new symbols (those defined in RTF spec 1.2).
 * 14 Mar 94
 * - Public functions RTFMsg() and RTFPanic() now take variable arguments.
 * This means RTFPanic() now is used in place of what was formerly the
 * internal function Error().
 * - 8-bit characters are now legal, so they're not converted to \'xx
 * hex wchar_t representation now.
 * 01 Apr 94
 * - Added public variables rtfLineNum and rtfLinePos.
 * - #include wstring.h or strings.h, avoiding strncmp() problem where
 * last argument is treated as zero when prototype isn't available.
 * 04 Apr 94
 * - Treat style numbers 222 and 0 properly as L"no style" and "normal".
 */

#include  <stdio.h>
#include  <ctype.h>
#include  <stdarg.h>

#include  "tokenscan.h"
#include  "rtf.h"
#include "reader.h"

#include <string.h>
#include <stringext.h>
#include <fstream>

#include "tb_exceptions.h"
#include "rt_errmsg.h"
#include "tbutils.h"
#include "tb_wstring.h"

#if !defined(__darwin__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif // !defined(__darwin__) && !defined(__FreeBSD__)

/*
 * Public variables 
 */

int rtfClass;
int rtfMajor;
int rtfMinor;
int rtfParam;
char *rtfTextBuf = (char *) NULL;
int rtfTextLen;

long rtfLineNum;
int rtfLinePos;

bool wasBSlashPrimeSym = false;

bool ansi = true;
int codepage = 0;
int deflang = 0;
int deflangfe = 0;
// private functions

static void _RTFGetToken ();
static void _RTFGetToken2 ();
static int GetChar ();
static void ReadFontTbl ();
static void ReadColorTbl ();
static void ReadStyleSheet ();
static void ReadInfoGroup ();
static void ReadPictGroup ();
static void ReadObjGroup ();
static void LookupInit ();
static void Lookup (char *);
static int SHash (const char *);

static void CharSetInit ();
static void ReadCharSetMaps ();
void RTFSetCurFontNum(int fn);
void RTFSetNonAsciiFontNum(int fn);

// 20060324 for unicode 
static void ReadNewDestination();
static void ReadTwoEmbedded();
static void ReadUnicode();
static void ReadThemeData();
static void ReadColorSchemeMapping();
static void ReadLatentStyles();
static void ReadDataStore();
static void FactoIdName();

static void ACharAttr();
static void RTFVersion();
static void ANSICodePage();
static void DefaultLanguage();
static void DefaultLanguageFE();

static void RTFBookmarkStart();
static void RTFBookmarkEnd();

/* Return pointer to new element of type t, or NULL
 * if no memory available.
 */

# define New(t)	((t *) RTFAlloc ((int) sizeof (t)))


/* maximum number of character values representable in a byte */

# define charSetSize 256

/* charset stack size */

# define maxCSStack 50


#ifndef THINK_C
extern wchar_t *malloc ();
#else
extern void *malloc(size_t);
#endif

/*
 * Private stuff
 */

static int pushedChar;	/* pushback wchar_t if read too far */

static int pushedClass;	/* pushed token info for RTFUngetToken() */
static int pushedMajor;
static int pushedMinor;
static int pushedParam;
static char *pushedTextBuf = (char *) NULL;

static int prevChar;
static int bumpLine;


static RTFFont	*fontList = (RTFFont *) NULL;	/* these lists MUST be */
static RTFColor	*colorList = (RTFColor *) NULL;	/* initialized to NULL */
static RTFStyle	*styleList = (RTFStyle *) NULL;


static ifstream	*rtffp = NULL;

static wchar_t *inputName = (wchar_t *) NULL;
static wchar_t *outputName = (wchar_t *) NULL;

/*
 * This array is used to map standard character names onto their numeric codes.
 * The position of the name within the array is the code.
 * stdcharnames.h is generated in the ../h directory.
 */

static const char *stdCharName[] =
{
# include "stdcharnames.h"
    (const char*) NULL
};

/*
 * These arrays are used to map RTF input character values onto the standard
 * character names represented by the values.  Input character values are
 * used as indices into the arrays to produce standard character codes.
 */

static wchar_t *genCharSetFile = (wchar_t *) NULL;
static int genCharCode[charSetSize];	/* general */
static int haveGenCharSet = 0;

static wchar_t *symCharSetFile = (wchar_t *) NULL;
static int symCharCode[charSetSize];	/* symbol */
static int haveSymCharSet = 0;

static int curCharSet = rtfCSGeneral;
static int *curCharCode = genCharCode;
static int curFontNum = -1;
static int nonAsciiFontNum = -1;

/*
 * By default, the reader is configured to handle charset mapping invisibly,
 * including reading the charset files and switching charset maps as necessary
 * for Symbol font.
 */

static int autoCharSetFlags;

/*
 * Stack for keeping track of charset map on group begin/end.  This is
 * necessary because group termination reverts the font to the previous
 * value, which may implicitly change it.
 */

static int csStack[maxCSStack];
static int csTop = 0;

/*
 * Initialize the reader.  This may be called multiple times,
 * to read multiple files.  The only thing not reset is the input
 * stream; that must be done with RTFSetStream().
 */
void RTFInit ()
{
  int i;
  RTFColor *cp;
  RTFFont *fp;
  RTFStyle *sp;
  RTFStyleElt *eltList, *ep;

  if (rtfTextBuf == (char *) NULL)  /* initialize the text buffers */
  {
    rtfTextBuf = RTFAlloc (rtfBufSiz);
    pushedTextBuf = RTFAlloc (rtfBufSiz);
    if (rtfTextBuf == (char *) NULL
      || pushedTextBuf == (char *) NULL)
      RTFPanic (L"Cannot allocate text buffers.");
    rtfTextBuf[0] = pushedTextBuf[0] = '\0';
  }

  RTFWFree (inputName);
  RTFWFree (outputName);
  inputName = outputName = (wchar_t *) NULL;
      
  /* initialize lookup table */
  LookupInit ();

  for (i = 0; i < rtfMaxClass; i++)
    RTFSetClassCallback (i, (RTFFuncPtr) NULL);
  for (i = 0; i < rtfMaxDestination; i++)
    RTFSetDestinationCallback (i, (RTFFuncPtr) NULL);

  /* install built-in destination readers */
  RTFSetDestinationCallback (rtfFontTbl, ReadFontTbl);
  RTFSetDestinationCallback (rtfColorTbl, ReadColorTbl);
  RTFSetDestinationCallback (rtfStyleSheet, ReadStyleSheet);
  RTFSetDestinationCallback (rtfInfo, ReadInfoGroup);
  RTFSetDestinationCallback (rtfPict, ReadPictGroup);
  RTFSetDestinationCallback (rtfObject, ReadObjGroup);

  // 20060324 for unicode
  RTFSetDestinationCallback (rtfNewDestination, ReadNewDestination);
  RTFSetDestinationCallback (rtfTwoEmbedded, ReadTwoEmbedded);
  RTFSetDestinationCallback (rtfUnicode, ReadUnicode);
  RTFSetDestinationCallback (rtfThemeData, ReadThemeData);
  RTFSetDestinationCallback (rtfColorSchemeMapping, ReadColorSchemeMapping);
  RTFSetDestinationCallback (rtfLatentStyles, ReadLatentStyles);
  RTFSetDestinationCallback (rtfDataStore, ReadDataStore);
  RTFSetDestinationCallback (rtfFactoIdName, FactoIdName);

  RTFSetDestinationCallback (rtfBookmarkStart, RTFBookmarkStart);
  RTFSetDestinationCallback (rtfBookmarkEnd, RTFBookmarkEnd);

  RTFSetReadHook ((RTFFuncPtr) NULL);

  /* dump old lists if necessary */

  while (fontList != (RTFFont *) NULL)
  {
    fp = fontList->rtfNextFont;
    RTFWFree (fontList->rtfFName);
    RTFWFree ((wchar_t *) fontList);
    fontList = fp;
  }
  while (colorList != (RTFColor *) NULL)
  {
    cp = colorList->rtfNextColor;
    RTFFree ((char*) colorList);
    colorList = cp;
  }
  while (styleList != (RTFStyle *) NULL)
  {
    sp = styleList->rtfNextStyle;
    eltList = styleList->rtfSSEList;
    while (eltList != (RTFStyleElt *) NULL)
    {
      ep = eltList->rtfNextSE;
      RTFFree (eltList->rtfSEText);
      RTFWFree ((wchar_t *) eltList);
      eltList = ep;
    }
    RTFFree (styleList->rtfSName);
    RTFWFree ((wchar_t *) styleList);
    styleList = sp;
  }

  rtfClass = -1;
  pushedClass = -1;
  pushedChar = EOF;

  rtfLineNum = 0;
  rtfLinePos = 0;
  prevChar = EOF;
  bumpLine = 0;

  CharSetInit ();
  csTop = 0;
}

/*
 * Set the reader's input stream to the given stream.  Can
 * be used to redirect to other than the default (stdin).
 */
void RTFSetStream (ifstream  *stream)
{
  rtffp = stream;
}

/*
 * Set or get the input or output file name.  These are never guaranteed
 * to be accurate, only insofar as the calling program makes them so.
 */
void RTFSetInputName (wchar_t *name)
{
  if ((inputName = RTFWStrSave (name)) == (wchar_t *) NULL)
    RTFPanic (L"RTFSetInputName: out of memory");
}

wchar_t * RTFGetInputName ()
{
  return (inputName);
}

void RTFSetOutputName (wchar_t *name)
{
  if ((outputName = RTFWStrSave (name)) == (wchar_t *) NULL)
    RTFPanic (L"RTFSetOutputName: out of memory");
}

wchar_t * RTFGetOutputName ()
{
  return (outputName);
}

/* ---------------------------------------------------------------------- */

/*
 * Callback table manipulation routines
 */

/*
 * Install or return a writer callback for a token class
 */
static RTFFuncPtr  ccb[rtfMaxClass];    /* class callbacks */

void RTFSetClassCallback (int _class, RTFFuncPtr callback)
{
  if (_class >= 0 && _class < rtfMaxClass)
    ccb[_class] = callback;
}

RTFFuncPtr RTFGetClassCallback (int _class)
{
  if (_class >= 0 && _class < rtfMaxClass)
    return (ccb[_class]);
  return ((RTFFuncPtr) NULL);
}

/*
 * Install or return a writer callback for a destination type
 */
static RTFFuncPtr  dcb[rtfMaxDestination];  /* destination callbacks */

void RTFSetDestinationCallback ( int dest, RTFFuncPtr callback)
{
  if (dest >= 0 && dest < rtfMaxDestination)
    dcb[dest] = callback;
}

RTFFuncPtr RTFGetDestinationCallback (int  dest)
{
  if (dest >= 0 && dest < rtfMaxDestination)
    return (dcb[dest]);
  return ((RTFFuncPtr) NULL);
}

/* ---------------------------------------------------------------------- */

/*
 * Token reading routines
 */

/*
 * Read the input stream, invoking the writer's callbacks
 * where appropriate.
 */
void RTFRead ()
{
  while (RTFGetToken () != rtfEOF) {
    RTFRouteToken ();
  }
}

/*
 * Route a token.  If it's a destination for which a reader is
 * installed, process the destination internally, otherwise
 * pass the token to the writer's class callback.
 */

void RTFRouteToken ()
{
//wcout << L"rtfClass: " << rtfClass << L" " << TBWSTR::string2wstring(rtfTextBuf) << endl;
//wcout << L"rtfControl: " << rtfControl << endl;
//wcout << L"rtfDestination: " << rtfDestination << endl;

  RTFFuncPtr p;

  if (rtfClass < 0 || rtfClass >= rtfMaxClass)  /* watchdog */
  {
    RTFPanic (L"Unknown class " + Int(rtfClass).ascii() +
              L":" + TBWSTR::string2wstring(rtfTextBuf) + L" (reader malfunction)");
  }
  if (RTFCheckCM (rtfControl, rtfDestination))
  {
    /* invoke destination-specific callback if there is one */
    if ((p = RTFGetDestinationCallback (rtfMinor))
              != (RTFFuncPtr) NULL)
    {
      (*p) ();
      return;
    }
  }
  /* invoke class callback if there is one */
  if ((p = RTFGetClassCallback (rtfClass)) != (RTFFuncPtr) NULL)
  {
    (*p) ();
  }

  if (RTFCheckCM (rtfControl, rtfACharAttr))
  {
    ACharAttr();
  }
  else if (RTFCheckCM (rtfControl, rtfVersion))
  {
    RTFVersion();
  }
  else if (RTFCheckCM (rtfControl, rtfCodePage))
  {
    ANSICodePage();
  }
  else if (RTFCheckCMM (rtfControl, rtfDocAttr, rtfDefLanguage))
  {
    DefaultLanguage();
  }
  else if (RTFCheckCMM (rtfControl, rtfDocAttr, rtfDefLanguageFE))
  {
    DefaultLanguageFE();
  }
}

/*
 * Skip to the end of the current group.  When this returns,
 * writers that maintain a state stack may want to call their
 * state unstacker; global vars will still be set to the group's
 * closing brace.
 */

void RTFSkipGroup ()
{
  int  level = 1;

  while (RTFGetToken () != rtfEOF)
  {
    if (rtfClass == rtfGroup)
    {
      if (rtfMajor == rtfBeginGroup)
        ++level;
      else if (rtfMajor == rtfEndGroup)
      {
        if (--level < 1)
          break;  /* end of initial group */
      }
    }
  }
}

/*
 * Read one token.  Call the read hook if there is one.  The
 * token class is the return value.  Returns rtfEOF when there
 * are no more tokens.
 */
int RTFGetToken ()
{
  RTFFuncPtr  p;

  for (;;)
  {
    _RTFGetToken ();

    if ((p = RTFGetReadHook ()) != (RTFFuncPtr) NULL)
      (*p) ();  /* give read hook a look at token */

    /* Silently discard newlines, carriage returns, nulls.  */
    if (!(rtfClass == rtfText
      && (rtfMajor == '\n' || rtfMajor == '\r'
            || rtfMajor == '\0')))
      break;
  }
  return (rtfClass);
}


/*
 * Install or return a token reader hook.
 */

static RTFFuncPtr  readHook;

void RTFSetReadHook (RTFFuncPtr f)
{
  readHook = f;
}


RTFFuncPtr RTFGetReadHook ()
{
  return (readHook);
}


void RTFUngetToken ()
{
  if (pushedClass >= 0)  /* there's already an ungotten token */
    RTFPanic (L"cannot unget two tokens");
  if (rtfClass < 0)
    RTFPanic (L"no token to unget");
  pushedClass = rtfClass;
  pushedMajor = rtfMajor;
  pushedMinor = rtfMinor;
  pushedParam = rtfParam;
  //(void) strcpy (pushedTextBuf, rtfTextBuf);
  size_t len_rtfTextBuf = strlen(rtfTextBuf) + 1;
  memcpy (pushedTextBuf, rtfTextBuf, len_rtfTextBuf);
}

int RTFPeekToken ()
{
  _RTFGetToken ();
  RTFUngetToken ();
  return (rtfClass);
}

static void _RTFGetToken ()
{
  RTFFont  *fp;

  /* first check for pushed token from RTFUngetToken() */

  if (pushedClass >= 0)
  {
    rtfClass = pushedClass;
    rtfMajor = pushedMajor;
    rtfMinor = pushedMinor;
    rtfParam = pushedParam;
    //(void) strcpy (rtfTextBuf, pushedTextBuf);
    size_t len_pushedTextBuf = strlen(pushedTextBuf) + 1;
    memcpy (rtfTextBuf, pushedTextBuf, len_pushedTextBuf);
    rtfTextLen = strlen (rtfTextBuf);
    pushedClass = -1;
    return;
  }

  /*
   * Beyond this point, no token is ever seen twice, which is
   * important, e.g., for making sure no L"}" pops the font stack twice.
   */

  _RTFGetToken2 ();
  if (rtfClass == rtfText)  /* map RTF wchar_t to standard code */
    rtfMinor = RTFMapChar (rtfMajor);

  /*
   * If auto-charset stuff is activated, see if anything needs doing,
   * like reading the charset maps or switching between them.
   */

  if (autoCharSetFlags == 0)
    return;

  if ((autoCharSetFlags & rtfReadCharSet)
    && RTFCheckCM (rtfControl, rtfCharSet))
  {
    ReadCharSetMaps ();
  }
  else if ((autoCharSetFlags & rtfSwitchCharSet)
    && RTFCheckCMM (rtfControl, rtfCharAttr, rtfFontNum))
  {
    if ((fp = RTFGetFont (rtfParam)) != (RTFFont *) NULL)
    {
      if (wcsncmp (fp->rtfFName, L"Symbol", 6) == 0)
        curCharSet = rtfCSSymbol;
      else
        curCharSet = rtfCSGeneral;
      RTFSetCharSet (curCharSet);
      RTFSetCurFontNum(fp->rtfFNum);
    }
  }
  else if ((autoCharSetFlags & rtfSwitchCharSet) && rtfClass == rtfGroup)
  {
    switch (rtfMajor)
    {
    case rtfBeginGroup:
      if (csTop >= maxCSStack)
        RTFPanic (L"_RTFGetToken: stack overflow");
      csStack[csTop++] = curCharSet;
      break;
    case rtfEndGroup:
      if (csTop <= 0)
        RTFPanic (L"_RTFGetToken: stack underflow");
      curCharSet = csStack[--csTop];
      RTFSetCharSet (curCharSet);
      break;
    }
  }
}

/* this shouldn't be called anywhere but from _RTFGetToken() */

static void _RTFGetToken2 ()
{
  int  sign;
  int  c;

  /* initialize token vars */

  rtfClass = rtfUnknown;
  rtfParam = rtfNoParam;
  rtfTextBuf[rtfTextLen = 0] = '\0';

  /* get first character, which may be a pushback from previous token */

  if (pushedChar != EOF)
  {
    c = pushedChar;
    rtfTextBuf[rtfTextLen++] = c;
    rtfTextBuf[rtfTextLen] = '\0';
    pushedChar = EOF;
  }
  else if ((c = GetChar ()) == EOF)
  {
    rtfClass = rtfEOF;
    return;
  }

  if (c == '{')
  {
    rtfClass = rtfGroup;
    rtfMajor = rtfBeginGroup;
    return;
  }
  if (c == '}')
  {
    rtfClass = rtfGroup;
    rtfMajor = rtfEndGroup;
    return;
  }
  if (c != '\\')
  {
    /*
     * Two possibilities here:
     * 1) ASCII 9, effectively like \tab control symbol
     * 2) literal text wchar_t
     */
    if (c == '\t')      /* ASCII 9 */
    {
      rtfClass = rtfControl;
      rtfMajor = rtfSpecialChar;
      rtfMinor = rtfTab;
    }
    else
    {
      rtfClass = rtfText;
      rtfMajor = c;
    }
    return;
  }

  if ((c = GetChar ()) == EOF)
  {
    /* early eof, whoops (class is rtfUnknown) */
    return;
  }

  // 20061116
  if ( c == '*' )
  {
    // 
//    if ((c = GetChar ()) == EOF) return;
//    if ((c = GetChar ()) == EOF) return;
    rtfTextBuf[0] = '\0';
    return;
  }

  if (!isalpha (c))
  {
    /*
     * Three possibilities here:
     * 1) hex encoded text wchar_t, e.g., \'d5, \'d3
     * 2) special escaped text wchar_t, e.g., \{, \}
     * 3) control symbol, e.g., \_, \-, \|, \<10>
     */
    if (c == '\'')        /* hex wchar_t */
    {
      int  c2=0; // =0 to silence egcs.

      if ((c = GetChar ()) != EOF && (c2 = GetChar ()) != EOF)
      {
        /* should do isxdigit check! */
        rtfClass = rtfText;
        rtfMajor = RTFCharToHex (c) * 16 + RTFCharToHex (c2);

        if(!ansi)
        {
          RTFSetNonAsciiFontNum(RTFGetCurFontNum());
          wasBSlashPrimeSym = true;
        }
        return;
      }
      /* early eof, whoops (class is rtfUnknown) */
      return;
    }

    /* escaped wchar_t */
    if (c == ':' || c == '{' || c == '}' || c == '\\')
    {
      rtfClass = rtfText;
      rtfMajor = c;
      return;
    }

    // 20060324
//    if ( c == '*' )
//    {
// 20061116
//      rtfClass = rtfText;
//      rtfMajor = c;
//      return;
//    }

    /* control symbol */
    Lookup (rtfTextBuf);  /* sets class, major, minor */
    return;
  }

  // 20060324
  // for unicode
  if( c == 'u' )
  {
    if( ( c = GetChar() ) != EOF )
    {
      // unicode char
      if( !isalpha(c) )
      {
        std::string ustr;
        ustr += c;
        while(((c = GetChar()) != EOF) && isdigit(c)) ustr += c;
        if( c != EOF )
          if( c != ' ' )
          {
            pushedChar = c;
            rtfTextBuf[rtfTextLen-1] = '\0';
          }
        return;
      }
      else
        pushedChar = c;
    }
  }

  /* control word */
  while (isalpha (c))
  {
    if ((c = GetChar ()) == EOF)
      break;
  }

  /*
   * At this point, the control word is all collected, so the
   * major/minor numbers are determined before the parameter
   * (if any) is scanned.  There will be one too many characters
   * in the buffer, though, so fix up before and restore after
   * looking up.
   */

  if (c != EOF)
    rtfTextBuf[rtfTextLen-1] = '\0';
  Lookup (rtfTextBuf);  /* sets class, major, minor */
  if (c != EOF)
    rtfTextBuf[rtfTextLen-1] = c;

  /*
   * Should be looking at first digit of parameter if there
   * is one, unless it's negative.  In that case, next wchar_t
   * is '-', so need to gobble next wchar_t, and remember sign.
   */

  sign = 1;
  if (c == '-')
  {
    sign = -1;
    c = GetChar ();
  }
  if (c != EOF && isdigit (c))
  {
    rtfParam = 0;
    while (isdigit (c))  /* gobble parameter */
    {
      rtfParam = rtfParam * 10 + c - '0';
      if ((c = GetChar ()) == EOF)
        break;
    }
    rtfParam *= sign;
  }
  /*
   * If control symbol delimiter was a blank, gobble it.
   * Otherwise the character is first wchar_t of next token, so
   * push it back for next call.  In either case, delete the
   * delimiter from the token buffer.
   */
  if (c != EOF)
  {
    if (c != ' ')
      pushedChar = c;
    rtfTextBuf[--rtfTextLen] = '\0';
  }
}


/*
 * Read the next character from the input.  This handles setting the
 * current line and position-within-line variables.  Those variable are
 * set correctly whether lines end with CR, LF, or CRLF (the last being
 * the tricky case).
 *
 * bumpLine indicates whether the line number should be incremented on
 * the *next* input character.
 */

void AddCharToChunkBuffer(char);

static int
GetChar ()
{
  int  c;
  int  oldBumpLine;

  if ((c = rtffp->get()) != EOF)
  {
    rtfTextBuf[rtfTextLen++] = c;
    rtfTextBuf[rtfTextLen] = '\0';
    AddCharToChunkBuffer(c); 
  }
       
  if (prevChar == EOF)
    bumpLine = 1;
  oldBumpLine = bumpLine;  /* non-zero if prev wchar_t was line ending */
  bumpLine = 0;
  if (c == '\r')
    bumpLine = 1;
  else if (c == '\n')
  {
    bumpLine = 1;
    if (prevChar == '\r')    /* oops, previous \r wasn't */
      oldBumpLine = 0;  /* really a line ending */
  }
  ++rtfLinePos;
  if (oldBumpLine)  /* were we supposed to increment the */
  {      /* line count on this wchar_t? */
    ++rtfLineNum;
    rtfLinePos = 1;
  }
  prevChar = c;
  return (c);
}


/*
 * Synthesize a token by setting the global variables to the
 * values supplied.  Typically this is followed with a call
 * to RTFRouteToken().
 *
 * If a param value other than rtfNoParam is passed, it becomes
 * part of the token text.
 */

void
RTFSetToken (
             int _class, 
             int major, 
             int minor, 
             int param,
             const char *text)
{
  rtfClass = _class;
  rtfMajor = major;
  rtfMinor = minor;
  rtfParam = param;
  if (param == rtfNoParam)
  {
    //(void) strcpy (rtfTextBuf, text);
    size_t len_text = strlen(text) + 1;
    memcpy (rtfTextBuf, text, len_text);
  }
  else {
    std::string textParam(text);
    char numbuf[30];
    sprintf(numbuf,"%d", param);
    textParam += numbuf;
    //strcpy(rtfTextBuf, textParam.c_str());
    size_t len_textParam = textParam.length() + 1;
    memcpy(rtfTextBuf, textParam.c_str(), len_textParam);
  }
  rtfTextLen = strlen (rtfTextBuf);
}


/* ---------------------------------------------------------------------- */

/*
 * Routines to handle mapping of RTF character sets
 * onto standard characters.
 *
 * RTFStdCharCode(name)  given wchar_t name, produce numeric code
 * RTFStdCharName(code)  given wchar_t code, return name
 * RTFMapChar(c)  map input (RTF) wchar_t code to std code
 * RTFSetCharSet(id)  select given charset map
 * RTFGetCharSet()  get current charset map
 *
 * See ../h/README for more information about charset names and codes.
 */


/*
 * Initialize charset stuff.
 */

static void
CharSetInit ()
{
  autoCharSetFlags = (rtfReadCharSet | rtfSwitchCharSet);
  RTFWFree (genCharSetFile);
  genCharSetFile = (wchar_t *) NULL;
  haveGenCharSet = 0;
  RTFWFree (symCharSetFile);
  symCharSetFile = (wchar_t *) NULL;
  haveSymCharSet = 0;
  curCharSet = rtfCSGeneral;
  curCharCode = genCharCode;
  curFontNum = -1;
  nonAsciiFontNum = -1;
}


/*
 * Specify the name of a file to be read when auto-charset-file reading is
 * done.
 */

void
RTFSetCharSetMap (wchar_t  *name,
                  int  csId)
{
  if ((name = RTFWStrSave (name)) == (wchar_t *) NULL)  /* make copy */
    RTFPanic (L"RTFSetCharSetMap: out of memory");
  switch (csId)
  {
  case rtfCSGeneral:
    RTFWFree (genCharSetFile);  /* free any previous value */
    genCharSetFile = name;
    break;
  case rtfCSSymbol:
    RTFWFree (symCharSetFile);  /* free any previous value */
    symCharSetFile = name;
    break;
  }
}


/*
 * Do auto-charset-file reading.
 */

static void
ReadCharSetMaps ()
{
  wchar_t  buf[rtfBufSiz];

  if (genCharSetFile != (wchar_t *) NULL)
    (void) wcscpy (buf, genCharSetFile);
  else if (RTFReadCharSetMap (rtfCSGeneral) == 0)
    RTFPanic (L"ReadCharSetMaps: Cannot read charset map " +  std::wstring(buf));

  if (symCharSetFile != (wchar_t *) NULL)
    (void) wcscpy (buf, symCharSetFile);
  else {
    std::string bufStr (&rtfTextBuf[1]);
    bufStr += "-sym";
    wcsncpy(buf, TBWSTR::string2wstring(bufStr).c_str(), rtfBufSiz-1);
  }

  if (RTFReadCharSetMap (rtfCSSymbol) == 0)
    RTFPanic (L"ReadCharSetMaps: Cannot read charset map " + std::wstring(buf));
}



/*
 * Read in a file describing an RTF character set map.  Lines consist of pairs
 * associating character names with character values.
 *
 * If the filename is an absolute pathname, look in the specified location
 * only.  Otherwise try to find the file in the current directory or library.
 */

int
RTFReadCharSetMap ( int  csId)
{

int  *stdCodeArray;
int  i;

  switch (csId)
  {
  default:
    return (0);  /* illegal charset id */
  case rtfCSGeneral:
    stdCodeArray = genCharCode;
    break;
  case rtfCSSymbol:
    stdCodeArray = symCharCode;
    break;
  }

  /* clobber current mapping */

  for (i = 0; i < charSetSize; i++)
  {
    stdCodeArray[i] = rtfSC_nothing;
  }


        // Generated from ansi-sym/ansi-gen maps

   stdCodeArray[0x06] = RTFStdCharCode("formula");
   stdCodeArray[0x1e] = RTFStdCharCode("nobrkhyphen");
   stdCodeArray[0x1f] = RTFStdCharCode("opthyphen");
   stdCodeArray[(int)' '] = RTFStdCharCode("space");
   stdCodeArray[(int)'!'] = RTFStdCharCode("exclam");
   stdCodeArray[(int)'\"'] = RTFStdCharCode("quotedbl");
   stdCodeArray[(int)'#'] = RTFStdCharCode("numbersign");
   stdCodeArray[(int)'$'] = RTFStdCharCode("dollar");
   stdCodeArray[(int)'%'] = RTFStdCharCode("percent");
   stdCodeArray[(int)'&'] = RTFStdCharCode("ampersand");
   stdCodeArray[(int)'\''] = RTFStdCharCode("quoteright");
   stdCodeArray[(int)'('] = RTFStdCharCode("parenleft");
   stdCodeArray[(int)')'] = RTFStdCharCode("parenright");
   stdCodeArray[(int)'*'] = RTFStdCharCode("asterisk");
   stdCodeArray[(int)'+'] = RTFStdCharCode("plus");
   stdCodeArray[(int)','] = RTFStdCharCode("comma");
   stdCodeArray[(int)'-'] = RTFStdCharCode("hyphen");
   stdCodeArray[(int)'.'] = RTFStdCharCode("period");
   stdCodeArray[(int)'/'] = RTFStdCharCode("slash");
   stdCodeArray[(int)'0'] = RTFStdCharCode("zero");
   stdCodeArray[(int)'1'] = RTFStdCharCode("one");
   stdCodeArray[(int)'2'] = RTFStdCharCode("two");
   stdCodeArray[(int)'3'] = RTFStdCharCode("three");
   stdCodeArray[(int)'4'] = RTFStdCharCode("four");
   stdCodeArray[(int)'5'] = RTFStdCharCode("five");
   stdCodeArray[(int)'6'] = RTFStdCharCode("six");
   stdCodeArray[(int)'7'] = RTFStdCharCode("seven");
   stdCodeArray[(int)'8'] = RTFStdCharCode("eight");
   stdCodeArray[(int)'9'] = RTFStdCharCode("nine");
   stdCodeArray[(int)':'] = RTFStdCharCode("colon");
   stdCodeArray[(int)';'] = RTFStdCharCode("semicolon");
   stdCodeArray[(int)'<'] = RTFStdCharCode("less");
   stdCodeArray[(int)'='] = RTFStdCharCode("equal");
   stdCodeArray[(int)'>'] = RTFStdCharCode("greater");
   stdCodeArray[(int)'?'] = RTFStdCharCode("question");
   stdCodeArray[(int)'@'] = RTFStdCharCode("at");
   stdCodeArray[(int)'A'] = RTFStdCharCode("A");
   stdCodeArray[(int)'B'] = RTFStdCharCode("B");
   stdCodeArray[(int)'C'] = RTFStdCharCode("C");
   stdCodeArray[(int)'D'] = RTFStdCharCode("D");
   stdCodeArray[(int)'E'] = RTFStdCharCode("E");
   stdCodeArray[(int)'F'] = RTFStdCharCode("F");
   stdCodeArray[(int)'G'] = RTFStdCharCode("G");
   stdCodeArray[(int)'H'] = RTFStdCharCode("H");
   stdCodeArray[(int)'I'] = RTFStdCharCode("I");
   stdCodeArray[(int)'J'] = RTFStdCharCode("J");
   stdCodeArray[(int)'K'] = RTFStdCharCode("K");
   stdCodeArray[(int)'L'] = RTFStdCharCode("");
   stdCodeArray[(int)'M'] = RTFStdCharCode("M");
   stdCodeArray[(int)'N'] = RTFStdCharCode("N");
   stdCodeArray[(int)'O'] = RTFStdCharCode("O");
   stdCodeArray[(int)'P'] = RTFStdCharCode("P");
   stdCodeArray[(int)'Q'] = RTFStdCharCode("Q");
   stdCodeArray[(int)'R'] = RTFStdCharCode("R");
   stdCodeArray[(int)'S'] = RTFStdCharCode("S");
   stdCodeArray[(int)'T'] = RTFStdCharCode("T");
   stdCodeArray[(int)'U'] = RTFStdCharCode("U");
   stdCodeArray[(int)'V'] = RTFStdCharCode("V");
   stdCodeArray[(int)'W'] = RTFStdCharCode("W");
   stdCodeArray[(int)'X'] = RTFStdCharCode("X");
   stdCodeArray[(int)'Y'] = RTFStdCharCode("Y");
   stdCodeArray[(int)'Z'] = RTFStdCharCode("Z");
   stdCodeArray[(int)'['] = RTFStdCharCode("bracketleft");
   stdCodeArray[(int)'\\'] = RTFStdCharCode("backslash");
   stdCodeArray[(int)']'] = RTFStdCharCode("bracketright");
   stdCodeArray[(int)'^'] = RTFStdCharCode("asciicircum");
   stdCodeArray[(int)'_'] = RTFStdCharCode("underscore");
   stdCodeArray[(int)'`'] = RTFStdCharCode("quoteleft");
   stdCodeArray[(int)'a'] = RTFStdCharCode("a");
   stdCodeArray[(int)'b'] = RTFStdCharCode("b");
   stdCodeArray[(int)'c'] = RTFStdCharCode("c");
   stdCodeArray[(int)'d'] = RTFStdCharCode("d");
   stdCodeArray[(int)'e'] = RTFStdCharCode("e");
   stdCodeArray[(int)'f'] = RTFStdCharCode("f");
   stdCodeArray[(int)'g'] = RTFStdCharCode("g");
   stdCodeArray[(int)'h'] = RTFStdCharCode("h");
   stdCodeArray[(int)'i'] = RTFStdCharCode("i");
   stdCodeArray[(int)'j'] = RTFStdCharCode("j");
   stdCodeArray[(int)'k'] = RTFStdCharCode("k");
   stdCodeArray[(int)'l'] = RTFStdCharCode("l");
   stdCodeArray[(int)'m'] = RTFStdCharCode("m");
   stdCodeArray[(int)'n'] = RTFStdCharCode("n");
   stdCodeArray[(int)'o'] = RTFStdCharCode("o");
   stdCodeArray[(int)'p'] = RTFStdCharCode("p");
   stdCodeArray[(int)'q'] = RTFStdCharCode("q");
   stdCodeArray[(int)'r'] = RTFStdCharCode("r");
   stdCodeArray[(int)'s'] = RTFStdCharCode("s");
   stdCodeArray[(int)'t'] = RTFStdCharCode("t");
   stdCodeArray[(int)'u'] = RTFStdCharCode("u");
   stdCodeArray[(int)'v'] = RTFStdCharCode("v");
   stdCodeArray[(int)'w'] = RTFStdCharCode("w");
   stdCodeArray[(int)'x'] = RTFStdCharCode("x");
   stdCodeArray[(int)'y'] = RTFStdCharCode("y");
   stdCodeArray[(int)'z'] = RTFStdCharCode("z");
   stdCodeArray[(int)'{'] = RTFStdCharCode("braceleft");
   stdCodeArray[(int)'|'] = RTFStdCharCode("bar");
   stdCodeArray[(int)'}'] = RTFStdCharCode("braceright");
   stdCodeArray[(int)'~'] = RTFStdCharCode("asciitilde");
   stdCodeArray[0xa0] = RTFStdCharCode("nobrkspace");
   stdCodeArray[0xa1] = RTFStdCharCode("exclamdown");
   stdCodeArray[0xa2] = RTFStdCharCode("cent");
   stdCodeArray[0xa3] = RTFStdCharCode("sterling");
   stdCodeArray[0xa4] = RTFStdCharCode("currency");
   stdCodeArray[0xa5] = RTFStdCharCode("yen");
   stdCodeArray[0xa6] = RTFStdCharCode("brokenbar");
   stdCodeArray[0xa7] = RTFStdCharCode("section");
   stdCodeArray[0xa8] = RTFStdCharCode("dieresis");
   stdCodeArray[0xa9] = RTFStdCharCode("copyright");
   stdCodeArray[0xaa] = RTFStdCharCode("ordfeminine");
   stdCodeArray[0xab] = RTFStdCharCode("guillemotleft");
   stdCodeArray[0xac] = RTFStdCharCode("logicalnot");
   stdCodeArray[0xad] = RTFStdCharCode("opthyphen");
   stdCodeArray[0xae] = RTFStdCharCode("registered");
   stdCodeArray[0xaf] = RTFStdCharCode("macron");
   stdCodeArray[0xb0] = RTFStdCharCode("degree");
   stdCodeArray[0xb1] = RTFStdCharCode("plusminus");
   stdCodeArray[0xb2] = RTFStdCharCode("twosuperior");
   stdCodeArray[0xb3] = RTFStdCharCode("threesuperior");
   stdCodeArray[0xb4] = RTFStdCharCode("acute");
   stdCodeArray[0xb5] = RTFStdCharCode("mu");
   stdCodeArray[0xb6] = RTFStdCharCode("paragraph");
   stdCodeArray[0xb7] = RTFStdCharCode("periodcentered");
   stdCodeArray[0xb8] = RTFStdCharCode("cedilla");
   stdCodeArray[0xb9] = RTFStdCharCode("onesuperior");
   stdCodeArray[0xba] = RTFStdCharCode("ordmasculine");
   stdCodeArray[0xbb] = RTFStdCharCode("guillemotright");
   stdCodeArray[0xbc] = RTFStdCharCode("onequarter");
   stdCodeArray[0xbd] = RTFStdCharCode("onehalf");
   stdCodeArray[0xbe] = RTFStdCharCode("threequarters");
   stdCodeArray[0xbf] = RTFStdCharCode("questiondown");
   stdCodeArray[0xc0] = RTFStdCharCode("Agrave");
   stdCodeArray[0xc1] = RTFStdCharCode("Aacute");
   stdCodeArray[0xc2] = RTFStdCharCode("Acircumflex");
   stdCodeArray[0xc3] = RTFStdCharCode("Atilde");
   stdCodeArray[0xc4] = RTFStdCharCode("Adieresis");
   stdCodeArray[0xc5] = RTFStdCharCode("Aring");
   stdCodeArray[0xc6] = RTFStdCharCode("AE");
   stdCodeArray[0xc7] = RTFStdCharCode("Ccedilla");
   stdCodeArray[0xc8] = RTFStdCharCode("Egrave");
   stdCodeArray[0xc9] = RTFStdCharCode("Eacute");
   stdCodeArray[0xca] = RTFStdCharCode("Ecircumflex");
   stdCodeArray[0xcb] = RTFStdCharCode("Edieresis");
   stdCodeArray[0xcc] = RTFStdCharCode("Igrave");
   stdCodeArray[0xcd] = RTFStdCharCode("Iacute");
   stdCodeArray[0xce] = RTFStdCharCode("Icircumflex");
   stdCodeArray[0xcf] = RTFStdCharCode("Idieresis");
   stdCodeArray[0xd0] = RTFStdCharCode("Eth");
   stdCodeArray[0xd1] = RTFStdCharCode("Ntilde");
   stdCodeArray[0xd2] = RTFStdCharCode("Ograve");
   stdCodeArray[0xd3] = RTFStdCharCode("Oacute");
   stdCodeArray[0xd4] = RTFStdCharCode("Ocircumflex");
   stdCodeArray[0xd5] = RTFStdCharCode("Otilde");
   stdCodeArray[0xd6] = RTFStdCharCode("Odieresis");
   stdCodeArray[0xd7] = RTFStdCharCode("multiply");
   stdCodeArray[0xd8] = RTFStdCharCode("Oslash");
   stdCodeArray[0xd9] = RTFStdCharCode("Ugrave");
   stdCodeArray[0xda] = RTFStdCharCode("Uacute");
   stdCodeArray[0xdb] = RTFStdCharCode("Ucircumflex");
   stdCodeArray[0xdc] = RTFStdCharCode("Udieresis");
   stdCodeArray[0xdd] = RTFStdCharCode("Yacute");
   stdCodeArray[0xde] = RTFStdCharCode("Thorn");
   stdCodeArray[0xdf] = RTFStdCharCode("germandbls");
   stdCodeArray[0xe0] = RTFStdCharCode("agrave");
   stdCodeArray[0xe1] = RTFStdCharCode("aacute");
   stdCodeArray[0xe2] = RTFStdCharCode("acircumflex");
   stdCodeArray[0xe3] = RTFStdCharCode("atilde");
   stdCodeArray[0xe4] = RTFStdCharCode("adieresis");
   stdCodeArray[0xe5] = RTFStdCharCode("aring");
   stdCodeArray[0xe6] = RTFStdCharCode("ae");
   stdCodeArray[0xe7] = RTFStdCharCode("ccedilla");
   stdCodeArray[0xe8] = RTFStdCharCode("egrave");
   stdCodeArray[0xe9] = RTFStdCharCode("eacute");
   stdCodeArray[0xea] = RTFStdCharCode("ecircumflex");
   stdCodeArray[0xeb] = RTFStdCharCode("edieresis");
   stdCodeArray[0xec] = RTFStdCharCode("igrave");
   stdCodeArray[0xed] = RTFStdCharCode("iacute");
   stdCodeArray[0xee] = RTFStdCharCode("icircumflex");
   stdCodeArray[0xef] = RTFStdCharCode("idieresis");
   stdCodeArray[0xf0] = RTFStdCharCode("eth");
   stdCodeArray[0xf1] = RTFStdCharCode("ntilde");
   stdCodeArray[0xf2] = RTFStdCharCode("ograve");
   stdCodeArray[0xf3] = RTFStdCharCode("oacute");
   stdCodeArray[0xf4] = RTFStdCharCode("ocircumflex");
   stdCodeArray[0xf5] = RTFStdCharCode("otilde");
   stdCodeArray[0xf6] = RTFStdCharCode("odieresis");
   stdCodeArray[0xf7] = RTFStdCharCode("divide");
   stdCodeArray[0xf8] = RTFStdCharCode("oslash");
   stdCodeArray[0xf9] = RTFStdCharCode("ugrave");
   stdCodeArray[0xfa] = RTFStdCharCode("uacute");
   stdCodeArray[0xfb] = RTFStdCharCode("ucircumflex");
   stdCodeArray[0xfc] = RTFStdCharCode("udieresis");
   stdCodeArray[0xfd] = RTFStdCharCode("yacute");
   stdCodeArray[0xfe] = RTFStdCharCode("thorn");
   stdCodeArray[0xff] = RTFStdCharCode("ydieresis");

#if 0
  /*
   * Turn off scanner's backslash escape mechanism while reading
   * charset file.  Restore it later.
   */
  TSGetScanner (&scanner);
  scanEscape = scanner.scanEscape;
  scanner.scanEscape = L"";
  TSSetScanner (&scanner);

  /* read file */

  while (fgets (buf, (int) sizeof (buf), f) != (wchar_t *) NULL)
  {
    if(buf[0] == '#')  /* skip comment lines */
      continue;
    TSScanInit (buf);
    if ((name = TSScan ()) == (wchar_t *) NULL)
      continue;  /* skip blank lines */
    if ((stdCode = RTFStdCharCode (name)) < 0)
    {
      RTFPanic (fn + L": unknown character name: " + name);
      continue;
    }
    if ((p = TSScan ()) == (wchar_t *) NULL)
    {
      RTFPanic (fn + L": malformed charset map line for character " + name);
      continue;
    }
    if (p[1] == '\0')  /* single wchar_t - use ascii value */
      value = p[0];
    else
    {
      radix = 10;
      if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
      {
        radix = 16;
        p += 2;
      }
      value = 0;
      while (*p != '\0')
        value = value * radix + RTFCharToHex(*p++);
    }
    if (value >= charSetSize)
    {
      RTFMsg (fn + L": character value " + 
              Int(value).ascii() + L" for " + 
              name + L" too high\n");
      RTFPanic (L"maximum value is " +  
                Int(charSetSize - 1).ascii());
    }
    stdCodeArray[value] = stdCode;
  }
  scanner.scanEscape = scanEscape;
  TSSetScanner (&scanner);
  fclose(f);

#endif

  switch (csId)
  {
  case rtfCSGeneral:
    haveGenCharSet = 1;
    break;
  case rtfCSSymbol:
    haveSymCharSet = 1;
    break;
  }

  return (1);
}


/*
 * Given a standard character name (a string), find its code (a number).
 * Return -1 if name is unknown.
 */

int RTFStdCharCode (const char* name)
{
  for (int i = 0; i < rtfSC_MaxChar; i++)
    if (strcmp (name, stdCharName[i]) == 0)
      return (i);
  return (-1);
}


/*
 * Given a standard character code (a number), find its name (a wstring).
 * Return NULL if code is unknown.
 */

const char* RTFStdCharName (int  code)
{
  if (code < 0 || code >= rtfSC_MaxChar)
    return ((const char*) NULL);
  return (stdCharName[code]);
}


/*
 * Given an RTF input character code, find standard character code.
 * The translator should read the appropriate charset maps when it finds a
 * charset control.  However, the file might not contain one.  In this
 * case, no map will be available.  When the first attempt is made to
 * map a character under these circumstances, RTFMapChar() assumes ANSI
 * and reads the map as necessary.
 */

int RTFMapChar (int  c)
{
  switch (curCharSet)
  {
  case rtfCSGeneral:
    if (!haveGenCharSet)
    {
      if (RTFReadCharSetMap ( rtfCSGeneral) == 0)
        RTFPanic (L"RTFMapChar: cannot read ansi-gen");
    }
    break;
  case rtfCSSymbol:
    if (!haveSymCharSet)
    {
      if (RTFReadCharSetMap (rtfCSSymbol) == 0)
        RTFPanic (L"RTFMapChar: cannot read ansi-sym");
    }
    break;
  }
  if (c < 0 || c >= charSetSize)
    return (rtfSC_nothing);
  return (curCharCode[c]);
}

/*
 * Sets/gets the current font number - i.e. the index in the font table of
 * the current font
 */

void RTFSetCurFontNum(int fn)
{
  curFontNum = fn;
}

int RTFGetCurFontNum()
{
  return curFontNum;
}

/*
 * Sets/gets the current non-ascii font number - i.e. the index in the
 * font table of the current font, where the current text is non-ascii
 */

void RTFSetNonAsciiFontNum(int fn)
{
  nonAsciiFontNum = fn;
}

int RTFGetNonAsciiFontNum()
{
  return nonAsciiFontNum;
}

/*
 * Set the current character set.  If csId is illegal, uses general charset.
 */

void
RTFSetCharSet (int  csId)
{
  switch (csId)
  {
  default:    /* use general if csId unknown */
  case rtfCSGeneral:
    curCharCode = genCharCode;
    curCharSet = csId;
    break;
  case rtfCSSymbol:
    curCharCode = symCharCode;
    curCharSet = csId;
    break;
  }
}


int
RTFGetCharSet ()
{
  return (curCharSet);
}


/* ---------------------------------------------------------------------- */

/*
 * Special destination readers.  They gobble the destination so the
 * writer doesn't have to deal with them.  That's wrong for any
 * translator that wants to process any of these itself.  In that
 * case, these readers should be overridden by installing a different
 * destination callback.
 *
 * NOTE: The last token read by each of these reader will be the
 * destination's terminating '}', which will then be the current token.
 * That '}' token is passed to RTFRouteToken() - the writer has already
 * seen the '{' that began the destination group, and may have pushed a
 * state; it also needs to know at the end of the group that a state
 * should be popped.
 *
 * It's important that rtf.h and the control token lookup table list
 * as many symbols as possible, because these destination readers
 * unfortunately make strict assumptions about the input they expect,
 * and a token of class rtfUnknown will throw them off easily.
 */

/*
 * Read { \fonttbl ... } destination.  Old font tables don't have
 * braces around each table entry; try to adjust for that.
 */
static void ReadFontTbl ()
{
  RTFFont *fp = NULL;
  wchar_t buf[rtfBufSiz], *bp;
  int old = -1;
  const wchar_t *fn = L"ReadFontTbl";

  for (;;)
  {
    (void) RTFGetToken ();
    if (RTFCheckCM (rtfGroup, rtfEndGroup))
      break;
    if (old < 0)    /* first entry - determine tbl type */
    {
      if (RTFCheckCMM (rtfControl, rtfCharAttr, rtfFontNum))
        old = 1;  /* no brace */
      else if (RTFCheckCM (rtfGroup, rtfBeginGroup))
        old = 0;  /* brace */
      else      /* can't tell! */
        RTFPanic (std::wstring(fn) + L": Cannot determine format");
    }
    if (old == 0)    /* need to find L"{" here */
    {
      if (!RTFCheckCM (rtfGroup, rtfBeginGroup))
        RTFPanic (std::wstring(fn) + L": missing \"{\"");
      (void) RTFGetToken ();  /* yes, skip to next token */
    }
    if ((fp = New (RTFFont)) == (RTFFont *) NULL)
      RTFPanic(std::wstring(fn) + L": cannot allocate font entry");

    fp->rtfNextFont = fontList;
    fontList = fp;

    fp->rtfFName = (wchar_t *) NULL;
    fp->rtfFAltName = (wchar_t *) NULL;
    fp->rtfFNum = -1;
    fp->rtfFFamily = 0;
    fp->rtfFCharSet = 0;
    fp->rtfFPitch = 0;
    fp->rtfFType = 0;
    fp->rtfFCodePage = 0;

    while (rtfClass != rtfEOF && !RTFCheckCM (rtfText, ';'))
    {
      if (rtfClass == rtfControl)
      {
        switch (rtfMajor) {
          case rtfFontFamily: {
            fp->rtfFFamily = rtfMinor;
            break;
          }
          case rtfCharAttr: {
            switch (rtfMinor)
            {
            default:
              break;  /* ignore unknown? */
            case rtfFontNum:
              fp->rtfFNum = rtfParam;
              break;
            }
            break;
          }
          case rtfFontAttr: {
            switch (rtfMinor) {
              default:
              break;  /* ignore unknown? */
              case rtfFontCharSet:
                fp->rtfFCharSet = rtfParam;
                break;
              case rtfFontPitch:
                fp->rtfFPitch = rtfParam;
                break;
              case rtfFontCodePage:
                fp->rtfFCodePage = rtfParam;
                break;
              case rtfFTypeNil:
              case rtfFTypeTrueType:
                fp->rtfFType = rtfParam;
                break;
            }
            break;
          }
          default: {
            /* ignore token but announce it */
            RTFMsg (std::wstring(fn) + L": unknown token \"" +
                    TBWSTR::string2wstring(rtfTextBuf) + L"\"\n");
          }
        }
      }
      else if (RTFCheckCM (rtfGroup, rtfBeginGroup))  /* dest */
      {
        RTFSkipGroup ();  /* ignore for now */
      }
      else if (rtfClass == rtfText)  /* font name */
      {
        bp = buf;
        while (rtfClass != rtfEOF
          && !RTFCheckCM (rtfText, ';'))
        {
          *bp++ = rtfMajor;
          (void) RTFGetToken ();
        }
        *bp = '\0';
        fp->rtfFName = RTFWStrSave (buf);
        if (fp->rtfFName == (wchar_t *) NULL)
          RTFPanic(std::wstring(fn) + L": cannot allocate font name");
        /* already have next token; don't read one */
        /* at bottom of loop */
        continue;
      }
      else
      {
        /* ignore token but announce it */
        RTFMsg (std::wstring(fn) + L": unknown token \"" +
                TBWSTR::string2wstring(rtfTextBuf) + L"\"\n");
      }
      (void) RTFGetToken ();
    }
    if (old == 0)  /* need to see L"}" here */
    {
      (void) RTFGetToken ();
      if (!RTFCheckCM (rtfGroup, rtfEndGroup))
        RTFPanic(std::wstring(fn) + L": missing \"}\"");
    }
  }
  if (fp->rtfFNum == -1)
    RTFPanic(std::wstring(fn) + L": missing font number");
/*
 * Could check other pieces of structure here, too, I suppose.
 */
  RTFRouteToken ();  /* feed L"}" back to router */
}


/*
 * The color table entries have color values of -1 if
 * the default color should be used for the entry (only
 * a semi-colon is given in the definition, no color values).
 * There will be a problem if a partial entry (1 or 2 but
 * not 3 color values) is given.  The possibility is ignored
 * here.
 */

static void
ReadColorTbl ()
{
RTFColor  *cp;
int    cnum = 0;
const wchar_t *fn = L"ReadColorTbl";

  for (;;)
  {
    (void) RTFGetToken ();
    if (RTFCheckCM (rtfGroup, rtfEndGroup))
      break;
    if ((cp = New (RTFColor)) == (RTFColor *) NULL)
      RTFPanic(std::wstring(fn) + L": cannot allocate color entry");
    cp->rtfCNum = cnum++;
    cp->rtfCRed = cp->rtfCGreen = cp->rtfCBlue = -1;
    cp->rtfNextColor = colorList;
    colorList = cp;
    while (RTFCheckCM (rtfControl, rtfColorName))
    {
      switch (rtfMinor)
      {
      case rtfRed:  cp->rtfCRed = rtfParam; break;
      case rtfGreen:  cp->rtfCGreen = rtfParam; break;
      case rtfBlue:  cp->rtfCBlue = rtfParam; break;
      }
      RTFGetToken ();
    }
    if (!RTFCheckCM (rtfText, (int) ';'))
      RTFPanic(std::wstring(fn) + L": malformed entry");
  }
  RTFRouteToken ();  /* feed L"}" back to router */
}


/*
 * The L"Normal" style definition doesn't contain any style number,
 * all others do.  Normal style is given style rtfNormalStyleNum.
 */

static void
ReadStyleSheet ()
{
  RTFStyle  *sp;
  RTFStyleElt  *sep, *sepLast;
  char buf[rtfBufSiz], *bp;
  const wchar_t *fn = L"ReadStyleSheet";

  for (;;)
  {
    (void) RTFGetToken ();
    if (RTFCheckCM (rtfGroup, rtfEndGroup))
      break;
    if ((sp = New (RTFStyle)) == (RTFStyle *) NULL)
      RTFPanic(std::wstring(fn) + L": cannot allocate stylesheet entry");
    sp->rtfSName = (char *) NULL;
//    sp->rtfSNum = -1;
    sp->rtfSNum = 0; // 20060324
    sp->rtfSType = rtfParStyle;
    sp->rtfSAdditive = 0;
    sp->rtfSBasedOn = rtfNoStyleNum;
    sp->rtfSNextPar = -1;
    sp->rtfSSEList = sepLast = (RTFStyleElt *) NULL;
    sp->rtfNextStyle = styleList;
    sp->rtfExpanding = 0;
    styleList = sp;
    if (!RTFCheckCM (rtfGroup, rtfBeginGroup))
      RTFPanic(std::wstring(fn) + L": missing \"{\"");
    for (;;)
    {
      (void) RTFGetToken ();
      if (rtfClass == rtfEOF
        || RTFCheckCM (rtfText, ';'))
        break;
      if (RTFCheckCM (rtfText, '*')) continue;
      if (rtfClass == rtfControl)
      {
        if (RTFCheckMM (rtfSpecialChar, rtfOptDest))
          continue;  /* ignore L"\*" */
        if (RTFCheckMM (rtfParAttr, rtfStyleNum))
        {
          sp->rtfSNum = rtfParam;
          sp->rtfSType = rtfParStyle;
          continue;
        }
        if (RTFCheckMM (rtfCharAttr, rtfCharStyleNum))
        {
          sp->rtfSNum = rtfParam;
          sp->rtfSType = rtfCharStyle;
          continue;
        }
        if (RTFCheckMM (rtfSectAttr, rtfSectStyleNum))
        {
          sp->rtfSNum = rtfParam;
          sp->rtfSType = rtfSectStyle;
          continue;
        }
        if (RTFCheckMM (rtfStyleAttr, rtfBasedOn))
        {
          sp->rtfSBasedOn = rtfParam;
          continue;
        }
        if (RTFCheckMM (rtfStyleAttr, rtfAdditive))
        {
          sp->rtfSAdditive = 1;
          continue;
        }
        if (RTFCheckMM (rtfStyleAttr, rtfNext))
        {
          sp->rtfSNextPar = rtfParam;
          continue;
        }
                                if ((sep = New (RTFStyleElt)) == (RTFStyleElt *) NULL)
          RTFPanic(std::wstring(fn) + L": cannot allocate style element");
        sep->rtfSEClass = rtfClass;
        sep->rtfSEMajor = rtfMajor;
        sep->rtfSEMinor = rtfMinor;
        sep->rtfSEParam = rtfParam;
        if ((sep->rtfSEText = RTFStrSave (rtfTextBuf))
                == (char *) NULL)
          RTFPanic(std::wstring(fn) + L": cannot allocate style element text");
        if (sepLast == (RTFStyleElt *) NULL)
          sp->rtfSSEList = sep;  /* first element */
        else        /* add to end */
          sepLast->rtfNextSE = sep;
        sep->rtfNextSE = (RTFStyleElt *) NULL;
        sepLast = sep;
      }
      else if (RTFCheckCM (rtfGroup, rtfBeginGroup))
      {
        /*
         * This passes over "{\*\keycode ... }, among
         * other things. A temporary (perhaps) hack.
         */
        RTFSkipGroup ();
        continue;
      }
      else if (rtfClass == rtfText)  /* style name */
      {
        bp = buf;
        while (rtfClass == rtfText)
        {
          if (rtfMajor == ';')
          {
            /* put back for L"for" loop */
            (void) RTFUngetToken ();
            break;
          }
          *bp++ = rtfMajor;
          (void) RTFGetToken ();
        }
        *bp = '\0';
        if ((sp->rtfSName = RTFStrSave (buf)) == (char *) NULL)
          RTFPanic(std::wstring(fn) + L": cannot allocate style name");
      }
      else    /* unrecognized */
      {
        /* ignore token but announce it */
        // stay silent LTO - RTFMsg (L"%s: unknown token \"%s\"\n",  fn, rtfTextBuf);
      }
    }
    (void) RTFGetToken ();
    if (!RTFCheckCM (rtfGroup, rtfEndGroup))
      RTFPanic(std::wstring(fn) + L": missing \"}\"");

    /*
     * Check over the style structure.  A name is a must.
     * If no style number was specified, check whether it's the
     * Normal style (in which case it's given style number
     * rtfNormalStyleNum).  Note that some L"normal" style names
     * just begin with L"Normal" and can have other stuff following,
     * e.g., L"Normal,Times 10 point".  Ugh.
     *
     * Some German RTF writers use L"Standard" instead of "Normal".
     */
    if (sp->rtfSName == (char *) NULL)
      RTFPanic(std::wstring(fn) + L": missing style name");
    if (sp->rtfSNum < 0)
    {
      if (strncmp (buf, "Normal", 6) != 0
          && strncmp (buf, "Standard", 8) != 0)
        RTFPanic(std::wstring(fn) + L": missing style number");
      sp->rtfSNum = rtfNormalStyleNum;
    }
    if (sp->rtfSNextPar == -1)  /* if \snext not given, */
      sp->rtfSNextPar = sp->rtfSNum;  /* next is itself */
  }
  RTFRouteToken ();  /* feed L"}" back to router */
}


static void ReadInfoGroup ()
{
  RTFSkipGroup ();
  RTFRouteToken ();  /* feed L"}" back to router */
}


static void ReadPictGroup ()
{
  RTFSkipGroup ();
  RTFRouteToken ();  /* feed L"}" back to router */
}


static void ReadObjGroup ()
{
  RTFSkipGroup ();
  RTFRouteToken ();  /* feed L"}" back to router */
}


/* ---------------------------------------------------------------------- */

/*
 * Routines to return pieces of stylesheet, or font or color tables.
 * References to style 0 are mapped onto the Normal style.
 */

RTFStyle * RTFGetStyle (int  num)
{
  RTFStyle  *s;

  if (num == -1)
    return (styleList);
  for (s = styleList; s != (RTFStyle *) NULL; s = s->rtfNextStyle)
  {
    if (s->rtfSNum == num)
      break;
  }
  return (s);    /* NULL if not found */
}

// get the style number for a given style
int RTFGetStyleNumber(const char * name)
{
  RTFStyle  *s;
  
  for (s = styleList; s != (RTFStyle *) NULL; s = s->rtfNextStyle)
    {
      if (!strcmp(s->rtfSName, name)) return (s->rtfSNum);
    }
  return (-1);    /* -1 */
}

int RTFGetFontNumByCharset (const std::wstring & charset)
{
  int cs = 0;
//  if( charset == L"SJIS" ) cs = 128;
#ifdef _MSC_VER
  if( charset == L"932" ) cs = 128;
#else
  if( charset == L"ja_JP.SJIS" ) cs = 128;
#endif // _MSC_VER

  if( cs != 0 )
    return RTFGetFontNumByCharset(cs);
  else
    return -1;
}

int RTFGetFontNumByCharset (int cs)
{
  RTFFont  *f;

  for (f = fontList; f != (RTFFont *) NULL; f = f->rtfNextFont)
  {
    if ( (f->rtfFCharSet == cs) && ( f->rtfFName[0] != L'@' ) )
    {
      return f->rtfFNum;
    }
  }
  return -1;
}

RTFFont * RTFGetFont (int num)
{
  RTFFont  *f;

  if (num == -1)
    return (fontList);
  for (f = fontList; f != (RTFFont *) NULL; f = f->rtfNextFont)
  {
    if (f->rtfFNum == num)
      break;
  }
  return (f);    /* NULL if not found */
}

RTFColor * RTFGetColor (int num)
{
  RTFColor  *c;

  if (num == -1)
    return (colorList);
  for (c = colorList; c != (RTFColor *) NULL; c = c->rtfNextColor)
  {
    if (c->rtfCNum == num)
      break;
  }
  return (c);    /* NULL if not found */
}

/* ---------------------------------------------------------------------- */

/*
 * Expand style n, if there is such a style.
 */

void RTFExpandStyle (int n)
{
  RTFStyle  *s;
  RTFStyleElt  *se;

  if (n == -1 || (s = RTFGetStyle (n)) == (RTFStyle *) NULL)
    return;
  if (s->rtfExpanding != 0)
    RTFPanic (L"Style expansion loop, style " + Int(n).ascii());
  s->rtfExpanding = 1;  /* set expansion flag for loop detection */
  /*
   * Expand L"based-on" style (unless it's the same as the current
   * style -- Normal style usually gives itself as its own based-on
   * style).  Based-on style expansion is done by synthesizing
   * the token that the writer needs to see in order to trigger
   * another style expansion, and feeding to token back through
   * the router so the writer sees it.
   */
  if (n != s->rtfSBasedOn)
  {
    RTFSetToken (rtfControl, rtfParAttr, rtfStyleNum,
              s->rtfSBasedOn, "\\s");
    RTFRouteToken ();
  }
  /*
   * Now route the tokens unique to this style.  RTFSetToken()
   * isn't used because it would add the param value to the end
   * of the token text, which already has it in.
   */
  for (se = s->rtfSSEList; se != (RTFStyleElt *) NULL; se = se->rtfNextSE)
  {
    rtfClass = se->rtfSEClass;
    rtfMajor = se->rtfSEMajor;
    rtfMinor = se->rtfSEMinor;
    rtfParam = se->rtfSEParam;
    //(void) strcpy (rtfTextBuf, se->rtfSEText);
    size_t len_rtfSEText = strlen(se->rtfSEText) + 1;
    memcpy (rtfTextBuf, se->rtfSEText, len_rtfSEText);
    rtfTextLen = strlen (rtfTextBuf);
    RTFRouteToken ();
  }
  s->rtfExpanding = 0;  /* done - clear expansion flag */
}

/* ---------------------------------------------------------------------- */

/*
 * Control symbol lookup routines
 */

typedef struct RTFKey  RTFKey;

struct RTFKey
{
  int rtfKMajor;  /* major number */
  int rtfKMinor;  /* minor number */
  const char *rtfKStr;  /* symbol name */
  int rtfKHash;   /* symbol name hash value */
};

/*
 * A minor number of -1 means the token has no minor number
 * (all valid minor numbers are >= 0).
 */

static RTFKey rtfKey[] =
{
  /*
   * Special characters
   */
  {rtfSpecialChar,  rtfIIntVersion,  "vern",  0},
  {rtfSpecialChar,  rtfICreateTime,  "creatim",  0},
  {rtfSpecialChar,  rtfIRevisionTime,  "revtim",  0},
  {rtfSpecialChar,  rtfIPrintTime,  "printim",  0},
  {rtfSpecialChar,  rtfIBackupTime,  "buptim",  0},
  {rtfSpecialChar,  rtfIEditTime,  "edmins",  0},
  {rtfSpecialChar,  rtfIYear,  "yr",  0},
  {rtfSpecialChar,  rtfIMonth,  "mo",  0},
  {rtfSpecialChar,  rtfIDay,  "dy",  0},
  {rtfSpecialChar,  rtfIHour,  "hr",  0},
  {rtfSpecialChar,  rtfIMinute,  "min",  0},
  {rtfSpecialChar,  rtfISecond,  "sec",  0},
  {rtfSpecialChar,  rtfINPages,  "nofpages",  0},
  {rtfSpecialChar,  rtfINWords,  "nofwords",  0},
  {rtfSpecialChar,  rtfINChars,  "nofchars",  0},
  {rtfSpecialChar,  rtfIIntID,  "id",  0},

  {rtfSpecialChar,  rtfCurHeadDate,  "chdate",  0},
  {rtfSpecialChar,  rtfCurHeadDateLong,  "chdpl",  0},
  {rtfSpecialChar,  rtfCurHeadDateAbbrev,  "chdpa",  0},
  {rtfSpecialChar,  rtfCurHeadTime,  "chtime",  0},
  {rtfSpecialChar,  rtfCurHeadPage,  "chpgn",  0},
  {rtfSpecialChar,  rtfSectNum,  "sectnum",  0},
  {rtfSpecialChar,  rtfCurFNote,  "chftn",  0},
  {rtfSpecialChar,  rtfCurAnnotRef,  "chatn",  0},
  {rtfSpecialChar,  rtfFNoteSep,  "chftnsep",  0},
  {rtfSpecialChar,  rtfFNoteCont,  "chftnsepc",  0},
  {rtfSpecialChar,  rtfCell,  "cell",  0},
  {rtfSpecialChar,  rtfRow,  "row",  0},
  {rtfSpecialChar,  rtfPar,  "par",  0},  /* newline and carriage return are synonyms for */
  /* \par when they are preceded by a \ character */
  {rtfSpecialChar,  rtfPar,      "\n",    0},
  {rtfSpecialChar,  rtfPar,      "\r",    0},

  {rtfSpecialChar,  rtfSect,  "sect",  0},
  {rtfSpecialChar,  rtfPage,  "page",  0},
  {rtfSpecialChar,  rtfColumn,  "column",  0},
  {rtfSpecialChar,  rtfLine,  "line",  0},
  {rtfSpecialChar,  rtfSoftPage,  "softpage",  0},
  {rtfSpecialChar,  rtfSoftColumn,  "softcol",  0},
  {rtfSpecialChar,  rtfSoftLine,  "softline",  0},
  {rtfSpecialChar,  rtfSoftLineHt,  "softlheight",  0},
  {rtfSpecialChar,  rtfTab,  "tab",  0},
  {rtfSpecialChar,  rtfEmDash,  "emdash",  0},
  {rtfSpecialChar,  rtfEnDash,  "endash",  0},
  {rtfSpecialChar,  rtfEmSpace,  "emspace",  0},
  {rtfSpecialChar,  rtfEnSpace,  "enspace",  0},
  {rtfSpecialChar,  rtfBullet,  "bullet",  0},
  {rtfSpecialChar,  rtfLQuote,  "lquote",  0},
  {rtfSpecialChar,  rtfRQuote,  "rquote",  0},
  {rtfSpecialChar,  rtfLDblQuote,  "ldblquote",  0},
  {rtfSpecialChar,  rtfRDblQuote,  "rdblquote",  0},  
  {rtfSpecialChar,  rtfFormula,    "|",    0},
  {rtfSpecialChar,  rtfNoBrkSpace,    "~",    0},
  {rtfSpecialChar,  rtfNoReqHyphen,    "-",    0},

  {rtfSpecialChar,  rtfNoBrkHyphen,  "_",  0},
  {rtfSpecialChar,  rtfOptDest,    "*",    0},

  {rtfSpecialChar,  rtfLTRMark,  "ltrmark",  0},
  {rtfSpecialChar,  rtfRTLMark,  "rtlmark",  0},
  {rtfSpecialChar,  rtfNoWidthJoiner,  "zwj",  0},
  {rtfSpecialChar,  rtfNoWidthNonJoiner,  "zwnj",  0},  /* is this valid? */

  {rtfSpecialChar,  rtfCurHeadPict,  "chpict",  0},

  /*
   * Character formatting attributes
   */
  {rtfCharAttr,  rtfPlain,  "plain",  0},
  {rtfCharAttr,  rtfBold,  "b",  0},
  {rtfCharAttr,  rtfAllCaps,  "caps",  0},
  {rtfCharAttr,  rtfDeleted,  "deleted",  0},
  {rtfCharAttr,  rtfSubScript,  "dn",  0},
  {rtfCharAttr,  rtfSubScrShrink,  "sub",  0},
  {rtfCharAttr,  rtfNoSuperSub,  "nosupersub",  0},
  {rtfCharAttr,  rtfExpand,  "expnd",  0},
  {rtfCharAttr,  rtfExpandTwips,  "expndtw",  0},
  {rtfCharAttr,  rtfKerning,  "kerning",  0},
  {rtfCharAttr,  rtfFontNum,  "f",  0},
  {rtfCharAttr,  rtfFontSize,  "fs",  0},
  {rtfCharAttr,  rtfItalic,  "i",  0},
  {rtfCharAttr,  rtfOutline,  "outl",  0},
  {rtfCharAttr,  rtfRevised,  "revised",  0},
  {rtfCharAttr,  rtfRevAuthor,  "revauth",  0},
  {rtfCharAttr,  rtfRevDTTM,  "revdttm",  0},
  {rtfCharAttr,  rtfSmallCaps,  "scaps",  0},
  {rtfCharAttr,  rtfShadow,  "shad",  0},
  {rtfCharAttr,  rtfStrikeThru,  "strike",  0},
  {rtfCharAttr,  rtfUnderline,  "ul",  0},
  {rtfCharAttr,  rtfDotUnderline,  "uld",  0},
  {rtfCharAttr,  rtfDbUnderline,  "uldb",  0},
  {rtfCharAttr,  rtfNoUnderline,  "ulnone",  0},
  {rtfCharAttr,  rtfWordUnderline,  "ulw",  0},
  {rtfCharAttr,  rtfSuperScript,  "up",  0},
  {rtfCharAttr,  rtfSuperScrShrink,  "super",  0},
  {rtfCharAttr,  rtfInvisible,  "v",  0},
  {rtfCharAttr,  rtfForeColor,  "cf",  0},
  {rtfCharAttr,  rtfBackColor,  "cb",  0},
  {rtfCharAttr,  rtfRTLChar,  "rtlch",  0},
  {rtfCharAttr,  rtfLTRChar,  "ltrch",  0},
  {rtfCharAttr,  rtfCharStyleNum,  "cs",  0},
  {rtfCharAttr,  rtfCharCharSet,  "cchs",  0},
  {rtfCharAttr,  rtfLanguage,  "lang",  0},  /* this has disappeared from spec 1.2 */

  {rtfCharAttr,  rtfGray,  "gray",  0},

  /*
   * Paragraph formatting attributes
   */
  {rtfParAttr,  rtfParDef,  "pard",  0},
  {rtfParAttr,  rtfStyleNum,  "s",  0},
  {rtfParAttr,  rtfHyphenate,  "hyphpar",  0},
  {rtfParAttr,  rtfInTable,  "intbl",  0},
  {rtfParAttr,  rtfKeep,  "keep",  0},
  {rtfParAttr,  rtfNoWidowControl,  "nowidctlpar",  0},
  {rtfParAttr,  rtfKeepNext,  "keepn",  0},
  {rtfParAttr,  rtfOutlineLevel,  "level",  0},
  {rtfParAttr,  rtfNoLineNum,  "noline",  0},
  {rtfParAttr,  rtfPBBefore,  "pagebb",  0},
  {rtfParAttr,  rtfSideBySide,  "sbys",  0},
  {rtfParAttr,  rtfQuadLeft,  "ql",  0},
  {rtfParAttr,  rtfQuadRight,  "qr",  0},
  {rtfParAttr,  rtfQuadJust,  "qj",  0},
  {rtfParAttr,  rtfQuadCenter,  "qc",  0},
  {rtfParAttr,  rtfFirstIndent,  "fi",  0},
  {rtfParAttr,  rtfLeftIndent,  "li",  0},
  {rtfParAttr,  rtfRightIndent,  "ri",  0},
  {rtfParAttr,  rtfSpaceBefore,  "sb",  0},
  {rtfParAttr,  rtfSpaceAfter,  "sa",  0},
  {rtfParAttr,  rtfSpaceBetween,  "sl",  0},
  {rtfParAttr,  rtfSpaceMultiply,  "slmult",  0},

  {rtfParAttr,  rtfSubDocument,  "subdocument",  0},

  {rtfParAttr,  rtfRTLPar,  "rtlpar",  0},
  {rtfParAttr,  rtfLTRPar,  "ltrpar",  0},

  {rtfParAttr,  rtfTabPos,  "tx",  0},

  /*
   * FrameMaker writes \tql (to mean left-justified tab, apparently)
   * although it's not in the spec.  It's also redundant, since lj
   * tabs are the default.1G
   */

  {rtfParAttr,  rtfTabLeft,  "tql",  0},
  {rtfParAttr,  rtfTabRight,  "tqr",  0},
  {rtfParAttr,  rtfTabCenter,  "tqc",  0},
  {rtfParAttr,  rtfTabDecimal,  "tqdec",  0},
  {rtfParAttr,  rtfTabBar,  "tb",  0},
  {rtfParAttr,  rtfLeaderDot,  "tldot",  0},
  {rtfParAttr,  rtfLeaderHyphen,  "tlhyph",  0},
  {rtfParAttr,  rtfLeaderUnder,  "tlul",  0},
  {rtfParAttr,  rtfLeaderThick,  "tlth",  0},
  {rtfParAttr,  rtfLeaderEqual,  "tleq",  0},

  {rtfParAttr,  rtfParLevel,  "pnlvl",  0},
  {rtfParAttr,  rtfParBullet,  "pnlvlblt",  0},
  {rtfParAttr,  rtfParSimple,  "pnlvlbody",  0},
  {rtfParAttr,  rtfParNumCont,  "pnlvlcont",  0},
  {rtfParAttr,  rtfParNumOnce,  "pnnumonce",  0},
  {rtfParAttr,  rtfParNumAcross,  "pnacross",  0},
  {rtfParAttr,  rtfParHangIndent,  "pnhang",  0},
  {rtfParAttr,  rtfParNumRestart,  "pnrestart",  0},
  {rtfParAttr,  rtfParNumCardinal,  "pncard",  0},
  {rtfParAttr,  rtfParNumDecimal,  "pndec",  0},
  {rtfParAttr,  rtfParNumULetter,  "pnucltr",  0},
  {rtfParAttr,  rtfParNumURoman,  "pnucrm",  0},
  {rtfParAttr,  rtfParNumLLetter,  "pnlcltr",  0},
  {rtfParAttr,  rtfParNumLRoman,  "pnlcrm",  0},
  {rtfParAttr,  rtfParNumOrdinal,  "pnord",  0},
  {rtfParAttr,  rtfParNumOrdinalText,  "pnordt",  0},
  {rtfParAttr,  rtfParNumBold,  "pnb",  0},
  {rtfParAttr,  rtfParNumItalic,  "pni",  0},
  {rtfParAttr,  rtfParNumAllCaps,  "pncaps",  0},
  {rtfParAttr,  rtfParNumSmallCaps,  "pnscaps",  0},
  {rtfParAttr,  rtfParNumUnder,  "pnul",  0},
  {rtfParAttr,  rtfParNumDotUnder,  "pnuld",  0},
  {rtfParAttr,  rtfParNumDbUnder,  "pnuldb",  0},
  {rtfParAttr,  rtfParNumNoUnder,  "pnulnone",  0},
  {rtfParAttr,  rtfParNumWordUnder,  "pnulw",  0},
  {rtfParAttr,  rtfParNumStrikethru,  "pnstrike",  0},
  {rtfParAttr,  rtfParNumForeColor,  "pncf",  0},
  {rtfParAttr,  rtfParNumFont,  "pnf",  0},
  {rtfParAttr,  rtfParNumFontSize,  "pnfs",  0},
  {rtfParAttr,  rtfParNumIndent,  "pnindent",  0},
  {rtfParAttr,  rtfParNumSpacing,  "pnsp",  0},
  {rtfParAttr,  rtfParNumInclPrev,  "pnprev",  0},
  {rtfParAttr,  rtfParNumCenter,  "pnqc",  0},
  {rtfParAttr,  rtfParNumLeft,  "pnql",  0},
  {rtfParAttr,  rtfParNumRight,  "pnqr",  0},
  {rtfParAttr,  rtfParNumStartAt,  "pnstart",  0},

  {rtfParAttr,  rtfBorderTop,  "brdrt",  0},
  {rtfParAttr,  rtfBorderBottom,  "brdrb",  0},
  {rtfParAttr,  rtfBorderLeft,  "brdrl",  0},
  {rtfParAttr,  rtfBorderRight,  "brdrr",  0},
  {rtfParAttr,  rtfBorderBetween,  "brdrbtw",  0},
  {rtfParAttr,  rtfBorderBar,  "brdrbar",  0},
  {rtfParAttr,  rtfBorderBox,  "box",  0},
  {rtfParAttr,  rtfBorderSingle,  "brdrs",  0},
  {rtfParAttr,  rtfBorderThick,  "brdrth",  0},
  {rtfParAttr,  rtfBorderShadow,  "brdrsh",  0},
  {rtfParAttr,  rtfBorderDouble,  "brdrdb",  0},
  {rtfParAttr,  rtfBorderDot,  "brdrdot",  0},
  {rtfParAttr,  rtfBorderDot,  "brdrdash",  0},
  {rtfParAttr,  rtfBorderHair,  "brdrhair",  0},
  {rtfParAttr,  rtfBorderWidth,  "brdrw",  0},
  {rtfParAttr,  rtfBorderColor,  "brdrcf",  0},
  {rtfParAttr,  rtfBorderSpace,  "brsp",  0},

  {rtfParAttr,  rtfShading,  "shading",  0},
  {rtfParAttr,  rtfBgPatH,  "bghoriz",  0},
  {rtfParAttr,  rtfBgPatV,  "bgvert",  0},
  {rtfParAttr,  rtfFwdDiagBgPat,  "bgfdiag",  0},
  {rtfParAttr,  rtfBwdDiagBgPat,  "bgbdiag",  0},
  {rtfParAttr,  rtfHatchBgPat,  "bgcross",  0},
  {rtfParAttr,  rtfDiagHatchBgPat,  "bgdcross",  0},
  {rtfParAttr,  rtfDarkBgPatH,  "bgdkhoriz",  0},
  {rtfParAttr,  rtfDarkBgPatV,  "bgdkvert",  0},
  {rtfParAttr,  rtfFwdDarkBgPat,  "bgdkfdiag",  0},
  {rtfParAttr,  rtfBwdDarkBgPat,  "bgdkbdiag",  0},
  {rtfParAttr,  rtfDarkHatchBgPat,  "bgdkcross",  0},
  {rtfParAttr,  rtfDarkDiagHatchBgPat,  "bgdkdcross",  0},
  {rtfParAttr,  rtfBgPatLineColor,  "cfpat",  0},
  {rtfParAttr,  rtfBgPatColor,  "cbpat",  0},

  /*
   * Section formatting attributes
   */
  {rtfSectAttr,  rtfSectDef,  "sectd",  0},
  {rtfSectAttr,  rtfENoteHere,  "endnhere",  0},
  {rtfSectAttr,  rtfPrtBinFirst,  "binfsxn",  0},
  {rtfSectAttr,  rtfPrtBin,  "binsxn",  0},
  {rtfSectAttr,  rtfSectStyleNum,  "ds",  0},

  {rtfSectAttr,  rtfNoBreak,  "sbknone",  0},
  {rtfSectAttr,  rtfColBreak,  "sbkcol",  0},
  {rtfSectAttr,  rtfPageBreak,  "sbkpage",  0},
  {rtfSectAttr,  rtfEvenBreak,  "sbkeven",  0},
  {rtfSectAttr,  rtfOddBreak,  "sbkodd",  0},

  {rtfSectAttr,  rtfColumns,  "cols",  0},
  {rtfSectAttr,  rtfColumnSpace,  "colsx",  0},
  {rtfSectAttr,  rtfColumnNumber,  "colno",  0},
  {rtfSectAttr,  rtfColumnSpRight,  "colsr",  0},
  {rtfSectAttr,  rtfColumnWidth,  "colw",  0},
  {rtfSectAttr,  rtfColumnLine,  "linebetcol",  0},

  {rtfSectAttr,  rtfLineModulus,  "linemod",  0},
  {rtfSectAttr,  rtfLineDist,  "linex",  0},
  {rtfSectAttr,  rtfLineStarts,  "linestarts",  0},
  {rtfSectAttr,  rtfLineRestart,  "linerestart",  0},
  {rtfSectAttr,  rtfLineRestartPg,  "lineppage",  0},
  {rtfSectAttr,  rtfLineCont,  "linecont",  0},

  {rtfSectAttr,  rtfSectPageWid,  "pgwsxn",  0},
  {rtfSectAttr,  rtfSectPageHt,  "pghsxn",  0},
  {rtfSectAttr,  rtfSectMarginLeft,  "marglsxn",  0},
  {rtfSectAttr,  rtfSectMarginRight,  "margrsxn",  0},
  {rtfSectAttr,  rtfSectMarginTop,  "margtsxn",  0},
  {rtfSectAttr,  rtfSectMarginBottom,  "margbsxn",  0},
  {rtfSectAttr,  rtfSectMarginGutter,  "guttersxn",  0},
  {rtfSectAttr,  rtfSectLandscape,  "lndscpsxn",  0},
  {rtfSectAttr,  rtfTitleSpecial,  "titlepg",  0},
  {rtfSectAttr,  rtfHeaderY,  "headery",  0},
  {rtfSectAttr,  rtfFooterY,  "footery",  0},

  {rtfSectAttr,  rtfPageStarts,  "pgnstarts",  0},
  {rtfSectAttr,  rtfPageCont,  "pgncont",  0},
  {rtfSectAttr,  rtfPageRestart,  "pgnrestart",  0},
  {rtfSectAttr,  rtfPageNumRight,  "pgnx",  0},
  {rtfSectAttr,  rtfPageNumTop,  "pgny",  0},
  {rtfSectAttr,  rtfPageDecimal,  "pgndec",  0},
  {rtfSectAttr,  rtfPageURoman,  "pgnucrm",  0},
  {rtfSectAttr,  rtfPageLRoman,  "pgnlcrm",  0},
  {rtfSectAttr,  rtfPageULetter,  "pgnucltr",  0},
  {rtfSectAttr,  rtfPageLLetter,  "pgnlcltr",  0},
  {rtfSectAttr,  rtfPageNumHyphSep,  "pgnhnsh",  0},
  {rtfSectAttr,  rtfPageNumSpaceSep,  "pgnhnsp",  0},
  {rtfSectAttr,  rtfPageNumColonSep,  "pgnhnsc",  0},
  {rtfSectAttr,  rtfPageNumEmdashSep,  "pgnhnsm",  0},
  {rtfSectAttr,  rtfPageNumEndashSep,  "pgnhnsn",  0},

  {rtfSectAttr,  rtfTopVAlign,  "vertalt",  0},  /* misspelled as "vertal" in specification 1.0 */

  {rtfSectAttr,  rtfBottomVAlign,  "vertalb",  0},
  {rtfSectAttr,  rtfCenterVAlign,  "vertalc",  0},
  {rtfSectAttr,  rtfJustVAlign,  "vertalj",  0},

  {rtfSectAttr,  rtfRTLSect,  "rtlsect",  0},
  {rtfSectAttr,  rtfLTRSect,  "ltrsect",  0},
  /* I've seen these in an old spec, but not in real files... */

  {rtfSectAttr,  rtfNoBreak,  "nobreak",  0},
  {rtfSectAttr,  rtfColBreak,  "colbreak",  0},
  {rtfSectAttr,  rtfPageBreak,  "pagebreak",  0},
  {rtfSectAttr,  rtfEvenBreak,  "evenbreak",  0},
  {rtfSectAttr,  rtfOddBreak,  "oddbreak",  0},

  /*
   * Document formatting attributes
   */
  {rtfDocAttr,  rtfDefTab,  "deftab",  0},
  {rtfDocAttr,  rtfHyphHotZone,  "hyphhotz",  0},
  {rtfDocAttr,  rtfHyphConsecLines,  "hyphconsec",  0},
  {rtfDocAttr,  rtfHyphCaps,  "hyphcaps",  0},
  {rtfDocAttr,  rtfHyphAuto,  "hyphauto",  0},
  {rtfDocAttr,  rtfLineStart,  "linestart",  0},
  {rtfDocAttr,  rtfFracWidth,  "fracwidth",  0},  /* \makeback was given in old version of spec, it's now */
  /* listed as \makebackup */

  {rtfDocAttr,  rtfMakeBackup,  "makeback",  0},
  {rtfDocAttr,  rtfMakeBackup,  "makebackup",  0},
  {rtfDocAttr,  rtfRTFDefault,  "defformat",  0},
  {rtfDocAttr,  rtfPSOverlay,  "psover",  0},
  {rtfDocAttr,  rtfDocTemplate,  "doctemp",  0},
  {rtfDocAttr,  rtfDefLanguage,  "deflang",  0},
  {rtfDocAttr,  rtfDefLanguageFE,  "deflangfe",  0},

  {rtfDocAttr,  rtfFENoteType,  "fet",  0},
  {rtfDocAttr,  rtfFNoteEndSect,  "endnotes",  0},
  {rtfDocAttr,  rtfFNoteEndDoc,  "enddoc",  0},
  {rtfDocAttr,  rtfFNoteText,  "ftntj",  0},
  {rtfDocAttr,  rtfFNoteBottom,  "ftnbj",  0},
  {rtfDocAttr,  rtfENoteEndSect,  "aendnotes",  0},
  {rtfDocAttr,  rtfENoteEndDoc,  "aenddoc",  0},
  {rtfDocAttr,  rtfENoteText,  "aftntj",  0},
  {rtfDocAttr,  rtfENoteBottom,  "aftnbj",  0},
  {rtfDocAttr,  rtfFNoteStart,  "ftnstart",  0},
  {rtfDocAttr,  rtfENoteStart,  "aftnstart",  0},
  {rtfDocAttr,  rtfFNoteRestartPage,  "ftnrstpg",  0},
  {rtfDocAttr,  rtfFNoteRestart,  "ftnrestart",  0},
  {rtfDocAttr,  rtfFNoteRestartCont,  "ftnrstcont",  0},
  {rtfDocAttr,  rtfENoteRestart,  "aftnrestart",  0},
  {rtfDocAttr,  rtfENoteRestartCont,  "aftnrstcont",  0},
  {rtfDocAttr,  rtfFNoteNumArabic,  "ftnnar",  0},
  {rtfDocAttr,  rtfFNoteNumLLetter,  "ftnnalc",  0},
  {rtfDocAttr,  rtfFNoteNumULetter,  "ftnnauc",  0},
  {rtfDocAttr,  rtfFNoteNumLRoman,  "ftnnrlc",  0},
  {rtfDocAttr,  rtfFNoteNumURoman,  "ftnnruc",  0},
  {rtfDocAttr,  rtfFNoteNumChicago,  "ftnnchi",  0},
  {rtfDocAttr,  rtfENoteNumArabic,  "aftnnar",  0},
  {rtfDocAttr,  rtfENoteNumLLetter,  "aftnnalc",  0},
  {rtfDocAttr,  rtfENoteNumULetter,  "aftnnauc",  0},
  {rtfDocAttr,  rtfENoteNumLRoman,  "aftnnrlc",  0},
  {rtfDocAttr,  rtfENoteNumURoman,  "aftnnruc",  0},
  {rtfDocAttr,  rtfENoteNumChicago,  "aftnnchi",  0},

  {rtfDocAttr,  rtfPaperWidth,  "paperw",  0},
  {rtfDocAttr,  rtfPaperHeight,  "paperh",  0},
  {rtfDocAttr,  rtfPaperSize,  "psz",  0},
  {rtfDocAttr,  rtfLeftMargin,  "margl",  0},
  {rtfDocAttr,  rtfRightMargin,  "margr",  0},
  {rtfDocAttr,  rtfTopMargin,  "margt",  0},
  {rtfDocAttr,  rtfBottomMargin,  "margb",  0},
  {rtfDocAttr,  rtfFacingPage,  "facingp",  0},
  {rtfDocAttr,  rtfGutterWid,  "gutter",  0},
  {rtfDocAttr,  rtfMirrorMargin,  "margmirror",  0},
  {rtfDocAttr,  rtfLandscape,  "landscape",  0},
  {rtfDocAttr,  rtfPageStart,  "pgnstart",  0},
  {rtfDocAttr,  rtfWidowCtrl,  "widowctrl",  0},

  {rtfDocAttr,  rtfLinkStyles,  "linkstyles",  0},

  {rtfDocAttr,  rtfNoAutoTabIndent,  "notabind",  0},
  {rtfDocAttr,  rtfWrapSpaces,  "wraptrsp",  0},
  {rtfDocAttr,  rtfPrintColorsBlack,  "prcolbl",  0},
  {rtfDocAttr,  rtfNoExtraSpaceRL,  "noextrasprl",  0},
  {rtfDocAttr,  rtfNoColumnBalance,  "nocolbal",  0},
  {rtfDocAttr,  rtfCvtMailMergeQuote,  "cvmme",  0},
  {rtfDocAttr,  rtfSuppressTopSpace,  "sprstsp",  0},
  {rtfDocAttr,  rtfSuppressPreParSpace,  "sprsspbf",  0},
  {rtfDocAttr,  rtfCombineTblBorders,  "otblrul",  0},
  {rtfDocAttr,  rtfTranspMetafiles,  "transmf",  0},
  {rtfDocAttr,  rtfSwapBorders,  "swpbdr",  0},
  {rtfDocAttr,  rtfShowHardBreaks,  "brkfrm",  0},

  {rtfDocAttr,  rtfFormProtected,  "formprot",  0},
  {rtfDocAttr,  rtfAllProtected,  "allprot",  0},
  {rtfDocAttr,  rtfFormShading,  "formshade",  0},
  {rtfDocAttr,  rtfFormDisplay,  "formdisp",  0},
  {rtfDocAttr,  rtfPrintData,  "printdata",  0},

  {rtfDocAttr,  rtfRevProtected,  "revprot",  0},
  {rtfDocAttr,  rtfRevisions,  "revisions",  0},
  {rtfDocAttr,  rtfRevDisplay,  "revprop",  0},
  {rtfDocAttr,  rtfRevBar,  "revbar",  0},

  {rtfDocAttr,  rtfAnnotProtected,  "annotprot",  0},

  {rtfDocAttr,  rtfRTLDoc,  "rtldoc",  0},
  {rtfDocAttr,  rtfLTRDoc,  "ltrdoc",  0},

  /*
   * Style attributes
   */
  {rtfStyleAttr,  rtfAdditive,  "additive",  0},
  {rtfStyleAttr,  rtfBasedOn,  "sbasedon",  0},
  {rtfStyleAttr,  rtfNext,  "snext",  0},

  /*
   * Picture attributes
   */
  {rtfPictAttr,  rtfMacQD,  "macpict",  0},
  {rtfPictAttr,  rtfPMMetafile,  "pmmetafile",  0},
  {rtfPictAttr,  rtfWinMetafile,  "wmetafile",  0},
  {rtfPictAttr,  rtfDevIndBitmap,  "dibitmap",  0},
  {rtfPictAttr,  rtfWinBitmap,  "wbitmap",  0},
  {rtfPictAttr,  rtfPixelBits,  "wbmbitspixel",  0},
  {rtfPictAttr,  rtfBitmapPlanes,  "wbmplanes",  0},
  {rtfPictAttr,  rtfBitmapWid,  "wbmwidthbytes",  0},

  {rtfPictAttr,  rtfPicWid,  "picw",  0},
  {rtfPictAttr,  rtfPicHt,  "pich",  0},
  {rtfPictAttr,  rtfPicGoalWid,  "picwgoal",  0},
  {rtfPictAttr,  rtfPicGoalHt,  "pichgoal",  0},  /* these two aren't in the spec, but some writers emit them */

  {rtfPictAttr,  rtfPicGoalWid,  "picwGoal",  0},
  {rtfPictAttr,  rtfPicGoalHt,  "pichGoal",  0},
  {rtfPictAttr,  rtfPicScaleX,  "picscalex",  0},
  {rtfPictAttr,  rtfPicScaleY,  "picscaley",  0},
  {rtfPictAttr,  rtfPicScaled,  "picscaled",  0},
  {rtfPictAttr,  rtfPicCropTop,  "piccropt",  0},
  {rtfPictAttr,  rtfPicCropBottom,  "piccropb",  0},
  {rtfPictAttr,  rtfPicCropLeft,  "piccropl",  0},
  {rtfPictAttr,  rtfPicCropRight,  "piccropr",  0},

  {rtfPictAttr,  rtfPicMFHasBitmap,  "picbmp",  0},
  {rtfPictAttr,  rtfPicMFBitsPerPixel,  "picbpp",  0},

  {rtfPictAttr,  rtfPicBinary,  "bin",  0},

  /*
   * NeXT graphic attributes
   */
  {rtfNeXTGrAttr,  rtfNeXTGWidth,  "width",  0},
  {rtfNeXTGrAttr,  rtfNeXTGHeight,  "height",  0},

  /*
   * Destinations
   */
  {rtfDestination,  rtfFontAltName,  "falt",  0},
  {rtfDestination,  rtfEmbeddedFont,  "fonteb",  0},
  {rtfDestination,  rtfFontFile,  "fontfile",  0},
  {rtfDestination,  rtfFontTbl,  "fonttbl", 0},
  {rtfDestination,  rtfFileTbl,  "filetbl",  0},
  {rtfDestination,  rtfFileInfo,  "file",  0},
  {rtfDestination,  rtfColorTbl,  "colortbl",  0},
  {rtfDestination,  rtfStyleSheet,  "stylesheet",  0},
  {rtfDestination,  rtfKeyCode,  "keycode",  0},
  {rtfDestination,  rtfRevisionTbl,  "revtbl",  0},
  {rtfDestination,  rtfInfo,  "info",  0},
  {rtfDestination,  rtfITitle,  "title",  0},
  {rtfDestination,  rtfISubject,  "subject",  0},
  {rtfDestination,  rtfIAuthor,  "author",  0},
  {rtfDestination,  rtfIOperator,  "operator",  0},
  {rtfDestination,  rtfIKeywords,  "keywords",  0},
  {rtfDestination,  rtfIComment,  "comment",  0},
  {rtfDestination,  rtfIVersion,  "version",  0},
  {rtfDestination,  rtfIDoccomm,  "doccomm",  0},  /* \verscomm may not exist -- was seen in earlier spec version */

  {rtfDestination,  rtfIVerscomm,  "verscomm",  0},
  {rtfDestination,  rtfNextFile,  "nextfile",  0},
  {rtfDestination,  rtfTemplate,  "template",  0},
  {rtfDestination,  rtfFNSep,  "ftnsep",  0},
  {rtfDestination,  rtfFNContSep,  "ftnsepc",  0},
  {rtfDestination,  rtfFNContNotice,  "ftncn",  0},
  {rtfDestination,  rtfENSep,  "aftnsep",  0},
  {rtfDestination,  rtfENContSep,  "aftnsepc",  0},
  {rtfDestination,  rtfENContNotice,  "aftncn",  0},
  {rtfDestination,  rtfPageNumLevel,  "pgnhn",  0},
  {rtfDestination,  rtfParNumLevelStyle,  "pnseclvl",  0},
  {rtfDestination,  rtfHeader,  "header",  0},
  {rtfDestination,  rtfFooter,  "footer",  0},
  {rtfDestination,  rtfHeaderLeft,  "headerl",  0},
  {rtfDestination,  rtfHeaderRight,  "headerr",  0},
  {rtfDestination,  rtfHeaderFirst,  "headerf",  0},
  {rtfDestination,  rtfFooterLeft,  "footerl",  0},
  {rtfDestination,  rtfFooterRight,  "footerr",  0},
  {rtfDestination,  rtfFooterFirst,  "footerf",  0},
  {rtfDestination,  rtfParNumText,  "pntext",  0},
  {rtfDestination,  rtfParNumbering,  "pn",  0},
  {rtfDestination,  rtfParNumTextAfter,  "pntexta",  0},
  {rtfDestination,  rtfParNumTextBefore,  "pntextb",  0},
  {rtfDestination,  rtfBookmarkStart,  "bkmkstart",  0},
  {rtfDestination,  rtfBookmarkEnd,  "bkmkend",  0},
  {rtfDestination,  rtfPict,  "pict",  0},
  {rtfDestination,  rtfObject,  "object",  0},
  {rtfDestination,  rtfObjClass,  "objclass",  0},
  {rtfDestination,  rtfObjName,  "objname",  0},
  {rtfObjAttr,  rtfObjTime,  "objtime",  0},
  {rtfDestination,  rtfObjData,  "objdata",  0},
  {rtfDestination,  rtfObjAlias,  "objalias",  0},
  {rtfDestination,  rtfObjSection,  "objsect",  0},  /* objitem and objtopic aren't documented in the spec! */

  {rtfDestination,  rtfObjItem,  "objitem",  0},
  {rtfDestination,  rtfObjTopic,  "objtopic",  0},
  {rtfDestination,  rtfObjResult,  "result",  0},
  {rtfDestination,  rtfDrawObject,  "do",  0},
  {rtfDestination,  rtfFootnote,  "footnote",  0},
  {rtfDestination,  rtfAnnotRefStart,  "atrfstart",  0},
  {rtfDestination,  rtfAnnotRefEnd,  "atrfend",  0},
  {rtfDestination,  rtfAnnotID,  "atnid",  0},
  {rtfDestination,  rtfAnnotAuthor,  "atnauthor",  0},
  {rtfDestination,  rtfAnnotation,  "annotation",  0},
  {rtfDestination,  rtfAnnotRef,  "atnref",  0},
  {rtfDestination,  rtfAnnotTime,  "atntime",  0},
  {rtfDestination,  rtfAnnotIcon,  "atnicn",  0},
  {rtfDestination,  rtfField,  "field",  0},
  {rtfDestination,  rtfFieldInst,  "fldinst",  0},
  {rtfDestination,  rtfFieldResult,  "fldrslt",  0},
  {rtfDestination,  rtfDataField,  "datafield",  0},
  {rtfDestination,  rtfIndex,  "xe",  0},
  {rtfDestination,  rtfIndexText,  "txe",  0},
  {rtfDestination,  rtfIndexRange,  "rxe",  0},
  {rtfDestination,  rtfTOC,  "tc",  0},
  {rtfDestination,  rtfNeXTGraphic,  "NeXTGraphic",  0},
        /* LTO */

  {rtfDestination,  rtfListLevel,  "listlevel",  0},
  {rtfDestination,  rtfListText,  "listtext",  0},

  // 20060324 for unicode 
  {rtfDestination,  rtfNewDestination,  "*",  0},
  {rtfDestination,  rtfTwoEmbedded,  "upr",  0},
  {rtfDestination,  rtfUnicode,  "ud",  0},
  {rtfDestination,  rtfThemeData,  "themedata",  0},
  {rtfDestination,  rtfColorSchemeMapping,  "colorschememapping",  0},
  {rtfDestination,  rtfLatentStyles,  "latentstyles",  0},
  {rtfDestination,  rtfDataStore,  "datastore",  0},
  {rtfDestination,  rtfFactoIdName,  "factoidname",  0},

  /*
   * Font families
   */
  {rtfFontFamily,  rtfFFNil,  "fnil",  0},
  {rtfFontFamily,  rtfFFRoman,  "froman",  0},
  {rtfFontFamily,  rtfFFSwiss,  "fswiss",  0},
  {rtfFontFamily,  rtfFFModern,  "fmodern",  0},
  {rtfFontFamily,  rtfFFScript,  "fscript",  0},
  {rtfFontFamily,  rtfFFDecor,  "fdecor",  0},
  {rtfFontFamily,  rtfFFTech,  "ftech",  0},
  {rtfFontFamily,  rtfFFBidirectional,  "fbidi",  0},
  {rtfFontFamily,  rtfFFLoMajor,  "flomajor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFHiMajor,  "fhimajor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFDbMajor,  "fdbmajor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFBiMajor,  "fbimajor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFLoMinor,  "flominor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFHiMinor,  "fhiminor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFDbMinor,  "fdbminor",  0}, // Word 2007
  {rtfFontFamily,  rtfFFBiMinor,  "fbiminor",  0}, // Word 2007

  /*
   * Font attributes
   */
  {rtfFontAttr,  rtfFontCharSet,  "fcharset",  0},
  {rtfFontAttr,  rtfFontPitch,  "fprq",  0},
  {rtfFontAttr,  rtfFontCodePage,  "cpg",  0},
  {rtfFontAttr,  rtfFTypeNil,  "ftnil",  0},
  {rtfFontAttr,  rtfFTypeTrueType,  "fttruetype",  0},

  /*
   * File table attributes
   */
  {rtfFileAttr,  rtfFileNum,  "fid",  0},
  {rtfFileAttr,  rtfFileRelPath,  "frelative",  0},
  {rtfFileAttr,  rtfFileOSNum,  "fosnum",  0},

  /*
   * File sources
   */
  {rtfFileSource,  rtfSrcMacintosh,  "fvalidmac",  0},
  {rtfFileSource,  rtfSrcDOS,  "fvaliddos",  0},
  {rtfFileSource,  rtfSrcNTFS,  "fvalidntfs",  0},
  {rtfFileSource,  rtfSrcHPFS,  "fvalidhpfs",  0},
  {rtfFileSource,  rtfSrcNetwork,  "fnetwork",  0},

  /*
   * Color names
   */
  {rtfColorName,  rtfRed,  "red",  0},
  {rtfColorName,  rtfGreen,  "green",  0},
  {rtfColorName,  rtfBlue,  "blue",  0},

  /*
   * Charset names
   */
  {rtfCharSet,  rtfMacCharSet,  "mac",  0},
  {rtfCharSet,  rtfAnsiCharSet,  "ansi",  0},
  {rtfCharSet,  rtfPcCharSet,  "pc",  0},
  {rtfCharSet,  rtfPcaCharSet,  "pca",  0},

  /*
   * Table attributes
   */
  {rtfTblAttr,  rtfRowDef,  "trowd",  0},
  {rtfTblAttr,  rtfRowGapH,  "trgaph",  0},
  {rtfTblAttr,  rtfCellPos,  "cellx",  0},
  {rtfTblAttr,  rtfMergeRngFirst,  "clmgf",  0},
  {rtfTblAttr,  rtfMergePrevious,  "clmrg",  0},

  {rtfTblAttr,  rtfRowLeft,  "trql",  0},
  {rtfTblAttr,  rtfRowRight,  "trqr",  0},
  {rtfTblAttr,  rtfRowCenter,  "trqc",  0},
  {rtfTblAttr,  rtfRowLeftEdge,  "trleft",  0},
  {rtfTblAttr,  rtfRowHt,  "trrh",  0},
  {rtfTblAttr,  rtfRowHeader,  "trhdr",  0},
  {rtfTblAttr,  rtfRowKeep,  "trkeep",  0},

  {rtfTblAttr,  rtfRTLRow,  "rtlrow",  0},
  {rtfTblAttr,  rtfLTRRow,  "ltrrow",  0},

  {rtfTblAttr,  rtfRowBordTop,  "trbrdrt",  0},
  {rtfTblAttr,  rtfRowBordLeft,  "trbrdrl",  0},
  {rtfTblAttr,  rtfRowBordBottom,  "trbrdrb",  0},
  {rtfTblAttr,  rtfRowBordRight,  "trbrdrr",  0},
  {rtfTblAttr,  rtfRowBordHoriz,  "trbrdrh",  0},
  {rtfTblAttr,  rtfRowBordVert,  "trbrdrv",  0},

  {rtfTblAttr,  rtfCellBordBottom,  "clbrdrb",  0},
  {rtfTblAttr,  rtfCellBordTop,  "clbrdrt",  0},
  {rtfTblAttr,  rtfCellBordLeft,  "clbrdrl",  0},
  {rtfTblAttr,  rtfCellBordRight,  "clbrdrr",  0},

  {rtfTblAttr,  rtfCellShading,  "clshdng",  0},
  {rtfTblAttr,  rtfCellBgPatH,  "clbghoriz",  0},
  {rtfTblAttr,  rtfCellBgPatV,  "clbgvert",  0},
  {rtfTblAttr,  rtfCellFwdDiagBgPat,  "clbgfdiag",  0},
  {rtfTblAttr,  rtfCellBwdDiagBgPat,  "clbgbdiag",  0},
  {rtfTblAttr,  rtfCellHatchBgPat,  "clbgcross",  0},
  {rtfTblAttr,  rtfCellDiagHatchBgPat,  "clbgdcross",  0},

  /*
   * The spec lists "clbgdkhor", but the corresponding non-cell
   * control is "bgdkhoriz".  At any rate Macintosh Word seems
   * to accept both "clbgdkhor" and "clbgdkhoriz".
   */

  {rtfTblAttr,  rtfCellDarkBgPatH,  "clbgdkhoriz",  0},
  {rtfTblAttr,  rtfCellDarkBgPatH,  "clbgdkhor",  0},
  {rtfTblAttr,  rtfCellDarkBgPatV,  "clbgdkvert",  0},
  {rtfTblAttr,  rtfCellFwdDarkBgPat,  "clbgdkfdiag",  0},
  {rtfTblAttr,  rtfCellBwdDarkBgPat,  "clbgdkbdiag",  0},
  {rtfTblAttr,  rtfCellDarkHatchBgPat,  "clbgdkcross",  0},
  {rtfTblAttr,  rtfCellDarkDiagHatchBgPat,  "clbgdkdcross",  0},
  {rtfTblAttr,  rtfCellBgPatLineColor,  "clcfpat",  0},
  {rtfTblAttr,  rtfCellBgPatColor,  "clcbpat",  0},

  /*
   * Field attributes
   */
  {rtfFieldAttr,  rtfFieldDirty,  "flddirty",  0},
  {rtfFieldAttr,  rtfFieldEdited,  "fldedit",  0},
  {rtfFieldAttr,  rtfFieldLocked,  "fldlock",  0},
  {rtfFieldAttr,  rtfFieldPrivate,  "fldpriv",  0},
  {rtfFieldAttr,  rtfFieldAlt,  "fldalt",  0},

  /*
   * Positioning attributes
   */
  {rtfPosAttr,  rtfAbsWid,  "absw",  0},
  {rtfPosAttr,  rtfAbsHt,  "absh",  0},

  {rtfPosAttr,  rtfRPosMargH,  "phmrg",  0},
  {rtfPosAttr,  rtfRPosPageH,  "phpg",  0},
  {rtfPosAttr,  rtfRPosColH,  "phcol",  0},
  {rtfPosAttr,  rtfPosX,  "posx",  0},
  {rtfPosAttr,  rtfPosNegX,  "posnegx",  0},
  {rtfPosAttr,  rtfPosXCenter,  "posxc",  0},
  {rtfPosAttr,  rtfPosXInside,  "posxi",  0},
  {rtfPosAttr,  rtfPosXOutSide,  "posxo",  0},
  {rtfPosAttr,  rtfPosXRight,  "posxr",  0},
  {rtfPosAttr,  rtfPosXLeft,  "posxl",  0},

  {rtfPosAttr,  rtfRPosMargV,  "pvmrg",  0},
  {rtfPosAttr,  rtfRPosPageV,  "pvpg",  0},
  {rtfPosAttr,  rtfRPosParaV,  "pvpara",  0},
  {rtfPosAttr,  rtfPosY,  "posy",  0},
  {rtfPosAttr,  rtfPosNegY,  "posnegy",  0},
  {rtfPosAttr,  rtfPosYInline,  "posyil",  0},
  {rtfPosAttr,  rtfPosYTop,  "posyt",  0},
  {rtfPosAttr,  rtfPosYCenter,  "posyc",  0},
  {rtfPosAttr,  rtfPosYBottom,  "posyb",  0},

  {rtfPosAttr,  rtfNoWrap,  "nowrap",  0},
  {rtfPosAttr,  rtfDistFromTextAll,  "dxfrtext",  0},
  {rtfPosAttr,  rtfDistFromTextX,  "dfrmtxtx",  0},
  {rtfPosAttr,  rtfDistFromTextY,  "dfrmtxty",  0},  /* \dyfrtext no longer exists in spec 1.2, apparently */
  /* replaced by \dfrmtextx and \dfrmtexty. */

  {rtfPosAttr,  rtfTextDistY,  "dyfrtext",  0},

  {rtfPosAttr,  rtfDropCapLines,  "dropcapli",  0},
  {rtfPosAttr,  rtfDropCapType,  "dropcapt",  0},

  /*
   * Object controls
   */
  {rtfObjAttr,  rtfObjEmb,  "objemb",  0},
  {rtfObjAttr,  rtfObjLink,  "objlink",  0},
  {rtfObjAttr,  rtfObjAutoLink,  "objautlink",  0},
  {rtfObjAttr,  rtfObjSubscriber,  "objsub",  0},
  {rtfObjAttr,  rtfObjPublisher,  "objpub",  0},
  {rtfObjAttr,  rtfObjICEmb,  "objicemb",  0},

  {rtfObjAttr,  rtfObjLinkSelf,  "linkself",  0},
  {rtfObjAttr,  rtfObjLock,  "objupdate",  0},
  {rtfObjAttr,  rtfObjUpdate,  "objlock",  0},

  {rtfObjAttr,  rtfObjHt,  "objh",  0},
  {rtfObjAttr,  rtfObjWid,  "objw",  0},
  {rtfObjAttr,  rtfObjSetSize,  "objsetsize",  0},
  {rtfObjAttr,  rtfObjAlign,  "objalign",  0},
  {rtfObjAttr,  rtfObjTransposeY,  "objtransy",  0},
  {rtfObjAttr,  rtfObjCropTop,  "objcropt",  0},
  {rtfObjAttr,  rtfObjCropBottom,  "objcropb",  0},
  {rtfObjAttr,  rtfObjCropLeft,  "objcropl",  0},
  {rtfObjAttr,  rtfObjCropRight,  "objcropr",  0},
  {rtfObjAttr,  rtfObjScaleX,  "objscalex",  0},
  {rtfObjAttr,  rtfObjScaleY,  "objscaley",  0},

  {rtfObjAttr,  rtfObjResRTF,  "rsltrtf",  0},
  {rtfObjAttr,  rtfObjResPict,  "rsltpict",  0},
  {rtfObjAttr,  rtfObjResBitmap,  "rsltbmp",  0},
  {rtfObjAttr,  rtfObjResText,  "rslttxt",  0},
  {rtfObjAttr,  rtfObjResMerge,  "rsltmerge",  0},

  {rtfObjAttr,  rtfObjBookmarkPubObj,  "bkmkpub",  0},
  {rtfObjAttr,  rtfObjPubAutoUpdate,  "pubauto",  0},

  /*
   * Associated character formatting attributes
   */
  {rtfACharAttr,  rtfACBold,  "ab",  0},
  {rtfACharAttr,  rtfACAllCaps,  "caps",  0},
  {rtfACharAttr,  rtfACForeColor,  "acf",  0},
  {rtfACharAttr,  rtfACSubScript,  "adn",  0},
  {rtfACharAttr,  rtfACExpand,  "aexpnd",  0},
  {rtfACharAttr,  rtfACFontNum,  "af",  0},
  {rtfACharAttr,  rtfACFontSize,  "afs",  0},
  {rtfACharAttr,  rtfACItalic,  "ai",  0},
  {rtfACharAttr,  rtfACLanguage,  "alang",  0},
  {rtfACharAttr,  rtfACOutline,  "aoutl",  0},
  {rtfACharAttr,  rtfACSmallCaps,  "ascaps",  0},
  {rtfACharAttr,  rtfACShadow,  "ashad",  0},
  {rtfACharAttr,  rtfACStrikeThru,  "astrike",  0},
  {rtfACharAttr,  rtfACUnderline,  "aul",  0},
  {rtfACharAttr,  rtfACDotUnderline,  "auld",  0},
  {rtfACharAttr,  rtfACDbUnderline,  "auldb",  0},
  {rtfACharAttr,  rtfACNoUnderline,  "aulnone",  0},
  {rtfACharAttr,  rtfACWordUnderline,  "aulw",  0},
  {rtfACharAttr,  rtfACSuperScript,  "aup",  0},
  {rtfACharAttr,  rtfACLowANSI,  "loch",  0},
  {rtfACharAttr,  rtfACHighANSI,  "hich",  0},
  {rtfACharAttr,  rtfACDoubleByte,  "dbch",  0},

  /*
   * Footnote attributes
   */
  {rtfFNoteAttr,  rtfFNAlt,  "ftnalt",  0},

  /*
   * Key code attributes
   */
  {rtfKeyCodeAttr,  rtfAltKey,  "alt",  0},
  {rtfKeyCodeAttr,  rtfShiftKey,  "shift",  0},
  {rtfKeyCodeAttr,  rtfControlKey,  "ctrl",  0},
  {rtfKeyCodeAttr,  rtfFunctionKey,  "fn",  0},

  /*
   * Bookmark attributes
   */
  {rtfBookmarkAttr,  rtfBookmarkFirstCol,  "bkmkcolf",  0},
  {rtfBookmarkAttr,  rtfBookmarkLastCol,  "bkmkcoll",  0},

  /*
   * Index entry attributes
   */
  {rtfIndexAttr,  rtfIndexNumber,  "xef",  0},
  {rtfIndexAttr,  rtfIndexBold,  "bxe",  0},
  {rtfIndexAttr,  rtfIndexItalic,  "ixe",  0},

  /*
   * Table of contents attributes
   */
  {rtfTOCAttr,  rtfTOCType,  "tcf",  0},
  {rtfTOCAttr,  rtfTOCLevel,  "tcl",  0},

  /*
   * Drawing object attributes
   */
  {rtfDrawAttr,  rtfDrawLock,  "dolock",  0},
  {rtfDrawAttr,  rtfDrawPageRelX,  "doxpage",  0},
  {rtfDrawAttr,  rtfDrawColumnRelX,  "dobxcolumn",  0},
  {rtfDrawAttr,  rtfDrawMarginRelX,  "dobxmargin",  0},
  {rtfDrawAttr,  rtfDrawPageRelY,  "dobypage",  0},
  {rtfDrawAttr,  rtfDrawColumnRelY,  "dobycolumn",  0},
  {rtfDrawAttr,  rtfDrawMarginRelY,  "dobymargin",  0},
  {rtfDrawAttr,  rtfDrawHeight,  "dobhgt",  0},

  {rtfDrawAttr,  rtfDrawBeginGroup,  "dpgroup",  0},
  {rtfDrawAttr,  rtfDrawGroupCount,  "dpcount",  0},
  {rtfDrawAttr,  rtfDrawEndGroup,  "dpendgroup",  0},
  {rtfDrawAttr,  rtfDrawArc,  "dparc",  0},
  {rtfDrawAttr,  rtfDrawCallout,  "dpcallout",  0},
  {rtfDrawAttr,  rtfDrawEllipse,  "dpellipse",  0},
  {rtfDrawAttr,  rtfDrawLine,  "dpline",  0},
  {rtfDrawAttr,  rtfDrawPolygon,  "dppolygon",  0},
  {rtfDrawAttr,  rtfDrawPolyLine,  "dppolyline",  0},
  {rtfDrawAttr,  rtfDrawRect,  "dprect",  0},
  {rtfDrawAttr,  rtfDrawTextBox,  "dptxbx",  0},

  {rtfDrawAttr,  rtfDrawOffsetX,  "dpx",  0},
  {rtfDrawAttr,  rtfDrawSizeX,  "dpxsize",  0},
  {rtfDrawAttr,  rtfDrawOffsetY,  "dpy",  0},
  {rtfDrawAttr,  rtfDrawSizeY,  "dpysize",  0},

  {rtfDrawAttr,  rtfCOAngle,  "dpcoa",  0},
  {rtfDrawAttr,  rtfCOAccentBar,  "dpcoaccent",  0},
  {rtfDrawAttr,  rtfCOBestFit,  "dpcobestfit",  0},
  {rtfDrawAttr,  rtfCOBorder,  "dpcoborder",  0},
  {rtfDrawAttr,  rtfCOAttachAbsDist,  "dpcodabs",  0},
  {rtfDrawAttr,  rtfCOAttachBottom,  "dpcodbottom",  0},
  {rtfDrawAttr,  rtfCOAttachCenter,  "dpcodcenter",  0},
  {rtfDrawAttr,  rtfCOAttachTop,  "dpcodtop",  0},
  {rtfDrawAttr,  rtfCOLength,  "dpcolength",  0},
  {rtfDrawAttr,  rtfCONegXQuadrant,  "dpcominusx",  0},
  {rtfDrawAttr,  rtfCONegYQuadrant,  "dpcominusy",  0},
  {rtfDrawAttr,  rtfCOOffset,  "dpcooffset",  0},
  {rtfDrawAttr,  rtfCOAttachSmart,  "dpcosmarta",  0},
  {rtfDrawAttr,  rtfCODoubleLine,  "dpcotdouble",  0},
  {rtfDrawAttr,  rtfCORightAngle,  "dpcotright",  0},
  {rtfDrawAttr,  rtfCOSingleLine,  "dpcotsingle",  0},
  {rtfDrawAttr,  rtfCOTripleLine,  "dpcottriple",  0},

  {rtfDrawAttr,  rtfDrawTextBoxMargin,  "dptxbxmar",  0},
  {rtfDrawAttr,  rtfDrawTextBoxText,  "dptxbxtext",  0},
  {rtfDrawAttr,  rtfDrawRoundRect,  "dproundr",  0},

  {rtfDrawAttr,  rtfDrawPointX,  "dpptx",  0},
  {rtfDrawAttr,  rtfDrawPointY,  "dppty",  0},
  {rtfDrawAttr,  rtfDrawPolyCount,  "dppolycount",  0},

  {rtfDrawAttr,  rtfDrawArcFlipX,  "dparcflipx",  0},
  {rtfDrawAttr,  rtfDrawArcFlipY,  "dparcflipy",  0},

  {rtfDrawAttr,  rtfDrawLineBlue,  "dplinecob",  0},
  {rtfDrawAttr,  rtfDrawLineGreen,  "dplinecog",  0},
  {rtfDrawAttr,  rtfDrawLineRed,  "dplinecor",  0},
  {rtfDrawAttr,  rtfDrawLinePalette,  "dplinepal",  0},
  {rtfDrawAttr,  rtfDrawLineDashDot,  "dplinedado",  0},
  {rtfDrawAttr,  rtfDrawLineDashDotDot,  "dplinedadodo",  0},
  {rtfDrawAttr,  rtfDrawLineDash,  "dplinedash",  0},
  {rtfDrawAttr,  rtfDrawLineDot,  "dplinedot",  0},
  {rtfDrawAttr,  rtfDrawLineGray,  "dplinegray",  0},
  {rtfDrawAttr,  rtfDrawLineHollow,  "dplinehollow",  0},
  {rtfDrawAttr,  rtfDrawLineSolid,  "dplinesolid",  0},
  {rtfDrawAttr,  rtfDrawLineWidth,  "dplinew",  0},

  {rtfDrawAttr,  rtfDrawHollowEndArrow,  "dpaendhol",  0},
  {rtfDrawAttr,  rtfDrawEndArrowLength,  "dpaendl",  0},
  {rtfDrawAttr,  rtfDrawSolidEndArrow,  "dpaendsol",  0},
  {rtfDrawAttr,  rtfDrawEndArrowWidth,  "dpaendw",  0},
  {rtfDrawAttr,  rtfDrawHollowStartArrow,  "dpastarthol",  0},
  {rtfDrawAttr,  rtfDrawStartArrowLength,  "dpastartl",  0},
  {rtfDrawAttr,  rtfDrawSolidStartArrow,  "dpastartsol",  0},
  {rtfDrawAttr,  rtfDrawStartArrowWidth,  "dpastartw",  0},

  {rtfDrawAttr,  rtfDrawBgFillBlue,  "dpfillbgcb",  0},
  {rtfDrawAttr,  rtfDrawBgFillGreen,  "dpfillbgcg",  0},
  {rtfDrawAttr,  rtfDrawBgFillRed,  "dpfillbgcr",  0},
  {rtfDrawAttr,  rtfDrawBgFillPalette,  "dpfillbgpal",  0},
  {rtfDrawAttr,  rtfDrawBgFillGray,  "dpfillbggray",  0},
  {rtfDrawAttr,  rtfDrawFgFillBlue,  "dpfillfgcb",  0},
  {rtfDrawAttr,  rtfDrawFgFillGreen,  "dpfillfgcg",  0},
  {rtfDrawAttr,  rtfDrawFgFillRed,  "dpfillfgcr",  0},
  {rtfDrawAttr,  rtfDrawFgFillPalette,  "dpfillfgpal",  0},
  {rtfDrawAttr,  rtfDrawFgFillGray,  "dpfillfggray",  0},
  {rtfDrawAttr,  rtfDrawFillPatIndex,  "dpfillpat",  0},

  {rtfDrawAttr,  rtfDrawShadow,  "dpshadow",  0},
  {rtfDrawAttr,  rtfDrawShadowXOffset,  "dpshadx",  0},
  {rtfDrawAttr,  rtfDrawShadowYOffset,  "dpshady",  0},
  {rtfVersion,  -1,      "rtf",    0},
  {rtfDefFont,  -1,      "deff",    0},
  {rtfCodePage, -1,      "ansicpg",    0},

  {0,    -1,      (char *) NULL,  0}
};

/*
 * Initialize lookup table hash values.  Only need to do this once.
 */
static void LookupInit ()
{
  static int  inited = 0;
  RTFKey  *rp;

  if (inited == 0)
  {
    for (rp = rtfKey; rp->rtfKStr != (char *) NULL; rp++)
      rp->rtfKHash = SHash (rp->rtfKStr);
    ++inited;
  }
}

/*
 * Determine major and minor number of control token.  If it's
 * not found, the class turns into rtfUnknown.
 */
static void Lookup (char  *s)
{
  RTFKey  *rp;
  int  hash;

  ++s;      /* skip over the leading \ character */
  hash = SHash (s);
  for (rp = rtfKey; rp->rtfKStr != (char *) NULL; rp++)
  {
    if (hash == rp->rtfKHash && strcmp (s, rp->rtfKStr) == 0)
    {
      rtfClass = rtfControl;
      rtfMajor = rp->rtfKMajor;
      rtfMinor = rp->rtfKMinor;
      return;
    }
  }
  rtfClass = rtfUnknown;
}

/*
 * Compute hash value of symbol
 */
static int SHash (const char *s)
{
  wchar_t  c;
  int  val = 0;

  while ((c = *s++) != '\0')
    val += (int) c;
  return (val);
}

/* ---------------------------------------------------------------------- */

/*
 * Memory allocation routines
 */

/*
 * Return pointer to block of size bytes, or NULL if there's
 * not enough memory available.
 *
 * This is called through RTFAlloc(), a define which coerces the
 * argument to int.  This avoids the persistent problem of allocation
 * failing under THINK C when a long is passed.
 */
char * _RTFAlloc (int  size)
{
  return ((char *) malloc (size));
}

/*
 * Saves a wstring on the heap and returns a pointer to it.
 */
char * RTFStrSave (char  *s)
{
  size_t len_s = strlen(s) + 1;
  char *p = new char[len_s];

  if (p == (char *) NULL)
    return ((char *) NULL);

  //return (strcpy (p, s));
  memcpy(p, s, len_s);
  return p;
}

wchar_t * RTFWStrSave (wchar_t *s)
{
  size_t len_s = wcslen(s) + 1;
  wchar_t*  p = new wchar_t[len_s];

  if (p == (wchar_t *) NULL)
    return ((wchar_t *) NULL);
  //return (wcscpy (p, s));
  memcpy(p, s, len_s);
  return p;
}

void RTFWFree (wchar_t *p)
{
  if (p != (wchar_t *) NULL)
    free (p);
}

void RTFFree (char *p)
{
  if (p != (char *) NULL)
    free (p);
}


/* ---------------------------------------------------------------------- */


/*
 * Token comparison routines
 */
bool RTFCheckCM (int _class, int major)
{
  return (rtfClass == _class && rtfMajor == major);
}

bool RTFCheckCMM (int _class, int major, int minor)
{
  return (rtfClass == _class && rtfMajor == major && rtfMinor == minor);
}

bool RTFCheckMM (int major, int minor)
{
  return (rtfMajor == major && rtfMinor == minor);
}


/* ---------------------------------------------------------------------- */

int RTFCharToHex (wchar_t  c)
{
  if (isupper (c))
    c = tolower (c);
  if (isdigit (c))
    return (c - '0');  /* '0'..'9' */
  return (c - 'a' + 10);    /* 'a'..'f' */
}

int RTFHexToChar (int  i)
{
  if (i < 10)
    return (i + '0');
  return (i - 10 + 'a');
}

/* ---------------------------------------------------------------------- */

/*
 * RTFReadOutputMap() -- Read output translation map
 */

/*
 * Read in a file describing the relation between the standard character set
 * and an RTF translator's corresponding output sequences.  Each line consists
 * of a standard character name and the output sequence for that character.
 *
 * outMap is an array of strings into which the sequences should be placed.
 * It should be declared like this in the calling program:
 *
 *  wchar_t *outMap[rtfSC_MaxChar];
 *
 * reinit should be non-zero if outMap should be initialized before reading the
 * file, zero otherwise.  (This allows the map to be constructed by reading
 * several files.)  It's assumed that any existing strings in the map were
 * allocated by RTFStrSave().  The map is initialized BEFORE any attempt is
 * made to read the file.
 *
 * If the filename is an absolute pathname, look in the specified location
 * only.  Otherwise try to find the file in the current directory or the
 * library directory.
 */

int RTFReadOutputMap ( const char *outMap[] )
{
   outMap[rtfSC_space  ] = " ";
   outMap[rtfSC_exclam  ] = "!";
   outMap[rtfSC_quotedbl] = "\"";
   outMap[rtfSC_numbersign] = "#";
   outMap[rtfSC_dollar  ] = "$";
   outMap[rtfSC_percent  ] = "%";
   outMap[rtfSC_ampersand] = "&";
   outMap[rtfSC_quoteright] = "\'";
   outMap[rtfSC_parenleft] = "(";
   outMap[rtfSC_parenright] = ")";
   outMap[rtfSC_asterisk] = "*";
   outMap[rtfSC_plus  ] = "+";
   outMap[rtfSC_comma  ] = ",";
   outMap[rtfSC_hyphen  ] = "-";
   outMap[rtfSC_period  ] = ".";
   outMap[rtfSC_slash  ] = "/";
   outMap[rtfSC_zero  ] = "0";
   outMap[rtfSC_one  ] = "1";
   outMap[rtfSC_two  ] = "2";
   outMap[rtfSC_three  ] = "3";
   outMap[rtfSC_four  ] = "4";
   outMap[rtfSC_five  ] = "5";
   outMap[rtfSC_six  ] = "6";
   outMap[rtfSC_seven  ] = "7";
   outMap[rtfSC_eight  ] = "8";
   outMap[rtfSC_nine  ] = "9";
   outMap[rtfSC_colon  ] = ":";
   outMap[rtfSC_semicolon] = ";";
   outMap[rtfSC_less  ] = "<";
   outMap[rtfSC_equal  ] = "=";
   outMap[rtfSC_greater  ] = ">";
   outMap[rtfSC_question] = "?";
   outMap[rtfSC_at  ] = "@";
   outMap[rtfSC_A  ] = "A";
   outMap[rtfSC_B  ] = "B";
   outMap[rtfSC_C  ] = "C";
   outMap[rtfSC_D  ] = "D";
   outMap[rtfSC_E  ] = "E";
   outMap[rtfSC_F  ] = "F";
   outMap[rtfSC_G  ] = "G";
   outMap[rtfSC_H  ] = "H";
   outMap[rtfSC_I  ] = "I";
   outMap[rtfSC_J  ] = "J";
   outMap[rtfSC_K  ] = "K";
   outMap[rtfSC_L  ] = "L";
   outMap[rtfSC_M  ] = "M";
   outMap[rtfSC_N  ] = "N";
   outMap[rtfSC_O  ] = "O";
   outMap[rtfSC_P  ] = "P";
   outMap[rtfSC_Q  ] = "Q";
   outMap[rtfSC_R  ] = "R";
   outMap[rtfSC_S  ] = "S";
   outMap[rtfSC_T  ] = "T";
   outMap[rtfSC_U  ] = "U";
   outMap[rtfSC_V  ] = "V";
   outMap[rtfSC_W  ] = "W";
   outMap[rtfSC_X  ] = "X";
   outMap[rtfSC_Y  ] = "Y";
   outMap[rtfSC_Z  ] = "Z";
   outMap[rtfSC_bracketleft] = "[";
   outMap[rtfSC_backslash] = "\\";
   outMap[rtfSC_bracketright] = "]";
   outMap[rtfSC_asciicircum] = "^";
   outMap[rtfSC_underscore] = "_";
   outMap[rtfSC_quoteleft] = "`";
   outMap[rtfSC_a  ] = "a";
   outMap[rtfSC_b  ] = "b";
   outMap[rtfSC_c  ] = "c";
   outMap[rtfSC_d  ] = "d";
   outMap[rtfSC_e  ] = "e";
   outMap[rtfSC_f  ] = "f";
   outMap[rtfSC_g  ] = "g";
   outMap[rtfSC_h  ] = "h";
   outMap[rtfSC_i  ] = "i";
   outMap[rtfSC_j  ] = "j";
   outMap[rtfSC_k  ] = "k";
   outMap[rtfSC_l  ] = "l";
   outMap[rtfSC_m  ] = "m";
   outMap[rtfSC_n  ] = "n";
   outMap[rtfSC_o  ] = "o";
   outMap[rtfSC_p  ] = "p";
   outMap[rtfSC_q  ] = "q";
   outMap[rtfSC_r  ] = "r";
   outMap[rtfSC_s  ] = "s";
   outMap[rtfSC_t  ] = "t";
   outMap[rtfSC_u  ] = "u";
   outMap[rtfSC_v  ] = "v";
   outMap[rtfSC_w  ] = "w";
   outMap[rtfSC_x  ] = "x";
   outMap[rtfSC_y  ] = "y";
   outMap[rtfSC_z  ] = "z";
   outMap[rtfSC_braceleft] = "{";
   outMap[rtfSC_bar  ] = "|";
   outMap[rtfSC_braceright] = "}";
   outMap[rtfSC_asciitilde] = "~";
   outMap[rtfSC_AE  ] = "AE";
   outMap[rtfSC_OE  ] = "OE";
   outMap[rtfSC_acute  ] = "\'";
   outMap[rtfSC_ae  ] = "ae";
   outMap[rtfSC_angleleft] = "<";
   outMap[rtfSC_angleright] = ">";
   outMap[rtfSC_arrowboth] = "<->";
   outMap[rtfSC_arrowdblboth] = "<=>";
   outMap[rtfSC_arrowdblleft] = "<=";
   outMap[rtfSC_arrowdblright] = "=>";
   outMap[rtfSC_arrowleft] = "<-";
   outMap[rtfSC_arrowright] = "->";
   outMap[rtfSC_bullet  ] = "o";
   outMap[rtfSC_cent  ] = "cent";
   outMap[rtfSC_circumflex] = "^";
   outMap[rtfSC_copyright] = "(c)";
   outMap[rtfSC_copyrightsans] = "(c)";
   outMap[rtfSC_degree  ] = "deg.";
   outMap[rtfSC_divide  ] = "/";
   outMap[rtfSC_dotlessi] = "i";
   outMap[rtfSC_ellipsis] = "...";
   outMap[rtfSC_emdash  ] = "--";
   outMap[rtfSC_endash  ] = "-";
   outMap[rtfSC_fi  ] = "fi";
   outMap[rtfSC_fl  ] = "fl";
   outMap[rtfSC_fraction] = "/";
   outMap[rtfSC_germandbls] = "ss";
         //   outMap[rtfSC_grave  ] = "\\`";  
   outMap[rtfSC_grave  ] = "`";  
   outMap[rtfSC_greaterequal] = ">=";
   outMap[rtfSC_guillemotleft] = "<<";
   outMap[rtfSC_guillemotright] = ">>";
   outMap[rtfSC_guilsinglleft] = "<";
   outMap[rtfSC_guilsinglright] = ">";
   outMap[rtfSC_lessequal] = "<=";
   outMap[rtfSC_logicalnot] = "~";
   outMap[rtfSC_mathasterisk] = "*";
   outMap[rtfSC_mathequal] = "=";
   outMap[rtfSC_mathminus] = "-";
   outMap[rtfSC_mathnumbersign] = "#";
   outMap[rtfSC_mathplus] = "+";
   outMap[rtfSC_mathtilde] = "~";
   outMap[rtfSC_minus  ] = "-";
   outMap[rtfSC_mu  ] = "u";
   outMap[rtfSC_multiply] = "x";
   outMap[rtfSC_nobrkhyphen] = "-";
   outMap[rtfSC_nobrkspace] = "\\ ";
   outMap[rtfSC_notequal] = "!=";
   outMap[rtfSC_oe  ] = "oe";
   outMap[rtfSC_onehalf  ] = "1/2";
   outMap[rtfSC_onequarter] = "1/4";
   outMap[rtfSC_periodcentered] = ".";
   outMap[rtfSC_plusminus] = "+/-";
   outMap[rtfSC_quotedblbase] = ",,";
   outMap[rtfSC_quotedblleft] = "\"";
   outMap[rtfSC_quotedblright] = "\"";
   outMap[rtfSC_quotesinglbase] = ",";
   outMap[rtfSC_registered] = "reg.";
   outMap[rtfSC_registersans] = "reg.";
   outMap[rtfSC_threequarters] = "3/4";
   outMap[rtfSC_tilde  ] = "~";
   outMap[rtfSC_trademark] = "(TM)";
   outMap[rtfSC_trademarksans] = "(TM)";

  return (1);
}


/* ---------------------------------------------------------------------- */

/*
 * Open a library file.
 */


static RTFOpenFileFuncPtr libFileOpen  = NULL;

void RTFSetOpenLibFileProc (RTFOpenFileFuncPtr proc )
{
  libFileOpen = proc;
}

FILE * RTFOpenLibFile (wchar_t *file, wchar_t *mode)
{
  if (libFileOpen == NULL)
    return ((FILE *) NULL);
  return ((*libFileOpen) (file, mode));
}

/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */

//////////////////////////////////////
// Error Message Handling
//////////////////////////////////////

// Mesaage to log or stdoutput - no exception

void RTFMsg (const std::wstring & msg)
{
  vdm_log << msg;
  return; 
}

// Fatal Errors
// Exception ERR_RTF_Parser is thrown 

void RTFPanic (const std::wstring & msg)
{
  std::wstring thisMsg(msg);
  thisMsg += L"\n";

  if (prevChar != EOF && rtfTextBuf != (char *) NULL)
    thisMsg += L"Last token read was \"" + TBWSTR::string2wstring(rtfTextBuf) +
               L"\" near line " + Int(rtfLineNum).ascii() +
               L", position " + Int(rtfLinePos).ascii() +
               L".\n";

  vdm_err << thisMsg;
  throw TB_Exception(ERR_RTF_Parser);
}

// 20060324 for unicode
void ReadNewDestination()
{
}

// for "upr"
void ReadTwoEmbedded()
{
}

// for "ud"
// discard all
void ReadUnicode()
{
  int count = 0;
  for (;;)
  {
    (void) RTFGetToken ();
    if (RTFCheckCM (rtfGroup, rtfBeginGroup))
    {
      count++;
    }
    else if (RTFCheckCM (rtfGroup, rtfEndGroup))
    {
      count--;
      if( count == 0 ) break;
    }
  }
}

void ReadThemeData()
{
  RTFSkipGroup ();
}

void ReadColorSchemeMapping()
{
  RTFSkipGroup ();
}

void ReadLatentStyles()
{
  RTFSkipGroup ();
}

void ReadDataStore()
{
  RTFSkipGroup ();
}

void FactoIdName()
{
  RTFSkipGroup ();
}

void ACharAttr()
{
  switch(rtfMinor) {
    case rtfACLowANSI:
    case rtfACHighANSI: {
      ansi = true;
      break;
    }
    case rtfACDoubleByte: {
      ansi = false;
      break;
    }
  }
}

void ANSICodePage()
{
  codepage = rtfParam;
}

void RTFVersion()
{
}

void DefaultLanguage()
{
  deflang = rtfParam;
}

void DefaultLanguageFE()
{
  deflangfe = rtfParam;
}

void RTFBookmarkStart()
{
  RTFSkipGroup ();
}

void RTFBookmarkEnd()
{
  RTFSkipGroup ();
}

