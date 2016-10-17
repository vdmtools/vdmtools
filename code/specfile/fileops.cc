/***
*  * WHAT
*  *    This file is part of the rtf parser toolset
*  *    that has been downloaded from www
*  *    http://night.primate.wisc.edu:80/software/RTF
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/fileops.cc,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:54 $
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
 * fileops.c -- file operations for UNIX versions of translators.
 */
/*
 * UnixOpenLibFile () - Open a library file.  Looks in the following
 * directories:
 * - Looks in current directory.
 * - if RTFLIBDIR environment variable is set, looks in directory named by it.
 * - Looks in executable program's directory, if UnixSetProgPath() has
 * been called.
 * - Looks in library directory, LIBDIR.
 *
 * Exception: if file is an absolute pathname, look only for file as named.
 *
 * Returns NULL if file cannot be found and opened.
 */
 
# include	<stdio.h>
# include	<string.h>
# include	<stdlib.h>
 
# include	"rtf.h"
# include	"rtf-unix.h"
# include "reader.h"
 
static wchar_t	*progPath = (wchar_t *) NULL;
 
 
void
UnixSetProgPath (wchar_t	*path)
{
int	i, j, n;

	n = strlen (path);
	for (j = -1, i = 0; i < n; i++)
	{
		if (path[i] == '/')
			j = i;
	}
	if (j < 0)		/* no slash found */
	{
		path = L".";
		j = 1;
	}
	if ((progPath = RTFAlloc (j + 1)) != (wchar_t *) NULL)
	{
		(void) strncpy (progPath, path, j);
		progPath[j] = '\0';
	}
}
 
 
FILE * UnixOpenLibFile (wchar_t	*file,
                        wchar_t	*mode)
{
FILE	*f;
char	buf[rtfBufSiz];
char	*p;
 
	if ((f = fopen (file, mode)) != (FILE *) NULL)
	{
		return (f);
	}
	/* if abolute pathname, give up, else look in library */
	if (file[0] == '/')
	{
		return ((FILE *) NULL);
	}
	if ((p = getenv (L"RTFLIBDIR")) != (wchar_t *) NULL)
	{
		sprintf (buf, L"%s/%s", p, file);
		if ((f = fopen (buf, mode)) != (FILE *) NULL)
			return (f);
	}
	if (progPath != (wchar_t *) NULL)
	{
		sprintf (buf, L"%s/%s", progPath, file);
		if ((f = fopen (buf, mode)) != (FILE *) NULL)
			return (f);
	}

#define LIBDIR "."

	sprintf (buf, L"%s/%s", LIBDIR, file);
	f = fopen (buf, mode);	/* NULL if it fails */
	return (f);
}



