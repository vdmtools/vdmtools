#include "metaiv.h"
#include "Integer.h"
#include "Object.h"
#include "JavaString.h"
#if !DEF_quote_NIL && DECL_quote_NIL
#define DEF_quote_NIL 1
const Quote quote_NIL(L"NIL");

#endif

