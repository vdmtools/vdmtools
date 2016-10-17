#include "FileDescriptor.h"

const wchar_t* vdm_FileDescriptor::name = L"JavaIoFileDescriptor";

#ifndef DEF_FileDescriptor_FileDescriptor

vdm_FileDescriptor::vdm_FileDescriptor ()
{  ;
}

Bool vdm_FileDescriptor::vdm_valid ()
{
  return Bool(false);
}

void vdm_FileDescriptor::vdm_sync_u_u ()
{}

#endif

