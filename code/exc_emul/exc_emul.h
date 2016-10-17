/*
 *  WHAT
 *     Module for emulation of C++ exceptions using setjmp/longjmp.
 *  IDENTIFICATION
 *     $Id: exc_emul.h,v 1.7 1999/09/08 11:33:52 hanne Exp $
 *  AUTHOR
 *     ETN
 *  COPYRIGHT
 *     (C) Kyushu University
 */


#ifndef EXC_EMUL_H_

#include <setjmp.h>

extern jmp_buf exc_dummy_buf; 
#ifdef __GNUC__
typedef typeof(exc_dummy_buf[0]) jmp_buf_type;
#else

#ifndef JMP_BUF_BASIC
#ifdef __alpha
#define JMP_BUF_BASIC long
#else
#define JMP_BUF_BASIC int
#endif // __alpha
#endif
typedef JMP_BUF_BASIC jmp_buf_type; 

#endif

extern jmp_buf_type *EXC_allocjmpbuf();
extern void EXC_try_done(int);
extern void EXC_throw(int ignored); 
extern int  EXC_get_top_id();
/* This variable is for general use. */

class EXC_ErrType {
public:
  int ExType;
  EXC_ErrType() : ExType(0) {}
  EXC_ErrType(int err) : ExType(err) {}
  void set(int err) { ExType = err; }
}; 


#ifndef USE_REAL_EXCEPTIONS 

extern EXC_ErrType EXC_Error;

#define EXC_TRY         if (setjmp(EXC_allocjmpbuf())==0) { volatile int EXC_top=EXC_get_top_id(); 
#define EXC_TRY_DONE    EXC_try_done(EXC_top)
#define EXC_CATCH       } else
#define EXC_THROW(error)   EXC_throw((EXC_Error.set(error),1))
#define EXC_PROTECT volatile 

# else 


#define EXC_TRY         try
#define EXC_TRY_DONE    do {} while (0)
#define EXC_CATCH       catch (EXC_ErrType EXC_Error)
#define EXC_THROW(error) throw (EXC_ErrType(error))


#endif
//////////////////////////////////////////////////
// This function can be used as parameter to VDMSetErrorHandler
extern
void EXC_ThrowHandler(int error, int exi);

//////////////////////////////////////////////////

#endif //EXC_EMUL_H
  
