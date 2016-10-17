//
// AS backend - reads an AS value from a file and generates a file
// representing this AS value
// $Id: j2vbe.cc,v 1.8 2005/05/26 23:59:20 vdmtools Exp $
//

#include "AS.h"
#include "CI.h"
#include "REP.h"
#include "astaux.h"
#include "as2ascii.h"
#include "cg.h"

#include <fstream>
#include <sys/stat.h>

#ifdef _MSC_VER
#include <direct.h>             // For mkdir
#endif

void vdm_CI_SetTypeInfo (const TYPE_CI_ContextId & cid, 
                         const TYPE_REP_TypeRep & rep)
{
}

Generic /*Record*/ vdm_CI_GetTypeInfo (const TYPE_CI_ContextId & cid)
{
  return Generic();
}

Bool vdm_CI_HasTypeInfo(const TYPE_CI_ContextId &vdm_CI_cid) {
  return false;
}

Bool vdm_CI_IsAValidContextId(const TYPE_CI_ContextId &vdm_CI_cid) {
    return true;
}


void usage() {
  cerr << "Usage: j2vbe [-r] [-d <path>] <AS file>" << endl;
  cerr << "  -r: Generate rtf file (default ascii)" << endl;
  cerr << "  -d <path>: an existing directory where to place";
  cerr << "             generated vpp files (default current)" << endl;
  exit(1);
}

int main(int argc, char* argv[]) {
  bool generateRTF = false;

  init_AS();
  init_CI();
  init_REP();

  wstring argfile;
  wstring outputDir=L".";

  bool mustShowUsage=false;

  //if (argc == 2)
  //  argfile = argv[1];
  //  else if ((argc ==3) && (wstring(argv[1]) == L"-r")) {
  //  argfile = argv[2];
  //  generateRTF = true;
  if (argc >=2) {
    for(int i=1; i<argc && !mustShowUsage; i++) {
      string arg (argv[i]);
      if (arg.compare("-r")==0) {
	generateRTF = true;
      } else if (arg.compare("-d")==0) {
	i++;
	if(i>=argc) {
	  mustShowUsage=true;
          continue;
	} else {
	  outputDir=string2wstring(argv[i]);
	}
      } else {
	argfile = string2wstring(arg);
      }
    }
  }
  
  mustShowUsage = mustShowUsage || argfile.length()==0;

  if(mustShowUsage) usage();

  Generic astval;
  bool b = CGUTIL::ParseVDMValue(argfile.c_str(), astval);
  if (!b) {
    cerr << "Unable to parse " << wstring2mbstr(argfile) << endl;
    exit(1);
  }

  if (!astval.IsMap()) {
    cerr << "Unexpected value - expected a map" << endl;
    exit(1);
  }

  Map vdmFiles (astval);
  Generic gDom, gRng;
  for (int i = vdmFiles.First(gDom, gRng); i; i = vdmFiles.Next(gDom, gRng)) {
    TYPE_AS_SpecFile sf (gRng);
    TYPE_CI_TokenContextInfo tci (sf.get_tokenci());
    Sequence classes (sf.get_vdm());

//  wstring packageName=Token(sf.get_name()).GetValue();
//  if(packageName.compare(L"nil")==0) packageName.erase();

    wstring packageName;
    packageName.erase();
    if(!sf.get_name().IsNil()) {
      Sequence s = sf.get_name(); 
      packageName=s.GetString();
    }

    wstring packageDir(outputDir+L"/"+packageName);
    string  pd = wstring2mbstr(packageDir);
#ifndef _MSC_VER
    mkdir(pd.c_str(), 0777);
#else
    mkdir(pd.c_str()); 
#endif
    Generic g;
    for (int j = classes.First(g); j; j = classes.Next(g)) {
      TYPE_AS_Class cls (g);
      TYPE_AS_Name nm (cls.get_nm());
      Sequence ids (nm.get_ids());
      Sequence clnm (ids.Hd());
      wstring clsName;
      clnm.GetString(clsName);
      wostream* os;

      wstring fileName(outputDir + L"/" + packageName + L"/" + clsName + L".vpp");
      string  fn = wstring2mbstr(fileName);
      if (generateRTF)
        os = (wostream*)NULL; // There is no wostrstream!!! new ostrstream();
      else
//        os = (ostream*) new ofstream(wstring(clsName + L".vpp").c_str());
	 os = (wostream*) new wofstream(fn.c_str());
      AS2ASCII conv;
//      cout << L"Writing " << clsName << ".vpp ..." << endl;
      cout << "Writing " << fn << endl;
      conv.BeginDocument(*os);
      conv.Class2ASCII(cls, *os, tci);
      conv.EndDocument(*os);
      string cnm = wstring2mbstr(clsName);
      cout << "Finished " << cnm << ".vpp" << endl;
      if (generateRTF){
 //         ContextInfo temp_context_info;
//          SpecFile sf(L".", clsName + ".rtf", temp_context_info);
//          if( ! sf.init() ) {
//            cerr << L"  Warning: Couldn't create file '" << sf.get_short_name() 
//                 << L"'. Check that this file does not exist already.\n";
//          }
//          else {
//            wstring s(((ostrstream*) os)->str(), ((ostrstream*) os)->pcount());
//            ((ostrstream*) os)->rdbuf()->freeze(0);
//            sf.set_merged_classes(s);
      
//            wstring err;
//            if(!sf.file_create(err)) {
//              cerr << L" Error occurred for file \"" 
//                   << sf.get_short_name() << L"\". " << err << "\n" << flush;
//            }
//          }
      }
      else
        ((wofstream*) os)->close();
    }
  }
}






