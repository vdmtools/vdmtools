#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "IO_userimpl-sl.cc"
#endif
#ifdef VDMPP
#define INCLFILE "IO_userimpl-pp.cc"
#endif

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
