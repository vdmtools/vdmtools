/***
*  * WHAT
*  *    Preprocesser ifs to check that the right lm_code.h was used.
*  * FILE
*  *    $Id: flexlmcheck.h,v 1.3 2001/06/12 15:04:23 paulm Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef FLEXLMCHECK_H_
#define FLEXLMCHECK_H_

#if !(defined(ENCRYPTION_SEED1) || defined(ENCRYPTION_SEED2) || defined(ENCRYPTION_CODE_1) || defined(ENCRYPTION_CODE_2))
#error Please include lm_code.h before including this file so the symbols ENCRYPTION_SEED1 and ENCRYPTION_SEED2 are defined
#error or (for HP, Solaris, SunOS) ENCRYPTION_CODE_1 or ENCRYPTION_CODE_2
#endif

#if defined(ENCRYPTION_SEED1) && ENCRYPTION_SEED1 !=  0xA04591F3
#error ENCRYPTION_SEED1 is wrong. Please fix lm_code.h and rebuild the FLEXlm libraries and utilities
#endif

#if defined(ENCRYPTION_SEED2) && ENCRYPTION_SEED2 !=  0xFCD3857F
#error ENCRYPTION_SEED2 is wrong. Please fix lm_code.h and rebuild the FLEXlm libraries and utilities
#endif

#if defined(ENCRYPTION_CODE_1) && ENCRYPTION_CODE_1 !=  0xA04591F3
#error ENCRYPTION_SEED1 is wrong. Please fix lm_code.h and rebuild the FLEXlm libraries and utilities
#endif

#if defined(ENCRYPTION_CODE_2) && ENCRYPTION_CODE_2 !=  0xFCD3857F
#error ENCRYPTION_SEED2 is wrong. Please fix lm_code.h and rebuild the FLEXlm libraries and utilities
#endif

#endif //FLEXLMCHECK_H_
