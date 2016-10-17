#ifdef _MSC_VER
int errno;
#endif
#include <string>
#include <iostream>
#include "metaiv.h"

#include "corba_client.h"
//#include L"NewProxies.h"


main(int argc, wchar_t *argv[])
{
  // Some Proxy stuff to be tested:
  //
  // Has been tested 4. may 1998. The smart proxies defined in
  // NewProxies.h works well, but it must be decided if this is the
  // way we should solve the problem with having to destroy all
  // distributed VDM values. Another option is to redesign the VDM
  // values part of the interface!!!
  //

//  _new_Generic_proxyObjectFactory *f1 = new _new_Generic_proxyObjectFactory;
//  _new_Int_proxyObjectFactory *f2 = new _new_Int_proxyObjectFactory;
//  _new_Real_proxyObjectFactory *f3 = new _new_Real_proxyObjectFactory;
//  _new_Sequence_proxyObjectFactory *f4 = new _new_Sequence_proxyObjectFactory;

  VDMApplication_var app;
  init_corba(argc, argv);
  if(!get_app(app, argc, argv)){
    cerr << argv[0] << L": cannot invoke on a nil object reference.\n" << endl;
    exit(0);
  }

#if 0

  VDMFactory_var fact;
  fact = app->GetVDMFactory();
  VDM::Generic_var g;
  VDMInterpreter_var interp = app->GetInterpreter();

  g = interp->EvalExpression(L"[ e | e in set {1,...,10}]");
  if(!g->IsSequence()){
    exit(-1);
  }
  else{
    VDM::Sequence_var s = VDM::Sequence::_narrow(g);
    for(int i = 1; i <= s->Length(); i++){
      VDM::Generic_var e = s->Index(i);
      if(e->IsNumeric()){
        VDM::Numeric_var ii = VDM::Numeric::_narrow(e);
        cout << ii->GetValue() * ii->GetValue() << L" ";
      }
      e->destroy();
    }
  }
  cout << L"\n";
  Generic gg = GetCPPValue(g);
  cout << L"The original sequence: " << g->ascii() << L"\n";

  Sequence s;
  for(int jj=1;jj<10;jj++)
    s.ImpPrepend(Int(jj*jj));
  
  VDM::Generic_var from = FromCPPValue(s, fact);
  cout << L"From C++ value: " << from->ascii() << L"\n";

  g->destroy();
  exit(0);



  VDM::Generic_var res;
  VDM::Sequence_var my_seq = fact->mkSequence();

  for(int ii=1; ii< 11; ii++){
    wchar_t ex[20];
    sprintf(ex,L"%d*%d",ii,ii);
    res = interp->EvalExpression(ex);
    cout << (wchar_t*) res->ascii() << L"\n";
    my_seq->ImpPrepend(res);
    //    res->destroy();
  }
  cout << L"A sequence: " << (wchar_t*) my_seq->ascii() << L"\n";
  Sequence real_seq = GetCPPValue(my_seq);
  cout << L"A *real* sequence: " << real_seq.ascii() << L"\n";
  VDM::Generic_var from_cpp_seq;
  from_cpp_seq = FromCPPValue(real_seq, fact);
  cout << L"Yet a sequence: " << (wchar_t*) from_cpp_seq->ascii() << L"\n";  
  
  exit(0);

  //interp->Verbose(true);
//  interp->EvalCmd(L"read auml.vdm");
//  interp->EvalCmd(L"read implsort.vpp"); 
//  interp->EvalCmd(L"read sorter.vpp");
//  interp->EvalCmd(L"read explsort.vpp");
//  interp->EvalCmd(L"read mergesort.vpp");
//  interp->EvalCmd(L"read sortmachine.vpp");
//  interp->Init();
//  interp->EvalCmd(L"create o := SortMachine ! new");
//  res = interp->EvalExpression(L"o!GoSorting([3,5,2,1])");
//  cout << (wchar_t *) res->ascii() << L"\n";
//  
//  VDM::Sequence_var arg_l = fact->mkSequence();
//  arg_l->ImpAppend(my_seq);
//  res = interp->Apply(L"o!GoSorting", arg_l);
//  cout << (wchar_t *) res->ascii() << L"\n";
  

  // Get the MetaIV factory object. Used to construct VDM values in
  // the client

  // Construct a VDM sequence and print it:
//  VDM::Sequence_var ns = fact->mkSequence();
//  VDM::Int_var i_var;
//  for(int i=0;i<N;i++){
//    i_var = fact->mkInt(i);  
//    ns->ImpAppend(i_var);
//    i_var->destroy();
//  }
//  cout << L"Generated sequence: " << ns->ascii() << L"\n";
//  ns->destroy();
  
#endif

  try{
    VDMProject_var prj;  
    prj = app->GetProject();

    prj->Open(L"sort.prj")

    ModuleList_var ml;
    // Alternatively one could use ModuleList *ml and pass the pointer:
    // prj->GetModules(ml); --- Maybee???
    
    prj->GetModules(ml);
    cout << L"Modules\n";
    VDMModuleRepos_var repos;
    repos = app->GetModuleRepos();
    int i;
    for(i=0; i<ml->length(); i++){
      ModuleStatus stat;
      repos->Status(stat, ml[i]);
      cout << (int) stat.SyntaxChecked 
           << (int) stat.TypeChecked
           << (int) stat.CodeGenerated
           << (int) stat.PrettyPrinted
           << (wchar_t *)ml[i] << L"\n";
      ClassList_var cl;
      int nl = repos->Uses(cl, ml[i]);
      cout << L"Uses: ";
      for(int j=0; j<nl; j++)
        cout << (wchar_t*) cl[j] << L", ";
      cout << L"\n";
    }


    cout << L"New project...\n";
    prj->New();
    prj->AddFile(L"auml.vdm");
    prj->AddFile(L"build.vdm");  

    VDMParser_var parser = app->GetParser();
    FileList_var fl;
    prj->GetFiles(fl);
    cout << L"Parsing files individually\n";
    for(i=0; i<fl->length(); i++){
      cout << (wchar_t *)fl[i] << L"...Parsing...";
      if(parser->Parse(fl[i]))
        cout << L"done.\n";
      else
        cout << L"error.\n";
    }
    cout << L"\nParsing entire list...";
    parser->ParseList(fl);
    cout << L"done.\n";

    ErrorList_var errs;
    VDMErrors_var errhandler = app->GetErrorHandler();
    int nerr = errhandler->n_err();
    nerr = errhandler->GetErrors(errs);
    cout << nerr << L" errors:\n";
    for(int ierr=0; ierr<nerr; ierr++){
      cout << (wchar_t *) errs[ierr].fname << L", " << errs[ierr].line << L"\n" 
           << (wchar_t*) errs[ierr].msg << L"\n";
    }
    nerr = errhandler->GetWarnings(errs);
    cout << nerr << L" warnings:\n";
    for(ierr=0; ierr<nerr; ierr++){
      cout << (wchar_t *) errs[ierr].fname << L", " << errs[ierr].line << L"\n" 
           << (wchar_t*) errs[ierr].msg << L"\n";
    }

  
//    // 
//    // Excerpt form client_example.cc
//    //
//    Generic gg;
//    gg = GetCPPValue(g);
//    cout << gg.ascii();
//
//    // Problems with tokens:
//    g = interp->EvalExpression(L"mk_token({2,4,5})");
//    if(g->IsToken()){
//      VDM::Token_var tk = VDM::Token::_narrow(g);
//      cout << L"\n" << g->ascii() << L"\n";
//      cout << tk->GetValue() << L"\n";
//    }
//    if(g->IsRecord()){
//      VDM::Record_var tk = VDM::Record::_narrow(g);
//      cout << L"\n" << g->ascii() << L"\n";
//      cout << tk->GetTag() << L"\n";
//    }
//    
//    g = interp->EvalExpression(L"[ e | e in set {1,...,10}]");
//    if(!g->IsSequence()){
//      exit(-1);
//    }
//    else{
//      VDM::Sequence_var s = VDM::Sequence::_narrow(g);
//      for(int i = 1; i <= s->Length(); i++){
//        VDM::Generic_var e = s->Index(i);
//        if(e->IsReal()){
//          VDM::Real_var ii = VDM::Real::_narrow(e);
//          cout << ii->GetValue() * ii->GetValue() << L" ";
//        }
//        e->destroy();
//      }
//    }
//    // 
//    // End.
//    //


  }
  catch(APIError &ex){
    cerr << L"Caught API error " << (wchar_t*)ex.msg << L"\n";
  }  
  return 0;

  VDM::Generic_var g_var;
  VDM::Generic_var g2_var;
  VDM::Sequence_var s_var, s2_var;
  
//  Generic g = GetCPPValue(g2_var);
//  //  cout << g.ascii() << L"\n";
//  //g_var->destroy();
//  //g2_var->destroy();
//
//  // Used to hold the return value of .ascii() method
//  CORBA::String_var str;
//
//  if(g_var->IsSequence() && g2_var->IsSequence()){
//    // OK to narrow
//    s_var = VDM::Sequence::_narrow(g_var);
//    s2_var = VDM::Sequence::_narrow(g2_var);
//    // corresponds to something like: 
//    // Sequence s(g);
//
//    str = s_var->ascii();
//    //    cout << L"s_var: " << (wchar_t *) str << L"\n";
//    str = s2_var->ascii();
//    //    cout << L"s2_var: " << (wchar_t *) str << L"\n";
//    
//    // Append sequence s2_var to s_var
//    s_var->ImpAppend(s2_var);
//    str = s_var->ascii();
//    //    cout << L"ImpAppend: " << (wchar_t *) str << L"\n";
//    
//    // Remove appended element again:
//    s_var->RemElem(s_var->Length());
//    str = s_var->ascii();
//    //    cout << L"RemElem: " << (wchar_t *) str << L"\n";
//    
//    // Concat sequence s2_var in stead:
//    s_var->ImpConc(s2_var);
//    str = s_var->ascii();
//    //    cout << L"ImpConc: " <<(wchar_t *) str << L"\n";
//    
//    cout << L"Iterating through s_var:\n";
//    try{
//      int n = s_var->Length() + 1;  // Too far on purpose...
//      for(int i=1;i<=n; i++){
//        g_var = s_var->Index(i);
//        
//        str = g_var->ascii();
//        //        cout << (wchar_t *) str << L", ";
//        g_var->destroy();
//      }
//    }
//    catch(VDM::VDMError &ex){
//      cerr << L"Caught MetaIV error " << ex.err << L"\n";
//    }
//    
//  }
//  s_var->destroy();
//  s2_var->destroy();
  return 0;
}
