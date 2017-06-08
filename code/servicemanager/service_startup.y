/*
$Id: service_startup.y,v 1.5 2005/01/21 03:16:06 vdmtools Exp $
*/

%{
#include <iostream>
#include <string.h>
#include "metaiv.h"
#include "ServiceManagerDesc.h"
#include "tb_wstring.h"

#define YYDEBUG 1

extern FILE *smd_in;
extern int smd_error(const char*s);
extern int smd_lex();
extern void smd_reset();
extern wostream* smd_err_stream;
extern int smd_lineno_local;
extern int smd_debug;


static Sequence* smd = new Sequence;

%}

%union {
  char* charp;
  int val;
  TYPE_ServiceManagerDesc_ServiceDescription *record;
  Sequence *sequence;
  type_cL *charSeq;
  type_cLL *charSeqSeq;
}

%token LEX_BEGIN_DESCRIPTION LEX_END_DESCRIPTION
%token LEX_BEGIN_NAME LEX_END_NAME
%token LEX_BEGIN_LOCATION LEX_END_LOCATION
%token <charp> LEX_FILEPATH
%token <charp> LEX_NAME
%token <charp> LEX_CHAR
%type <record> serviceDescription
%type <sequence> service_startup_file serviceDescriptions
%type <charSeq> serviceName
%type <charSeqSeq> serviceLocation
%type <charSeqSeq> filepath;


%%

service_startup_file:
                      {}
                    | serviceDescriptions
                      { *smd = *$1; }
                    ;

serviceDescriptions:  serviceDescription
                      { $$ = new Sequence;
                        $$->ImpAppend(*$1);
                        delete $1;
                      }
                    | serviceDescriptions serviceDescription
                      { $1->ImpAppend(*$2); 
                        delete $2;
                      }
                    ;
serviceDescription: LEX_BEGIN_DESCRIPTION serviceName serviceLocation LEX_END_DESCRIPTION
                    { $$ = new TYPE_ServiceManagerDesc_ServiceDescription;
                      $$->Init(*$2, *$3);
                      delete $2;
                      delete $3;
                    }
                   ;

serviceName:        LEX_BEGIN_NAME LEX_NAME LEX_END_NAME
                    { 
                      Sequence *s = new Sequence(TBWSTR::string2wstring($2));
                      $$ = new type_cL((Generic)*s);
                      delete $2;
                      delete s;
                    }
                   ;  

serviceLocation:    LEX_BEGIN_LOCATION '/' filepath LEX_END_LOCATION
                    { 
                      $$ = $3;
                    }
               |    LEX_BEGIN_LOCATION LEX_CHAR ':' '/' filepath LEX_END_LOCATION
                    { 
                      $$ = $5;
                      Char c (*$2);
                      type_cL s;
                      s.ImpAppend(c);
                      s.ImpAppend(Char(':'));
                      $$->ImpPrepend(s);
                      
                    }
               |    LEX_BEGIN_LOCATION  LEX_NAME LEX_END_LOCATION
                    { 
                      $$ = new type_cLL;
                      type_cL *s = new type_cL((Generic) Sequence(TBWSTR::string2wstring($2)));
                      $$->ImpAppend(*s);
                      delete s;
                      delete $2;
                    }

filepath:           LEX_NAME
                    { $$ = new type_cLL;
                      type_cL *s = new type_cL((Generic) Sequence(TBWSTR::string2wstring($1)));
                      $$->ImpAppend(*s);
                      delete $1;
                    }
                  | filepath '/' LEX_NAME
                    { type_cL *s = new type_cL((Generic) Sequence(TBWSTR::string2wstring($3)));
                      $$ = $1;
                      $$->ImpAppend(*s);
                      delete $3;                      
                    }

%%

// It is assumed that fname exists
Sequence smd_main(wostream *o, const wchar_t *fname)
{
  smd_in = fopen(TBWSTR::wstring2fsstr(fname).c_str(),"r");
  smd_lineno_local = 1;
  smd_err_stream = o;

  //  smd_debug = 1;
  int parse_result = smd_parse();
  fclose(smd_in);
  smd_in = (FILE*) 0;
  smd_reset();

  if(!(parse_result == 0))
    return Sequence();
  else
    return *smd;
}


/*  main(int argc, char **argv) */
/*  { */
/*    ++argv,--argc; */
/*    if(argc==0) */
/*    { */
/*      exit(1); */
/*    } */

/*    Set errs; */

/*    Sequence smd_file(smd_main(&cout,argv[0])); */
/*    cout << smd_file.ascii() << endl; */
/*  } */


  

                 
 
