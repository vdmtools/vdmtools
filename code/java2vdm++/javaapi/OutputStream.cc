#include "OutputStream.h"
#ifdef DEF_OutputStream_USERIMPL

#endif

vdm_OutputStream::vdm_OutputStream ()
{
}

#ifndef DEF_OutputStream_close

void vdm_OutputStream::vdm_close ()
{  
  ;
}

#endif

#ifndef DEF_OutputStream_flush

void vdm_OutputStream::vdm_flush ()
{
  ;
}

#endif

#ifndef DEF_OutputStream_write

void vdm_OutputStream::vdm_write (const type_iiM &vdm_b)
{
  vdm_write((Generic) vdm_b, (Generic) (Int) 0, (Generic) (Int) vdm_b.Dom().Card());
}

#endif

#ifndef DEF_OutputStream_write

void vdm_OutputStream::vdm_write (const type_iiM &vdm_b, const Int &vdm_off, const Int &vdm_len_u_u)
{
  ;
}

#endif

