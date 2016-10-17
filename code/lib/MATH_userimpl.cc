#undef INCLFILE
#ifdef VDMSL
#define INCLFILE "MATH_userimpl-sl.cc"
#endif
#ifdef VDMPP
#define INCLFILE "MATH_userimpl-pp.cc"
#endif

#if !defined(VDMSL) && !defined(VDMPP)
#error "VDMSL and VDMPP not defined"
#endif

#include INCLFILE
