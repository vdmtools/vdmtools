/***
*  * WHAT
*  *    This file is part of the rtf parser toolset
*  *    that has been downloaded from www
*  *    http://night.primate.wisc.edu:80/software/RTF
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/reader.h,v $
*  * VERSION
*  *    $Revision: 1.10 $
*  * DATE
*  *    $Date: 2006/04/26 05:50:09 $
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

#ifndef _INCL_RTF_READER
#define _INCL_RTF_READER

#include <string.h>
#include <stringext.h>
#include <fstream>

// public 

/*
 * Public RTF reader routines
 */

void		RTFInit ();
void		RTFSetStream (ifstream *);
void		RTFSetInputName (wchar_t *);
wchar_t		*RTFGetInputName ();
void		RTFSetOutputName (wchar_t *);
wchar_t		*RTFGetOutputName ();
void		RTFSetClassCallback (int, RTFFuncPtr);
RTFFuncPtr	RTFGetClassCallback (int);
void		RTFSetDestinationCallback (int,RTFFuncPtr);
RTFFuncPtr	RTFGetDestinationCallback ();
void		RTFRead ();
int		RTFGetToken ();	/* writer should rarely need this */
void		RTFUngetToken ();
int		RTFPeekToken ();
void		RTFSetToken (int,int,int,int,const char *);
void		RTFSetReadHook (void (*f)());
RTFFuncPtr	RTFGetReadHook ();
void		RTFRouteToken ();
void		RTFSkipGroup ();
void		RTFExpandStyle (int);
bool		RTFCheckCM (int,int);
bool		RTFCheckCMM (int,int,int);
bool		RTFCheckMM (int,int);
RTFFont		*RTFGetFont (int);
RTFColor	*RTFGetColor (int);
RTFStyle	*RTFGetStyle (int);
int             RTFGetStyleNumber (const char *);
int             RTFGetFontNumByCharset (const wstring &);
int             RTFGetFontNumByCharset (int);
# define	RTFAlloc(size)	_RTFAlloc ((int) size)
char		*_RTFAlloc (int);
wchar_t		*RTFWStrSave (wchar_t *);
char		*RTFStrSave (char *);
void		RTFWFree (wchar_t *);
void		RTFFree (char *);
int		RTFCharToHex (wchar_t);
int		RTFHexToChar (wchar_t);
void		RTFSetMsgProc (RTFFuncPtr);
void		RTFSetPanicProc (RTFFuncPtr);
int RTFGetCurFontNum();
int RTFGetNonAsciiFontNum();

int RTFReadOutputMap (const char *outMap[]);

void RTFSetOpenLibFileProc (FILE *(*proc) (wchar_t *, wchar_t *) );

FILE	*RTFOpenLibFile (wchar_t *,wchar_t *);

/*
 * Information pertaining to last token read by RTFToken.  The
 * text is exactly as it occurs in the input file, e.g., L"\{"
 * will be found in rtfTextBuf as L"\{", even though it means "{".
 * These variables are also set when styles are reprocessed.
 */

//extern wchar_t	*rtfTextBuf;		/* text of token */
extern int	rtfTextLen;		/* length of token in rtfTextBuf */
extern int	rtfClass;		/* token class */
extern int	rtfMajor;		/* token major number */
extern int	rtfMinor;		/* token minor number */
extern int	rtfParam;		/* control symbol parameter */

# ifdef THINK_C
# define	rtfNoParam	(-32768)	/* 16-bit max. neg. value */
# endif
# ifndef rtfNoParam
# define	rtfNoParam	(-1000000)
# endif

extern long	rtfLineNum;		/* input line number */
extern int	rtfLinePos;		/* input line position */

extern bool wasBSlashPrimeSym;          /* the curent symbol is \'xx */ 


#endif


