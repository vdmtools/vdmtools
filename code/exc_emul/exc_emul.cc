/*
 *  WHAT
 *     Module for emulation of C++ exceptions using setjmp/longjmp.
 *  IDENTIFICATION
 *     $Id: exc_emul.cc,v 1.6 2001/06/12 15:04:21 paulm Exp $
 *  AUTHOR
 *     ETN
 *  COPYRIGHT
 *     (C) Kyushu University
 */

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <setjmp.h>
#include <memory.h>

#include "exc_emul.h"

static wchar_t *RCSID = L"$Header: /home/vdmtools/cvsroot/toolbox/code/exc_emul/exc_emul.cc,v 1.6 2001/06/12 15:04:21 paulm Exp $";


static int EXC_curidx;

class EXC_Buf {
public:
  class EXC_Buf *next;
  jmp_buf JmpBuf;
  int curidx;              
};

static EXC_Buf * EXC_last_buf; 

EXC_ErrType EXC_Error;

int   EXC_get_top_id()
{
  return EXC_curidx;
}

jmp_buf exc_dummy_buf;

// __typeof__(exc_dummy_buf[0])
jmp_buf_type * EXC_allocjmpbuf()
{
  if (EXC_last_buf && EXC_last_buf->curidx != EXC_curidx) { 
    fprintf(stderr, L"Fatal: bad use detected in EH, exiting.\n");
    exit(1);
  }
  EXC_Buf *bp = new EXC_Buf();
  bp->next = EXC_last_buf;
  bp->curidx = ++EXC_curidx;
  EXC_last_buf = bp; 
  return bp->JmpBuf; 

}

void EXC_try_done(int top)
{
  if (!EXC_last_buf || EXC_last_buf->curidx != top || top!=EXC_curidx) {
    fprintf(stderr, L"Fatal: bad use detected in EXC_try_done, exiting.\n");
    exit(1);
  }
  EXC_curidx--;
  EXC_Buf *eb = EXC_last_buf; 
  EXC_last_buf = EXC_last_buf->next;
  delete eb; 
}


void EXC_throw(int error)
{
  EXC_Buf *bp;
  jmp_buf jb;
  if (!EXC_last_buf || error==0) {
    fprintf(stderr, L"Fatal: bad use detected in EXC_throw, exiting.\n");
    exit(1);
  }
  bp = EXC_last_buf;
  EXC_last_buf = EXC_last_buf->next;
  EXC_curidx--; 
  memcpy(jb, bp->JmpBuf, sizeof(jb));
  delete bp; 
  longjmp(jb,error); 
}
  
//////////////////////////////////////////////////
void
EXC_ThrowHandler(int error, int exi)
{
  EXC_THROW(error);
}

