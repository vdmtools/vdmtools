/***
*  * 概要
*  *    このファイルはVDM ToolboxのCORBA APIを用いて 
*  *    クライアント処理を実装する方法の例題である。
*  *                                                                       
*  *    このファイルは windows NT/95上のMS VC++ 6.0　および 
*  *    Unix上の gcc 2.95.2 でコンパイル可能である。
*  *                                                                       
*  *    98/NT上でnmakeを使用する場合は makefileにMakefile.nm を用い      
*  *    Linux上でコンパイルする場合はMakefile を用いる
*  * ID
*  *    $Id: client_example.cc,v 1.27 2006/02/07 05:14:11 vdmtools Exp $
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <iostream>
using namespace std;

#include <string>
// CORBA 初期化 および omniORB4のための他のこと
#include "corba_client.h"

#ifdef _MSC_VER
#include <direct.h> // getcwdのため
#else
#include <unistd.h> // getcwdのため
#endif // _MSC_VER

char ABS_FILENAME[200];
VDM::ClientID client_id;

#define ADD_PATH(p,s) strcat(strcpy(ABS_FILENAME, p), s)
#define SORT_NUM 20

void EchoPrimes(int, ToolboxAPI::VDMInterpreter_var, 
                ToolboxAPI::VDMApplication_var);
void EchoPrimes2(int, ToolboxAPI::VDMInterpreter_var, 
                 ToolboxAPI::VDMApplication_var);
void ListModules(ToolboxAPI::VDMApplication_var app);

int main(int argc, char *argv[])
{
  const char * source_path = getenv("VDM_SOURCE_LOCATION");
  string sdir;
  if( source_path == NULL )
  {
    char buf[1024];
    if( getcwd( buf, sizeof( buf ) ) != NULL )
    {
#ifdef _MSC_VER
      // Unix上では
      // パス中のバックスラッシュはフォワードスラッシュに変換する。
     for (char* s = buf; s = strchr(s, '¥¥'); s++) {
        *s = '/';
      }
#endif
      sdir = buf;
      for( int i = 0; i < 2; i++ )
      {
        unsigned int index = sdir.find_last_of( '/' );
        if( index == string::npos ) break;
        sdir = sdir.substr( 0, index );
      }
      sdir += "/";
    }
    source_path = sdir.c_str();

    cerr << "Environment variable VDM_SOURCE_LOCATION not set" << endl;
    cerr << "Default location: " << source_path << endl;
  }
  
  //  VDM Toolboxに対する主ハンドル:
  ToolboxAPI::VDMApplication_var app;

  // ORBの初期化。これがどのように行われるかについての詳細は
  // corba_client.{h,cc}とomniORB3 ユーザーマニュアルを参考に。
  init_corba(argc, argv);

  // 最後にスタートしたVDMToolboxに対するハンドルを取り出す。
  // ハンドルはVDM Toolboxにより生成された
  // CORBAオブジェクトの文字列表示を通して獲得する。
  // 文字列はファイル名の付いたobject.stringとして書き出され
  // VDM_OBJECT_LOCATIONにより定義されたディレクトリに置かれる。
  // これが設定されない場合、get_app は自動的にホーム（Unix）または 
  // プロファイルディレクトリ（Windows NT/95）にファイルを探しに行く。

#ifdef VDMPP
  GetAppReturnCode rt = get_app(app, NULL, ToolboxAPI::PP_TOOLBOX);
#else
  GetAppReturnCode rt = get_app(app, NULL, ToolboxAPI::SL_TOOLBOX);
#endif //VDMPP
  switch(rt){
  case VDM_OBJECT_LOCATION_NOT_SET:
    cerr << "Environment variable VDM_OBJECT_LOCATION not set" << endl;
    exit(0);
  case OBJECT_STRING_NON_EXISTING:
      cerr << "The file " + GetIORFileName() + " could not be located. ¥
               Make sure the Toolbox is running" << endl;
    exit(0);
  case CORBA_ERROR:
    cerr << "Unable to setup the CORBA environment" << endl;
    exit(0);
  case CORBA_SUCCESS:
  default:
    break;
  }

  try{
    // Toolboxにクライアントを登録:
   client_id = app->Register();

    // 現プロジェクトを構築するために、
    // VDMProjectインターフェイスに対するハンドルを最初に獲得:
    ToolboxAPI::VDMProject_var prj = app->GetProject();

    prj->New();  // 新規プロジェクト

    // 必要なファイルを含めるために、プロジェクトを構築する。
    // ファイルはVDM Toolboxがスタートした同じ場所に
    // 配置されなければならない。それ以外の場合は、
    // ファイルに対する絶対パスを用いるべきである
    if(app->Tool() == ToolboxAPI::SL_TOOLBOX)
      {
        prj->AddFile(ADD_PATH(source_path,"examples/sort/sort.vdm"));
      }
    else{
      prj->AddFile(ADD_PATH(source_path,"examples/sort/implsort.vpp")); 
      prj->AddFile(ADD_PATH(source_path,"examples/sort/sorter.vpp"));
      prj->AddFile(ADD_PATH(source_path,"examples/sort/explsort.vpp"));
      prj->AddFile(ADD_PATH(source_path,"examples/sort/mergesort.vpp"));
      prj->AddFile(ADD_PATH(source_path,"examples/sort/sortmachine.vpp"));
    }
    // ファイルを構文解析:
    ToolboxAPI::VDMParser_var parser = app->GetParser();
    ToolboxAPI::FileList_var fl;
    prj->GetFiles(fl);

    // ２つの違った方法でファイルを構文解析する。
    // 第一の方法ではファイル一覧を１つ１つ検討し
    // 各ファイルを個別に構文解析する
    // （もちろん、SL_TOOLBOXに対しては構築ファイルが唯１つであるが、
    // 説明ではこれで十分であろう)
    cout << "Parsing files individually" << endl;
    for(unsigned int i=0; i<fl->length(); i++){
      cout << (char *)fl[i] << "...Parsing...";
      if(parser->Parse(fl[i]))
        cout << "done." << endl;
      else
        cout << "error." << endl;
    }
    
    // そしてその後すべてのファイルを一度に構文解析:
    cout << "¥nParsing entire list...";
    parser->ParseList(fl);
    cout << "done." << endl;
    
    // 構文解析中にエラーが起きた場合には
    // ここで調査:
    ToolboxAPI::VDMErrors_var errhandler = app->GetErrorHandler();  
    // エラーハンドラー
    ToolboxAPI::ErrorList_var errs;

    // エラー列を取り出す
    int nerr = errhandler->GetErrors(errs); 
    if(nerr){
      // エラーの印刷:
      cout << nerr << " errors:" << endl;
      for(int ierr=0; ierr<nerr; ierr++)
        cout << (char *) errs[ierr].fname << ", " 
             << errs[ierr].line << endl 
             << (char *) errs[ierr].msg << endl;

    }
    // 同様に警告を問い合わせることもできる。

    // すべてのモジュールの名称と状態を一覧:
    ListModules(app);

    // すべてのモジュールの型チェック:
    ToolboxAPI::VDMTypeChecker_var tchk = app->GetTypeChecker();
    ToolboxAPI::ModuleList_var modules;
    prj->GetModules(modules);
    cout << "Type checking all modules...";
    if(tchk->TypeCheckList(modules))
      cout << "done." << endl;
    else
      cout << "errors." << endl;

    // すべてのモジュールの最新状態の一覧:
    ListModules(app);


    // 最後にインタープリターの使用法を示す。 

    cout << endl << "Interpreter tests:" << endl << endl;
    ToolboxAPI::VDMInterpreter_var interp = app->GetInterpreter();


    // 素数計算をする関数の呼び出し:
    EchoPrimes(20, interp, app);

    // 第２の方法として問合せを用いる方法を示す:
    // ソートされるべき整数の列を構築する。このために
    // VDM値を作り出すためのVDMFactoryに対するハンドルを求める:
    VDM::VDMFactory_var fact = app->GetVDMFactory();

    app->PushTag(client_id); 
    // これ以降は生成されたオブジェクトすべてにタグ付けする

    VDM::VDMSequence_var list = fact->MkSequence(client_id);
    VDM::VDMNumeric_var elem;
    for(int j=0; j<SORT_NUM; j++){
      elem = fact->MkNumeric(client_id, j);
      list->ImpPrepend(elem);
    }
    cout << "The sequence to be sorted: " << list->ToAscii() << endl;

    // 呼び出しのための引数の一覧の構築。これは
    // 右並びに全引数を含めるVDM::Sequenceの構築である:
    VDM::VDMSequence_var arg_l = fact->MkSequence(client_id);
    arg_l->ImpAppend(list);

    // ユーザーインターフェイスでインタープリターを用いる結果を
    // 表示するため、Verbose をtrueに設定:
    interp->Verbose(true);
    interp->Debug(true);
    
    // 始めにインタープリターを初期化する
    interp->Initialize();

    VDM::VDMGeneric_var g;
    if(app->Tool() == ToolboxAPI::SL_TOOLBOX){
      g = interp->Apply(client_id, "MergeSort", arg_l);
    }
    else{ // PP_TOOLBOX
      // 最初は、中心となるソートオブジェクの生成:
      interp->EvalCmd("create o := new SortMachine()");
      
      // 次は、このオブジェクト上で GoSorting メソッドを呼び出し:
      g = interp->Apply(client_id, "o.GoSorting", arg_l);
    }

    cout << "The sorted sequence: " << g->ToAscii() << endl;

    // 最後に、返された列を通して列中の全要素の
    // 合計を計算するための繰り返し:

    VDM::VDMSequence_var s = VDM::VDMSequence::_narrow(g);
    int sum=0;
    for(int k=1; k<=s->Length(); k++){
      VDM::VDMNumeric_var n = VDM::VDMNumeric::_narrow(s->Index(k));
      sum += (Int(GetCPPValue(n))).GetValue();
    }
    cout << "The sum of all the elements: " << sum << endl;

    EchoPrimes2(50, interp, app);

    app->DestroyTag(client_id);

    // クライアントの登録を解除:
    app->Unregister(client_id);
  }
  catch(ToolboxAPI::APIError &ex){
    cerr << "Caught API error " << (char *)ex.msg << endl;
  }
  catch(CORBA::COMM_FAILURE &ex) {
    cerr << "Caught system exception COMM_FAILURE, ¥
             unable to contact server" 
         << endl;
  }
  catch(omniORB::fatalException& ex) {
    cerr << "Caught omniORB3 fatalException" << endl;
  }

  return 0;  
}


void EchoPrimes(int n, ToolboxAPI::VDMInterpreter_var interp, 
                ToolboxAPI::VDMApplication_var app)
  // ｎ以下の素数の列を生成し、
  // それをstdoutにそのまま送り返す。
{
  app->PushTag(client_id);

  interp->Initialize ();

  // この VDM::Generic はインタープリターからの結果の保持に
  // 用いられる。
  VDM::VDMGeneric_var g;
  
  // 20以下の素数を計算するためにEvalExpressionを用いる
  char expr[200];
  sprintf(expr, "[e|e in set {1,...,%d} ¥
                  & exists1 x in set {2,...,e} & e mod x = 0 ]", n);
  g = interp->EvalExpression(client_id, expr);
  if(g->IsSequence()){
    cout << "All primes below " << n << ":" << endl << g->ToAscii() << endl;
  }
  VDM::VDMSequence_var s = VDM::VDMSequence::_narrow(g);
  int sum=0;
  for(int k=1; k<=s->Length(); k++){
    VDM::VDMNumeric_var n = VDM::VDMNumeric::_narrow(s->Index(k));
    sum += (Int(GetCPPValue(n))).GetValue();
  }
  cout << "The sum of all the primes: " << sum << endl;
  app->DestroyTag(client_id); // 仕上げ...
}

void EchoPrimes2(int n, ToolboxAPI::VDMInterpreter_var interp, 
                 ToolboxAPI::VDMApplication_var app)
  // ｎ以下の素数の列を生成し
  // その列をstdoutにそのまま送り返す。
  // 追加として、VDM値全体をtoolboxからクライアント側へ移行し
  // これをmetaiv.hに宣言されている"real"のC++値に変換するために
  // どうGetCPPValueを使用できるか、をこの関数が示している
{
  // この VDM::VDMGeneric はインタープリターからの結果を保持することに 
  // 用いる。
  VDM::VDMGeneric_var g;
  
  // 20以下の素数を計算するのにはEvalExpressionを用いる
  char expr[200];
  sprintf(expr, "[e|e in set {1,...,%d} & ¥
                  exists1 x in set {2,...,e} & e mod x = 0 ]", n);
  g = interp->EvalExpression(client_id, expr);

  // VDM::Generic g を"real" metaiv-Sequence値に
  // 変換する:
  Sequence s(GetCPPValue(g));
  
  // ここで値全体がクライアント側へ移されたので安全に g を
  // 破棄することができる:
  g->Destroy();

  cout << "All primes below " << n << ":" << endl
       << wstring2string(s.ascii()) << endl;
  int i, sum=0;
  Generic gg;
  for(i = s.First(gg); i; i = s.Next(gg)){
    sum += (int)Real(gg).GetValue();
  }
  cout << "The sum of all the primes: " << sum << endl;
}

        
void ListModules(ToolboxAPI::VDMApplication_var app)
  // この関数はモジュールとそれらの状態を一覧にする。
{
 // プロジェクトハンドル
  ToolboxAPI::VDMProject_var prj = app->GetProject();

  // モジュールリポジトリ
  ToolboxAPI::VDMModuleRepos_var repos = app->GetModuleRepos();

  ToolboxAPI::ModuleList_var ml;
  prj->GetModules(ml);
  cout << "Modules:" << endl;
  for(unsigned int i=0; i<ml->length(); i++){
    // この構造はモジュール状態の保持に使用される:
    ToolboxAPI::ModuleStatus stat; 
    // i番目のモジュールの状態を得る
    repos->Status(stat, ml[i]);  
    // 状態を印刷する。 0 = none, 1 = OK
    cout << (int) stat.SyntaxChecked 
         << (int) stat.TypeChecked
         << (int) stat.CodeGenerated
         << (int) stat.PrettyPrinted
         << " " << (char *)ml[i] << endl;
  }
}


