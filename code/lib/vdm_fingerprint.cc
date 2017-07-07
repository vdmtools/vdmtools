/***
*  * WHAT
*  *    Implementation of Fingerprint Function
*  * FILE
*  *    $Source:$
*  * VERSION
*  *    $Revision:$
*  * DATE
*  *    $Date:$
*  * FORMAT
*  *    $State:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author:$
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifdef VDM_FINGERPRINT
#include "metaiv.h"

unsigned long Fingerprint::crcTable[256];

/****************************************************************************/
unsigned long Fingerprint::getcrc(const wchar_t *p, int len)
{
  unsigned long crc;
  int c;

  crc = 0x0;
  while(len-->0) {
    c=*p++;
    crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ];
  }
  return(crc);
}

void Fingerprint::continue_crc(unsigned long & crcinout, const wchar_t *p, int len)
{
  unsigned long crc = crcinout;
  int c;

  while(len-->0) {
    c=*p++;
    crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ];
  }
  crcinout = crc;
}

void Fingerprint::continue_crc_on_crc(unsigned long & crc, unsigned long crcdata)
{
  wchar_t a[4];
  a[0] = (crcdata >>= 0) & 0xFF;
  a[1] = (crcdata >>= 8) & 0xFF;
  a[2] = (crcdata >>= 8) & 0xFF;
  a[3] = (crcdata >>= 8) & 0xFF;
  continue_crc(crc, a, 4);
}

/****************************************************************************/
void Fingerprint::crcgen( )
{
  unsigned long crc, poly;
  int i, j;

  poly = 0xEDB88320L;
  for (i=0; i<256; i++) {
    crc = i;
    for (j=8; j>0; j--) {
      if (crc&1) {
        crc = (crc >> 1) ^ poly;
      } else {
        crc >>= 1;
      }
    }
    crcTable[i] = crc;
  }
}

//======================================================================

Fingerprint::Fingerprint()
{
  value = 0;
  valid_fingerprint = false;
}

int Fingerprint::Compare(const Fingerprint& fp2) const
{
  return value < fp2.value ? -1 : (value > fp2.value ? 1 : 0);
}

void Fingerprint::init_crc()
{
  value = 0;
}

void Fingerprint::continue_crc(const void *p, int sz)
{
  this->continue_crc(value, (const wchar_t*)p, sz);
}

void Fingerprint::continue_crc_on_crc(const Fingerprint& fp)
{
  this->continue_crc_on_crc(value, fp.value);
}

void Fingerprint::setvalid(bool b)
{
  this->valid_fingerprint = b;
}

bool Fingerprint::valid() const
{
  return this->valid_fingerprint;
}

void Fingerprint::done_crc()
{
}
#endif // VDM_FINGERPRINT
