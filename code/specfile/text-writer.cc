/***
*  * WHAT
*  *    This file is part of the rtf parser toolset
*  *    that has been downloaded from www
*  *    http://night.primate.wisc.edu:80/software/RTF
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/text-writer.cc,v $
*  * VERSION
*  *    $Revision: 1.14 $
*  * DATE
*  *    $Date: 2005/10/27 05:28:35 $
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
 * text-writer -- RTF-to-text translation writer code.
 *
 * Read RTF input, write text of document (text extraction).
 *
 * Wrapper must call WriterInit() once before processing any files,
 * then set up input and call BeginFile() for each input file.
 *
 * This installs callbacks for the text and control token classes.
 * The control class is necessary so that special characters such as
 * \par, \tab, \sect, etc.  can be converted.
 *
 * It's problematic what to do with text in headers and footers, and
 * what to do about tables.
 *
 * This really is quite a stupid program, for instance, it could keep
 * track of the current leader character and dump that out when a tab
 * is encountered.
 *
 * 04 Feb 91	Paul DuBois	dubois@primate.wisc.edu
 *
 * 04 Feb 91
 * -Created.
 * 27 Feb 91
 * - Updated for distribution 1.05.
 * 13 Jul 93
 * - Updated to compile under THINK C 6.0.
 * 31 Aug 93
 * - Added Mike Sendall's entries for Macintosh char map.
 * 07 Sep 93
 * - Uses charset map and output sequence map for character translation.
 * 11 Mar 94
 * - Updated for 1.10 distribution.
 */

#define TEST_OUT 0
#include  <stdio.h>
#include  <ctype.h>

// RTF STUFF
#include  "rtf.h"
#include  "rtf2text.h"
#include "reader.h"
#include <sstream>

// TB STUFF
#include "metaiv.h"
#include "chunks.h"
#include "tb_wstring.h"

/* from token scan */
char * TSGetScanPos ();

static void  TextClass ();
static void  ControlClass ();
static void  Destination ();
static void  SpecialChar ();
static void  PutStdChar (int);
static void  PutLitChar (int);
       void  PutLitStr (const char *);
       void  PutMBLitStr (const string&);

void AdjustBufSize(int );
static const char *outMap[rtfSC_MaxChar];

static int section_count =1;  // first section in document

/*******************************************************/

/***********************************************
  CHUNK HANDLING
 ************************************************/

static string chunk_buffer; 
static int chunk_id;    /* the type of the current chunk */

static string chunk_footer = "";
static string plaintext_chunk_header; /* RTF header to a plain text section */
static int    plaintext_meet = 0;     /* Has plain text been meet in the plain text paragraph */
// static int    group_with_plain_text = 0; /* are we currently in a group containing plain text */

/***********************************************
  THE WORD STYLES
 ************************************************/

/* The number of the style used in the RTF document */
/* -1 is used until the stylesheet has been read */

// paragraph styles
static const int CHUNK_BLOB      = 0;     /* section is Blob or Normal style */
static int style_VDM       = -1;    /* section in the VDM style */
static int style_VDM_IF    = -1;    /* section in the VDM_IF style */
static int style_VDM_ENDIF = -1;    /* section in the VDM_ENDIF style */

// TEST COVERAGE SECTIONS
static int style_VDM_TC_TABLE = -1;    /* section in the VDM TESTCOVERAGE TABLE style */

// character styles

static int style_VDM_COV   = -1;    /* cov char style */
static int style_VDM_NCOV  = -1;    /* not cov char */


static const char * style_name_VDM       = "VDM";
static const char * style_name_VDM_TC_TABLE       = "VDM_TC_TABLE";
static const char * style_name_VDM_IF    = "VDM_IF";
static const char * style_name_VDM_ENDIF = "VDM_ENDIF";

static const char * style_name_VDM_COV   = "VDM_COV";
static const char * style_name_VDM_NCOV  = "VDM_NCOV";

/***********************************************
  REPARING THE RTF 
 ************************************************/

static int reset_paragraph; /* prefix style change with /pard/plain */

/***********************************************
  FOR INPUT THAT SHOULD BE STRIPPED TO PLAIN TEXT 
 ************************************************/

// plain_text indicates if we are a paragraph that 
// should be turned into plain text and writen to the chunk buffe
//  != 0  => strip text and insert in chunk buffer
//  0     => ignore 
static int plain_text;
static int plain_text_last;     /*  value of plain_text from the last chunk */

///////////////////////////////////////////
// Get character styles for covered not covered sections
///////////////////////////////////////////


string get_char_style(int num) {
  RTFStyle * sty = RTFGetStyle(num);
  ostringstream buf;
//  buf << "\\cs" << num;
  buf << "\\*\\cs" << num; // 20070123

  if (sty) {
    RTFStyleElt *elt = sty->rtfSSEList;
    while(elt) {
      buf << elt->rtfSEText;
      elt = elt->rtfNextSE;
    }
  }
  buf << " ";
  return buf.str();
}

string get_vdm_cov() {
  return get_char_style(style_VDM_COV);
}

string get_vdm_ncov() {
  return get_char_style(style_VDM_NCOV);
}

///////////////////////////////////////////
// Set the sequence in which to insert the chunks
///////////////////////////////////////////

static ChunkList * chunks;

void RTFSetChunkSequence(ChunkList * cs) {
   chunks = cs;
}

/***********************************************
 Handling of list of chunks
 ************************************************/

// If in plain_text mode then collect the charaters
// in the chunk buffer

void AddCharToChunkBuffer(char c);


/****************************************************
 Remember the position of the end of the last token 
*****************************************************/

/* Gets the Token before any one else */

void HandleAllTokens() {
}

/****************************************************
  Initialize the RTF parser 
*****************************************************/

void InitChunkList() {
  plain_text = 0;
  plain_text_last = 0;
  chunk_buffer = ""; 

  chunk_footer = "";

  plaintext_chunk_header = "";// the header for a plain text paragraph
  plaintext_meet = 0;   // plain text has not been meet yet in the current paragraph
  //  group_with_plain_text = 0;  // we are not in a group containing plain text

  chunk_id = CHUNK_BLOB; 
  RTFSetReadHook(HandleAllTokens);
  reset_paragraph =0;
  section_count = 1;
  
  style_VDM       = -1; 
  style_VDM_IF    = -1; 
  style_VDM_ENDIF = -1; 
  style_VDM_TC_TABLE = -1; 
  style_VDM_COV   = -1; 
  style_VDM_NCOV  = -1; 

}

///////////////////////////////////////
// ADD A NEW CHUNK/SECTION
///////////////////////////////////////

static void GenNewSection(SpecFileChunkType s, bool /* pt */)
 {
   switch (s) {

   case CHUNK_TYPE_VDM : {     
     ChunkVDM * ck = new ChunkVDM;

     ck->set_section(section_count);

     // remove the last char in the header (is part of the text)
     if (plaintext_chunk_header.length() > 0) {
       //#if defined(_MSC_VER) || (__GNUC__ >= 2 && __GNUC_MINOR__ >= 8)
       plaintext_chunk_header.erase(plaintext_chunk_header.length()-1);
       //#else
       //       plaintext_chunk_header.remove(plaintext_chunk_header.length()-1);
       //#endif
       // The following is definitely a hack! The code removes "\par"
       // from the header if exists at the end of the string. The
       // reason is that otherwise additional blank lines will be
       // pretty printed in some special cases...
       
       string::size_type n;
       const char *remove_this = "\\par";
       if( (n = plaintext_chunk_header.find_last_of(remove_this)) 
           != string::npos){   
         int ws_read = 1;
         string::size_type i = n + 1;
         while(i < plaintext_chunk_header.length() && ws_read )
           ws_read = isspace(plaintext_chunk_header[i]);
         if(ws_read){
           // The string plaintext_chunk_header is ended by '\par' and
           // some white space. OK To remove:
           plaintext_chunk_header = 
             plaintext_chunk_header.substr(0, n - strlen(remove_this));
         }
       }
     }

#if TEST_OUT
     cout << "=================\nVDM:\n" << plaintext_chunk_header << 
       "\n-----------------\n" <<chunk_buffer << "\n- - - - -\n" << chunk_footer << "\n";
#endif

     // HEADER

     ck->header = plaintext_chunk_header;

     // BODY
     char sc_buf[30];
     sprintf(sc_buf, "%d", section_count);
     ck->vdm_ascii = "\n--LINE " + string(sc_buf) + " 1\n";
     ck->vdm_ascii += chunk_buffer;
     
     // FOOTER

     // add \n\par because it is removed during plain text scan
     ck->footer = "\n\\par ";
     ck->footer += chunk_footer;

     // ADD
     
     chunks->Add(ck);
     break;
   }


   case CHUNK_TYPE_VDM_TC_TABLE : {     
     ChunkVDM_TC_TABLE * ck = new ChunkVDM_TC_TABLE;
     ck->set_section(section_count);

     // remove the last char in the header (is part of the text)
     if (plaintext_chunk_header.length() > 0) {
       int l = plaintext_chunk_header.length();
       // This is something of a hack: if the text contains non-ascii
       // characters then there could be spurious unicode or \' chars
       // at the end of the chunk (if there is a unicode char, then
       // there will also be a \' char, experience shows)
       if (plaintext_chunk_header.substr(l-11, 2) == "\\u") {
         plaintext_chunk_header.erase(l-11, 11);
       }
       else if (plaintext_chunk_header.substr(l-4,2) == "\\'") {
         plaintext_chunk_header.erase(l-4, 4);
       }
       else {
	 plaintext_chunk_header.erase(plaintext_chunk_header.length()-1);
       }
     }

     // HEADER

     ck->header = plaintext_chunk_header;

     // BODY

     int f,t;
     // strip the class/module name for leading/trailing non alpha chars
     for (f = 0; (unsigned)f<chunk_buffer.length();  f++) {
       if ((unsigned)(chunk_buffer[f]) > 0x20) {
         break; 
       }
     }
     for (t = chunk_buffer.size()-1; t >=0 ; t--) {
       if ((unsigned)(chunk_buffer[t]) > 0x20) {
         break;
       }
     }
     if (t>=f) {
       ck->mod_name = chunk_buffer.substr(f, t-f+1);
     }
     else {
       ck->mod_name = ""; 
     }
     
     // FOOTER

     // add \n\par because it is removed during plain text scan
     ck->footer = "\n\\par ";
     ck->footer += chunk_footer;

#if TEST_OUT
     cout << "=================\nVDM_TC_TABLE:\n" << plaintext_chunk_header << 
       "\n-----------------\n" << ck->mod_name <<"\n- - - - -\n" << chunk_footer << "\n";
#endif

     // ADD
     
     chunks->Add(ck);
     break;
   }

   case CHUNK_TYPE_BLOB : {
     ChunkBLOB * ck = new ChunkBLOB;
#if TEST_OUT
     cout << "============================\nBLOB:\n" <<chunk_buffer <<"\n";
#endif
     // BODY

     ck->set_section(section_count);
     ck->blob = chunk_buffer;

     // ADD
     chunks->Add(ck);
     break;
   }
   default: break;
   };

  chunk_footer = "";
  chunk_buffer = ""; 
  section_count ++;
}

/***********************************************
 Functions for handling different chunk types
 ************************************************/

/* NAME : GenVDM
   Function used for collecting VDM ASCII
*/

static void GenVDM() {
  GenNewSection(CHUNK_TYPE_VDM,true);
}

static void GenVDM_TC_TABLE() {
  GenNewSection(CHUNK_TYPE_VDM_TC_TABLE,true);
}

/* NAME : GenVDM_IF
   Function used for collecting IF
*/

static void GenVDM_IF() {
  GenNewSection(CHUNK_TYPE_VDM_IF,true);
}

/* NAME : GenVDM_ENDIF
   Function used for collecting ENDIF
*/

static void GenVDM_ENDIF() {
  GenNewSection(CHUNK_TYPE_VDM_ENDIF,true);
}

/* NAME : GenBLOB
   Function used for colleting Blop information. I.e.
   RTF that is a section that we have to save for later
   but which contents we really do not care about 
*/

static void GenBLOB() {
  GenNewSection(CHUNK_TYPE_BLOB,false);
}

void HandleChunk() {

  /* Perform operation on the current chunk */

  if (chunk_id == style_VDM) {
    GenVDM();
  }
  else if (chunk_id == style_VDM_TC_TABLE) {
    GenVDM_TC_TABLE();
  }
  else if (chunk_id == style_VDM_IF) {
    GenVDM_IF();
  }
  else if (chunk_id == style_VDM_ENDIF) {
    GenVDM_ENDIF();
  }
  else {
    GenBLOB();
  }
}



void HandleLastChunk() {
#if 0
  if (plain_text) {

    HandleChunk();   // Handle the last plain_text chunk

    // create a BLOB chunk to hold the stuff that was thrown away

    PutLitStr("\n\\par }");
    GenBLOB();

  } else {
#endif
    HandleChunk();
    //  }
}

/*******************************************************/


/*
 * Initialize the writer.
 */

void WriterInit ()
{
  if (RTFReadOutputMap (outMap) == 0) {
    RTFPanic (L"Cannot read output map");
  }
}


int
BeginFile ()
{
  
  InitChunkList();
  
  /* install class callbacks */

  RTFSetClassCallback (rtfText, TextClass);
  RTFSetClassCallback (rtfControl, ControlClass);

  return (1);
}

/*
 * Write out a character.  rtfMajor contains the input character, rtfMinor
 * contains the corresponding standard character code.
 *
 * If the input character isn't in the charset map, try to print some
 * representation of it.
 */

static void TextClass ()
{
  if (rtfMinor != rtfSC_nothing && !wasBSlashPrimeSym) {
    PutStdChar (rtfMinor);
  }
  else {
    if (rtfMajor < 128 && !wasBSlashPrimeSym) { /* in ASCII range */
      char buf[rtfBufSiz];
      sprintf (buf, "[[%c]]", rtfMajor);
      PutLitStr (buf);
    }
    else if(rtfMajor >= 128 && !wasBSlashPrimeSym) { // Hi ANSI
      PutMBLitStr(TBWSTR::hiansi2string(rtfMajor));
    }
    else {
/*
//      char buf[rtfBufSiz];
//      wchar_t wbuf [rtfBufSiz/2];
//#ifdef _MSC_VER
//      swprintf (wbuf, L"%c", rtfMajor);
//#else
//      // No swprintf available on Linux yet
//      wostringstream ws;
//      ws << (wchar_t) rtfMajor;
//      wcscpy(wbuf, ws.str().c_str());
//#endif //_MSC_VER
//      strcpy(buf, (char*) wbuf);
//      //sprintf (buf, "[[\\'%02x]]", rtfMajor);
*/
      string str;
      str += rtfMajor;
      PutMBLitStr (str);
    }
//    PutLitStr (buf);
  }
  wasBSlashPrimeSym = false;
}

static void
ControlClass ()
{
  switch (rtfMajor) {
    case rtfDestination: {
      Destination ();
      break;
    }
    case rtfSpecialChar: {
      SpecialChar ();
      break;
    }
    case rtfParAttr: {

      /* Style change in document */
 
      if ( (rtfMinor == rtfStyleNum) || (rtfMinor == rtfParDef) ) {

        // DON`t INCLUDE THE STYLE CHANGE COMMAND in the chunk buffer

        // ??? LTO chunk_buffer_pos = last_token_end-1;

        /* handle last chunk */

        HandleChunk();

        /* Get ready for next chunk */

        plain_text_last = plain_text;
  
        if ( rtfMinor == rtfParDef ) {
          chunk_id = 0;  // defaults to Normal style on \pard ???
        }
        else {
          chunk_id = rtfParam;
        }

        /* get hold of style numbers for the non BLOB styles */
 
        if (style_VDM == -1) {
          style_VDM       = RTFGetStyleNumber(style_name_VDM);
          style_VDM_IF    = RTFGetStyleNumber(style_name_VDM_IF);
          style_VDM_ENDIF = RTFGetStyleNumber(style_name_VDM_ENDIF);
          style_VDM_COV   = RTFGetStyleNumber(style_name_VDM_COV);
          style_VDM_NCOV  = RTFGetStyleNumber(style_name_VDM_NCOV);
          style_VDM_TC_TABLE  = RTFGetStyleNumber(style_name_VDM_TC_TABLE);
        }

        /* setup according to the new style */

        reset_paragraph = 0;
        plaintext_meet = 0;
        plaintext_chunk_header = "";
      
        // HANDLE PARAGRAPH STYLES

        if (chunk_id == style_VDM) {
          plain_text = 1;
        }
        else if (chunk_id == style_VDM_TC_TABLE) {
          plain_text = 1;
        }
        else if (chunk_id == style_VDM_IF) {
          plain_text = 1;
        }
        else if (chunk_id == style_VDM_ENDIF) {
          plain_text = 1;
        }
        else {
          reset_paragraph = 1;
          plain_text = 0;
        }
      }
      break;      
    }
  }
}

/*
 * This function notices destinations that should be ignored
 * and skips to their ends.  This keeps, for instance, picture
 * data from being considered as plain text.
 */

static void Destination ()
{
  switch (rtfMinor) {
    case rtfPict:
    case rtfFNContSep:
    case rtfFNContNotice:
    case rtfInfo:
    case rtfIndexRange:
    case rtfITitle:
    case rtfISubject:
    case rtfIAuthor:
    case rtfIOperator:
    case rtfIKeywords:
    case rtfIComment:
    case rtfIVersion:
    case rtfIDoccomm:
       
    case rtfBookmarkStart:  /* LTO */
    case rtfBookmarkEnd:    /* LTO */

    case rtfListLevel:      /* LTO */
    case rtfListText: {
      RTFSkipGroup ();
      break;
    }
  }
}


/*
 * The reason these use the rtfSC_xxx thingies instead of just writing
 * out ' ', '-', '"', etc., is so that the mapping for these characters
 * can be controlled by the text-map file.
 */

static void
SpecialChar ()
{
  switch (rtfMinor) {
    case rtfPage:
    case rtfSect:
    case rtfRow:
    case rtfLine:
    case rtfPar:         { PutLitChar ('\n'); break; }
    case rtfCell:        { PutStdChar (rtfSC_space);  /* make sure cells are separated */ break; }
    case rtfNoBrkSpace:  { PutStdChar (rtfSC_nobrkspace); break; }
    case rtfTab:         { PutLitChar ('\t'); break; }
    case rtfNoBrkHyphen: { PutStdChar (rtfSC_nobrkhyphen); break; }
    case rtfBullet:      { PutStdChar (rtfSC_bullet); break; }
    case rtfEmDash:      { PutStdChar (rtfSC_emdash); break; }
    case rtfEnDash:      { PutStdChar (rtfSC_endash); break; }
    case rtfLQuote:      { PutStdChar (rtfSC_quoteleft); break; }
    case rtfRQuote:      { PutStdChar (rtfSC_quoteright); break; }
    case rtfLDblQuote:   { PutStdChar (rtfSC_quotedblleft); break; }
    case rtfRDblQuote:   { PutStdChar (rtfSC_quotedblright); break; }
  }
}

///////////////////////////////////////////////////////////
// Routines for writing text to the chunk buffer
///////////////////////////////////////////////////////////

/*
 * Eventually this should keep track of the destination of the
 * current state and only write text when in the initial state.
 *
 * If the output sequence is unspecified in the output map, write
 * the character's standard name instead.  This makes map deficiencies
 * obvious and provides incentive to fix it. :-)
 */

static void
PutStdChar (int  stdCode)
{
  const char *oStr;
  char buf[rtfBufSiz];

  if (stdCode == rtfSC_nothing) {
    RTFPanic (L"Unknown character code, logic error\n");
  }
  oStr = outMap[stdCode];
  if (oStr == (char *) NULL) { /* no output sequence in map */
    sprintf (buf, "[[%s]]", RTFStdCharName (stdCode));
    oStr = buf;
  }
  PutLitStr (oStr);
}


// Save the ascii character to the chunk_buffer if we are currently handling a
// text paragraph

static void
PutLitChar (int  c)
{
  if (plain_text) {
    plaintext_meet = 1;
    chunk_footer = "";
    chunk_buffer += c; 
  }
}


// Save the ascii wstring to the chunk_buffer if we are currently handling a
// text paragraph

void
PutLitStr (const char *s)
{
  if (plain_text) {
    plaintext_meet = 1;
    chunk_footer = "";
    chunk_buffer.append(s); 
  }
}

void
PutMBLitStr (const string& s)
{
  if (plain_text) {
    plaintext_meet = 1;
    chunk_footer = "";
    chunk_buffer += s;
  }
}

////////////////////////////////////////////
// FUNC: AddCharToChunkBuffer
// DESCR:
// If the reader is currently in the non plain text mode characters should be
// appended to the current chunk buffer directly
// If the reader is in plain text mode all charecters up to the first plain
// charecter should be inserted in the chunk header
///////////////////////////////////////////// 

void AddCharToChunkBuffer(char c)
{
  if (!plain_text) {
    chunk_buffer += c; 
  } 

  else {
    // handle the rtf that stands between the paragraph style change and 
    // the first plain char. These chars should go into the chunk header

    if (!plaintext_meet ) {
      plaintext_chunk_header += c;
    }
    else {
      chunk_footer += c;  // the potential end of the text part
    }
  }

}


