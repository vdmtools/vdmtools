/***
*  * WHAT
*  *    Test driver for timefile parser (VICE)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/vice/test/tp_test.cc,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 2005/01/21 03:16:06 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <iostream>
#include <fstream>
#include <string.h>
#include "timeparser_aux.h"
#include "TIMEMAP.h"
#include "TIMEPARSER.h"

Quote Integer2Quote(Int I)
{
  const wchar_t* key;
  switch(I.GetValue()){
  case TAG_quote_BOOLEAN: { key = L"BOOLEAN"; break;}
  case TAG_quote_NAT:     { key = L"NAT"; break;}
  case TAG_quote_NATONE:  { key = L"NATONE"; break;}
  case TAG_quote_INTEGER: { key = L"INTEGER"; break;}
  case TAG_quote_RAT:     { key = L"RAT"; break;}
  case TAG_quote_REAL:    { key = L"REAL"; break;}
  case TAG_quote_CHAR:    { key = L"CHAR"; break;}
  case TAG_quote_TOKEN:   { key = L"TOKEN"; break;}
  }

  if (key != (wchar_t *) 0 ) return(Quote(key));
    
  cerr << L"Integer2Quote: UNDEFINED QUOTE (" << I << ")\n";
  return Quote (L"UNDEFINED");
}

Map TransformMap (Map m)
{
  // m : map (TIMEPARSER`Instruction * [REP`BasicTypeRep] to nat)
  Generic Element;
  Map mr;
  for (int i = m.First (Element); i; i = m.Next (Element))
  {
    Tuple ot(Element);
    if (!ot.GetField(2).IsNil())
    {
      Record rep(ot.GetField(2));
      rep.SetField(1, Integer2Quote(rep.GetField(1)));
      Tuple nt(2);
      nt.SetField(1, ot.GetField(1));
      nt.SetField(2, rep);
      mr.Insert(nt, m[Element]);
      
    }
    else
      mr.Insert (Element, m [Element]);
  }
  return mr;
}

void usage()
{
  cout << L"Usage: tp_test timefile [output_file]" << endl;
}

main(int argc, wchar_t **argv)
{
  ++argv,--argc;
  if(argc==0)
  {
    usage();
    exit(1);
  }

  Set errs;
  TYPE_TIMEMAP_Timemap timemap;

  init_TIMEPARSER();
  init_TIMEMAP();
  init_REP();

  if(tp_main(&cout,argv[0],errs,timemap))
  {

    // Transform map so that quote tags are replaced by quote values
    Map m = TransformMap(timemap);


    wchar_t *of;
    if (argc==2) {
      // Use given output file name
      *of = (wchar_t *) malloc(strlen(argv[1]));
      strcpy(of, argv[1]);
    }
    else {
      // Build output file name based on input file name
      *of = (wchar_t *) malloc(strlen(argv[0])+4);
      strcpy(of, argv[0]);
      strcpy(&of[strlen(argv[0])], L".vdm");
    }

    // Open output stream
    ofstream of_stream(of);
    if(!of_stream){
      cerr << L"Unable to open " << of << " for writing. " << endl;
      exit(1);
    }

    // VDM-SL header
    of_stream << L"module TIMEMAPVAL" << endl << endl;
    of_stream << L"exports all" << endl << endl;
    of_stream << L"definitions " << endl << endl;
    of_stream << L"values " << endl << endl;

    // Create value called L"tm"
    of_stream << L"tm = {" << endl;

    // Iterate over every maplet
    Generic g;
    int j = 0;
    for (int i = m.First(g); i; i = m.Next(g))
    {
      of_stream << L"       " << g.ascii();
      of_stream << L" |-> " << m[g].ascii();
      j++;

      // Make sure we output maplet separators correctly
      if (j != m.Size())
        of_stream << L",";
      of_stream << endl;
    }
    of_stream << L"     };" << endl << endl;

    // End module
    of_stream << L"end TIMEMAPVAL" << endl;
  }
  else
    tp_OutputErrors(cout, errs);

}


