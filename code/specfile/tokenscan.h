/***
*  * WHAT
*  *    This file is part of the rtf parser toolset
*  *    that has been downloaded from www
*  *    http://night.primate.wisc.edu:80/software/RTF
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/tokenscan.h,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:55 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *    
*  * PROJECT
*  *   
*  * AUTHOR
*  *    Lars T. Olsen + $Author: paulm $
*  * LOG

*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/*
 * tokenscan.h - token scanning library stuff
 */


/*
 * Flags for scanFlags field
 */

#define	tsNoConcatDelims	0x01	/* all delimiters are significant */


typedef	struct TSScanner	TSScanner;

typedef void   (*InitScannerFctPtr)(wchar_t *);
typedef const wchar_t * (*ScannerFctPtr)();

struct TSScanner
{
        InitScannerFctPtr  scanInit;
        ScannerFctPtr scanScan;
	const wchar_t *scanDelim;
	const wchar_t *scanQuote;
	const wchar_t *scanEscape;
	const wchar_t *scanEos;
	int scanFlags;
};


void	TSScanInit (wchar_t *);
const wchar_t *TSScan ();
void	TSSetScanner (TSScanner *);
void	TSGetScanner (TSScanner *);
void	TSSetScanPos (TSScanner *);
wchar_t	*TSGetScanPos ();
int	TSIsScanDelim (wchar_t);
int	TSIsScanQuote (wchar_t);
int	TSIsScanEscape (wchar_t);
int	TSIsScanEos (wchar_t);
int	TSTestScanFlags (int);
