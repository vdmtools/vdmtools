#include <iostream>
#include <fstream>
#include <string>
#include "priority_aux.h"
#include "SCHDTP.h"

void usage()
{
  cout << L"Usage: pr_test priorityfile" << endl;
}

main(int argc, wchar_t **argv)
{
  ++argv,--argc;
  if(argc==0)
  {
    usage();
    exit(1);
  }

  init_SCHDTP();

  Set errs;

  Sequence priority_file(pf_main(&cout,argv[0]));
  cout << priority_file.ascii() << endl;
}


  
  
