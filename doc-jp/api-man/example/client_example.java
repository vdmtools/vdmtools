import org.omg.CORBA.*;
import java.io.*;

import jp.co.csk.vdm.toolbox.api.ToolboxClient;
import jp.co.csk.vdm.toolbox.api.corba.ToolboxAPI.*;
import jp.co.csk.vdm.toolbox.api.corba.VDM.*;

public class client_example 
{
  private static short client;
  private static VDMApplication app;

  private static final String VdmToolboxHomeWin=
      "C:\\Program Files\\The VDM-SL Toolbox v3.7.1\\examples";
  private static final String VppToolboxHomeWin=
      "C:\\Program Files\\The VDM++ Toolbox v6.7.17\\examples";
  private static final String VdmToolboxHome=
      "/home/vdm/toolbox/examples/sl";    
  private static final String VppToolboxHome=
      "/home/vdm/toolbox/examples/pp";
    
  public static void main(String args[])
  {
    try {
      //
      // ORB生成
      //

      String os = System.getProperty("os.name", "");
      String ToolboxHome = System.getProperty("TOOLBOXHOME", "");

      if (System.getProperty("VDMPP") == null) {
          app = (new ToolboxClient ()).getVDMApplication(args,
                                                         ToolType.SL_TOOLBOX);
	  if( 0 == ToolboxHome.length() ) {
            if (os.startsWith("Windows")) 
              ToolboxHome = VdmToolboxHomeWin;
            else 
              ToolboxHome = VdmToolboxHome;	 
          }
      }
      else {
          app = (new ToolboxClient ()).getVDMApplication(args,
                                                         ToolType.PP_TOOLBOX);
	  if( 0 == ToolboxHome.length() ) {
            if (os.startsWith("Windows")) 
              ToolboxHome = VppToolboxHomeWin;
            else
              ToolboxHome = VppToolboxHome;
          }
      }
      
      // Toolboxにクライアントを登録:
      
      client = app.Register();
      
      System.out.println ("registered: " + client);
            
      // 最初に、現プロジェクトを構築するために
      // VDMProjectインターフェイスに対するハンドルを獲得:
            
      try{
        VDMProject prj = app.GetProject();
        prj.New();
                
        // 必要なファイルを含めるためにプロジェクトを構築する。
        // ファイルはVDM Toolboxが始動した同じディレクトリに
        // 配置されていなければならない。それ以外の場合は
        // ファイルには絶対パスを使用すべきである

                
        if(app.Tool() == ToolType.SL_TOOLBOX){ 
          prj.AddFile(ToolboxHome + "/sort/sort.vdm");
        }
        else{
          prj.AddFile(ToolboxHome + "/sort/implsort.vpp"); 
          prj.AddFile(ToolboxHome + "/sort/sorter.vpp");
          prj.AddFile(ToolboxHome + "/sort/explsort.vpp");
          prj.AddFile(ToolboxHome + "/sort/mergesort.vpp");
          prj.AddFile(ToolboxHome + "/sort/sortmachine.vpp");
        }

        // ファイルの構文解析:
        VDMParser parser = app.GetParser();
        FileListHolder fl = new FileListHolder();
        int count = prj.GetFiles(fl);
        String flist[] = fl.value;
        
        // ２つの異なる方法でファイルを構文解析する。第一は、
        // ファイル一覧を検討し各ファイルを個別に構文解析する。
        // (もちろん、SL_TOOLBOXに対しては構築ファイルが唯１つであるが、
        // 説明にはこれで十分であろう)
              
        System.out.println("Parsing files individually");
        for(int i=0; i<flist.length; i++){
          System.out.println(flist[i]);
          System.out.println("...Parsing...");
          if(parser.Parse(flist[i]))
            System.out.println("done.");
          else
            System.out.println("error.");
        }
              
        // そしてその後すべてのファイルを一度に構文解析:
       
        System.out.println("Parsing entire list...");
        parser.ParseList(flist);
        System.out.println("done.");
        
        // 構文解析中にエラーが起きた場合には
        // ここで調査:

        // エラーハンドラー

        VDMErrors errhandler = app.GetErrorHandler();  
        
        ErrorListHolder errs = new ErrorListHolder();
        // エラー列を取り出す
        int nerr = errhandler.GetErrors(errs); 
         jp.co.csk.vdm.toolbox.api.corba.ToolboxAPI.Error errlist[] = 
           errs.value;
        if(nerr>0){
        　// エラーの印刷:
         System.out.println("errors: ");
          for(int i=0; i<errlist.length; i++){
            System.out.println(errlist[i].fname);
            System.out.println(errlist[i].line);
            System.out.println(errlist[i].msg);
          }
        }
              
        // 同様に警告を問い合わせることもできる。
       
        // すべてのモジュールの名称と状態を一覧:
       ListModules(app);
     
        // すべてのモジュールの型チェック:

        VDMTypeChecker tchk = app.GetTypeChecker();
        ModuleListHolder moduleholder = new ModuleListHolder();
        prj.GetModules(moduleholder);
        String modules[] = moduleholder.value;
        System.out.println("Type checking all modules...");
        if(tchk.TypeCheckList(modules))
          System.out.println("done.");
        else
          System.out.println("errors.");
            
        // すべてのモジュールの最新状態の一覧:
       ListModules(app);
              
              
        // 最後にインタープリターの使用法を示す。 
        
        System.out.println("Interpreter tests:");
              
        VDMInterpreter interp = app.GetInterpreter();

              
        // 素数計算をする関数の呼び出し:
        EchoPrimes(20, interp, app);

        // 第２の方法として問合せを用いる方法を示す:
        // ソートされるべき整数の列を構築する。このために
        // VDM値を作り出すためのVDMFactoryに対するハンドルを求める:
    
        VDMFactory fact = app.GetVDMFactory();
              
        app.PushTag(client); 
        // これ以降は生成されたオブジェクトすべてにタグ付けする
       
        VDMSequence list = fact.MkSequence(client);
        
        VDMNumeric elem;
        for(int j=0; j<20; j++){
          elem = fact.MkNumeric(client, j);
          list.ImpPrepend(elem);
        }
              
        System.out.println("The sequence to be sorted: " + 
                         list.ToAscii());

        // 呼び出しのための引数の一覧の構築。これは
        // 右並びに全引数を含めるSequenceの構築である:
       VDMSequence arg_l = fact.MkSequence(client);
              
        arg_l.ImpAppend(list);

        // ユーザーインターフェイスでインタープリターを用いる結果を
        // 表示するため、Verbose をtrueに設定:
             
        interp.Verbose(true);
        interp.Debug(true);
    
        // 始めにインタープリターを初期化する
       System.out.println("About to initialize the interpreter");
        interp.Initialize();
              
        VDMGeneric g;
        if(app.Tool() == ToolType.SL_TOOLBOX){
          g = interp.Apply(client, "MergeSort", arg_l);
        }
        else{ // PP_TOOLBOX
        　// 最初は、中心となるソートオブジェクの生成:
          interp.EvalCmd("create o := new SortMachine()");
         
        　// 次は、このオブジェクト上で GoSorting メソッドを呼び出し:
         g = interp.Apply(client, "o.GoSorting", arg_l);
        }

        System.out.println("The sorted sequence: " + g.ToAscii());
              
        // 最後に、返された列挙を通して列挙中の全要素の
        // 合計を計算するための繰り返し:

        VDMSequence s = VDMSequenceHelper.narrow(g);
              
        VDMGenericHolder eholder = new VDMGenericHolder();
          
        int sum=0;
        for (int ii=s.First(eholder); ii != 0; ii=s.Next(eholder)) {
          VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
          sum = sum + GetNumeric( num );
        }

        System.out.println("The sum of all the elements: " +  sum);
        
        EchoPrimes2(50, interp, app);
        
        app.DestroyTag(client);

        app.Unregister(client);
        System.exit(0);
      }
      catch(APIError err) {
        System.err.println("API error"+err.getMessage ());
        System.exit(1);
      }
    }
    catch
      (jp.co.csk.vdm.toolbox.api.ToolboxClient.CouldNotResolveObjectException ex) 
      {
        System.err.println(ex.getMessage());
        System.exit(1);
      }
    catch(COMM_FAILURE ex) {
      System.err.println(ex.getMessage());
      ex.printStackTrace();
      System.exit(1);
    }
  };

  public static void ListModules(VDMApplication app){
        
    try{
      // この関数はモジュールとそれらの状態を一覧にする。
      // プロジェクトハンドル
        
      VDMProject prj = app.GetProject();
      
      // モジュールリポジトリ
      VDMModuleRepos repos = app.GetModuleRepos();
          
      ModuleListHolder ml = new ModuleListHolder();
      prj.GetModules(ml);
      String mlist[] = ml.value;
      System.out.println("Modules:");
          
      for(int i=0; i<mlist.length; i++){
              
     　 // この構造はモジュール状態の保持に使用される:
       ModuleStatusHolder stateholder = new ModuleStatusHolder();
        // i番目のモジュールの状態を得る
        repos.Status(stateholder, mlist[i]);  
        ModuleStatus stat = stateholder.value;
        
        // 状態を印刷する。
        System.out.println(mlist[i]);
        System.out.println("SyntaxChecked: " + stat.SyntaxChecked);
        System.out.println("TypeChecked: " + stat.TypeChecked);
        System.out.println("Code generated: " + stat.CodeGenerated);
        System.out.println("PrettyPrinted: " + stat.PrettyPrinted);
      }
    }
    catch(APIError err) {
      System.err.println("API error");
      System.exit(1);
    }
  }
    

  public static void EchoPrimes(int n, VDMInterpreter interp, 
                                  VDMApplication app)
  {
    try{
      // ｎ以下の素数の列を生成し
      // それをstdoutにそのまま送り返す。
        
      app.PushTag(client);
            
      // この Generic はインタープリターからの結果を保持することに
      // 用いる。
      VDMGeneric g;
        
      // 20以下の素数を計算するのにはEvalExpressionを用いる
           
      String expr = "[e|e in set {1,...,"+n+"} &"+
                    " exists1 x in set {2,...,e} & e mod x = 0]";
      g = interp.EvalExpression(client,expr);
            
      if(g.IsSequence()){
        System.out.println("All primes below " + n + ": " + 
                           g.ToAscii());
      }
            
      VDMSequence s = VDMSequenceHelper.narrow(g);
      
      VDMGenericHolder eholder = new VDMGenericHolder();
            
      int sum=0;
      for (int ii=s.First(eholder); ii != 0; ii=s.Next(eholder)) {
        VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
        sum = sum + GetNumeric( num );
      }
      System.out.println("The sum of all the primes: " + sum);
      app.DestroyTag(client); // 仕上げ...
   }
    catch(APIError err) {
      System.err.println("API error");
      System.exit(1);
    }
  }
    
  public static void EchoPrimes2(int n, VDMInterpreter interp, 
                                 VDMApplication app)
  {
    // ｎ以下の素数の列を生成し
    // その列をstdoutへそのまま送り返す。  
  　// 追加として、VDM値全体をtoolboxからクライアント側へ移行し
    // これをjp.co.csk.vdm.toolbox.VDMに宣言されている
  　// "real"のJava値に変換するために
  　// どうGetCPPValueを使用できるか、をこの関数が示している
        
    try{
      app.PushTag(client);
      
      // この VDMGeneric はインタープリターからの結果の保持に 
      // 使用される。
      VDMGeneric g;
        
      // EvalExpressionを使用し20以下の素数を計算する
            
      String expr = "[e|e in set {1,...,"+n+"} &" +
                    " exists1 x in set {2,...,e} & e mod x = 0]";
      g = interp.EvalExpression(client,expr);
        
      if(g.IsSequence()){
        System.out.println("All primes below " + n + ": " + g.ToAscii());
      }
            
      VDMSequence s = VDMSequenceHelper.narrow(g);
            
      // real Java VDM 値に変換!
            
      java.util.LinkedList sj = 
          new java.util.LinkedList ();
            
      VDMGenericHolder eholder = new VDMGenericHolder();
            
      // Generic g を "real" Java Sequence 値に変換する
            
      for (int ii=s.First(eholder); ii != 0; ii=s.Next(eholder)) {
        VDMNumeric num = VDMNumericHelper.narrow(eholder.value);
        sj.add(Integer.valueOf( GetNumeric( num ) ));
      }
            
      int sum=0;
      for (java.util.Iterator itr = sj.iterator(); 
           itr.hasNext();){
          Integer i = (Integer) itr.next ();
          sum = sum + i.intValue();
      }
      
      System.out.println("The sum of all the primes: " + sum);
      app.DestroyTag(client); // 仕上げ...
    }
    catch(APIError err) {
      System.err.println("API error");
      System.exit(1);
    }
  }

  public static int GetNumeric( VDMNumeric num )
  {
    byte[] b1 = num.GetCPPValue();
    try
    {
      InputStream is = new ByteArrayInputStream( b1 );
      int type = is.read(); 
      int c = -1;
      int last = -1;
      String str = "";
      while( true )
      {
        c = is.read();
        if ( ( c == -1 ) || ( c == ',' ) )
        {
          last = c;
          break;
        }
        str += Character.toString( (char)c );
      }
      return Integer.parseInt( str );
    }
    catch( Exception e )
    {
      return 0;
    }
  } 
}






