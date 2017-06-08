//
// File:        ToolboxInterface.java
// Description: Implements the interface between the Cash dispenser GUI
//              and the running VDM Toolbox executing the specification
// Date:        $Date: 2006/01/05 05:43:51 $
//
// Copyright Kyushu University

import java.io.*;
import java.awt.*;
import java.text.*;
import javax.swing.*;
import org.omg.CORBA.*;
import jp.vdmtools.api.*;
import jp.vdmtools.api.corba.ToolboxAPI.*;
import jp.vdmtools.api.corba.VDM.*;

public class ToolboxInterface {

  // MEMBERS:

  // An array representing the different pin codes for the various accounts.
  private final int PINS[] =
  {1234, 5678, 9012, 3456, 7890, 1357, 2469};

  // An array of cards to insert into the cash dispenser(s)
  private VDMGeneric[] cards = new VDMGeneric[7];

  // References used for communication with the VDM Toolbox via CORBA
  private VDMApplication app;
  private VDMInterpreter interp;
  private static short client;
  private String ref; // Reference to object.string

  // Variable telling us whether we are executing a VDM-SL specification
  // or a VDM++ specification: false = PP, true = SL
  private boolean ppOrSl = false;

  // A set used to keep track of the tills that are active
  private java.util.HashSet activeTills = new java.util.HashSet();

  // Some convenient string constants
  final private String slpath = "vdmsl";
  final private String[] pppath = {"vdm++","test"};
  final private String projectfile = "cashdispenser.prj";
  final private String projectfileUnix = "cashdispenserUnix.prj";
  final private String slSource = "vdmsl-model.rtf";
  final private String centralResource = "centRes";
  final private String clock = "clock";
  final private String letterBox = "letterBox";

  // A file in which communications are logged (useful for debugging purposes)
  private FileWriter commLog;


  // INNER CLASSES:

  // An inner class to help keep track of multiple tills
  public class VDMTill {
    VDMGeneric t;
    String nm;

    public VDMTill(VDMGeneric nt) {
      t = nt;
    }

    public VDMTill(VDMGeneric nt, String nnm) {
      t = nt;
      nm = nnm;
      activeTills.add(this);
    }

    public VDMGeneric getVal() {
      return t;
    }

    public String getName() {
      return nm;
    }

    public void delete() {
      if (!ppOrSl) {
        activeTills.remove(this);
        try {
          t.Destroy();
          EvalCmd("destroy " + nm);
        } catch (APIError e) {}
      }
    }

  }

  // METHODS:

  /**
   * A method to cleanly terminate communications. Objects created
   * within the Toolbox are destroyed.
   */

  public void close()
  {
    ToolboxClient toolboxClient = new ToolboxClient ();
    try {
      if (ppOrSl)
        toolboxClient.getVDMApplication(new String[] {}, ToolType.SL_TOOLBOX);
      else
        toolboxClient.getVDMApplication(new String[] {}, ToolType.PP_TOOLBOX);
      if (!ppOrSl) {
        int i;
        //PP
        EvalCmd("destroy " + clock);
        EvalCmd("destroy " + letterBox);
        EvalCmd("destroy " + centralResource);

        for (i=1; i < 8; i++)
          EvalCmd("destroy Card" + i);
        for (i=1; i < 6; i++)
          EvalCmd("destroy Holder" + i);
        for (i=1; i < 6; i++)
          EvalCmd("destroy acc" + i);

        java.util.Iterator iter;
        for (iter = activeTills.iterator(); iter.hasNext();)
          ((VDMTill) (iter.next())).delete();
      }
      app.DestroyTag(client);
    } catch (ToolboxClient.CouldNotResolveObjectException e) {
    } catch (RuntimeException e) {// Toolbox already shutdown
    } catch (APIError e) {}
    try {
      commLog.close();
    } catch (IOException e) {}

  }

  /**
   * The constructor opens the connection with the Toolbox API, reads
   * in the VDM model and initializes the data structures used for the
   * prototype.
   */
  public ToolboxInterface(String type) throws APIError, VDMError {


  ToolboxClient toolboxClient = new ToolboxClient ();

    // Connect to Toolbox API
    ppOrSl = (type.equals("sl") || type.equals("SL"));
    try {
      //ref = ToolboxClient.readRefFile();
      if (ppOrSl == true)
        app = toolboxClient.getVDMApplication(new String[]{},
                                              ToolType.SL_TOOLBOX);
      else
        app = toolboxClient.getVDMApplication(new String[]{},
                                              ToolType.PP_TOOLBOX);
    } catch (RuntimeException e)  {
        java.lang.Object[] options = { "OK"};
        String home = System.getProperty("user.home");
        String sep = System.getProperty("file.separator");
        String mesg = "Unable to connect to Toolbox - " + home + "\\ " +
                      sep + ".\n" +
                      "Make sure a Toolbox is running " +
                      "then restart Cash Dispenser gui";
        JOptionPane.showOptionDialog(null, mesg, "Error",
                                     JOptionPane.DEFAULT_OPTION,
                                     JOptionPane.ERROR_MESSAGE,
                                     null, options, options[0]);
        System.exit(0);
    } catch (ToolboxClient.CouldNotResolveObjectException e) {
        java.lang.Object[] options = { "OK"};
        String mesg = "Unable to resolve Application Object.\n" +
                      "Make sure a Toolbox is running " +
                      "then restart Cash Dispenser gui";
        JOptionPane.showOptionDialog(null, mesg, "Error",
                                     JOptionPane.DEFAULT_OPTION,
                                     JOptionPane.ERROR_MESSAGE,
                                     null, options, options[0]);
        System.exit(0);
    }
    client = app.Register();
    app.PushTag(client);

    // Load project containing cash dispenser specification
    VDMProject prj = app.GetProject();
    String workdir = System.getProperty ("user.dir");
    String path;
    if(!ppOrSl) {
        path = workdir;
        for (int i = 0; i < pppath.length; i++)
//          path = path + System.getProperty("file.separator") + pppath[i];
          path = path + "/" + pppath[i];
//        path = path + System.getProperty("file.separator");
        path += "/";
        String prjfile;
//        if ((System.getProperty("os.name").equals("Windows NT")) ||
//            (System.getProperty("os.name").equals("Windows 2000")))
        if (System.getProperty("os.name").startsWith("Windows"))
        {
          prjfile = projectfile;
        }
        else
          prjfile = projectfileUnix;
        prj.Open(path + prjfile);
        }
    else
        {
          path = workdir + System.getProperty("file.separator") + slpath +
            System.getProperty("file.separator");
          prj.New ();
          prj.AddFile (path + slSource);
        }


    // Check that Toolbox with which connection has been made is of
    // the correct type
    ToolType toolType = app.Tool();
    if(((toolType == ToolType.SL_TOOLBOX) && !ppOrSl) ||
       ((toolType == ToolType.PP_TOOLBOX) && ppOrSl)) {
      String mesg1, mesg2;
      if (ppOrSl){
        mesg1 = "VDM-SL";
        mesg2 = "VDM++";
      }
      else
        {
          mesg1 = "VDM++";
          mesg2 = "VDM-SL";
        }
      java.lang.Object[] options = { "OK"};
      String mesg = mesg1 + " model requested, but " + mesg2 +
        " Toolbox started\n" +
        "Restart correct Toolbox or select correct model\n" +
        "Click Ok to close application.\n";
      JOptionPane.showOptionDialog(null, mesg, "Error",
                                   JOptionPane.DEFAULT_OPTION,
                                   JOptionPane.ERROR_MESSAGE,
                                   null, options, options[0]);
      System.exit(0);
    }

    // Initialize communications log
    try {
      commLog = new FileWriter("commLog");
    } catch (IOException e) {
    }

    // Get list of files in project
    FileListHolder fl = new FileListHolder();
    prj.GetFiles(fl);
    String flist[] = fl.value;

    // Open progress bar for file input
    JProgressBar progressBar = new JProgressBar(0, flist.length+ 21);
    progressBar.setValue(0);
    progressBar.setStringPainted(true);
    JLabel status = new JLabel("Initializing...",
                               (int)Component.CENTER_ALIGNMENT);
    JPanel panel = new JPanel();
    panel.add(progressBar);

    JFrame frame = new JFrame("Progress");
    frame.getContentPane().setLayout(new BorderLayout());
    frame.getContentPane().add(panel, BorderLayout.NORTH);
    frame.getContentPane().add(status, BorderLayout.CENTER);
    frame.pack();
    frame.setVisible(true);

    // Parse files in project
    VDMParser parser = app.GetParser();
    for(int i=0; i<flist.length; i++){
      status.setText("Parsing specification");
      progressBar.setValue(i);
    }
    if (ppOrSl)
        parser.Parse (path + slSource);

    // Start up Toolbox interpreter
    interp = app.GetInterpreter();
    interp.Verbose(true);
    interp.DynPreCheck(true);
    interp.Initialize ();

    // If specification is VDM++, create specific objects in interpreter
    if (!ppOrSl){
      EvalCmd("create " + clock + " := new Clock()");
      java.util.Date date = java.util.Calendar.getInstance().getTime();
      jp.vdmtools.api.corba.VDM.VDMFactory fact = app.GetVDMFactory();
      String dateString = DateFormat.getDateInstance(DateFormat.MEDIUM).format(date);
      VDMText txtDate = fact.MkText(client, dateString);
      VDMSequence args = fact.MkSequence(client);
      args.ImpAppend(txtDate);
      Apply (client, clock + ".SetDate",args);

      EvalCmd("create " + letterBox + " := new Letterbox()");
      EvalCmd("create " + centralResource +
              " := new CentralResource()");
      EvalExpression (client, centralResource + ".Create(" + clock + ","
                                       + letterBox + ")");
    }

    // Create accounts and cards
    createAccounts(progressBar, status);
    createCards(progressBar, status);
    frame.setVisible(false);
  }

  /**
   * Creates a new till. For VDM++ this also creates a Till object at
   * the specification level.
   */

  public VDMTill getTill(String nm) throws APIError, VDMError
  {
    VDMGeneric t = null;
    if (!ppOrSl){
      EvalCmd("create " + nm + " := new Till().Create(" +
              centralResource + ")");
      t = EvalExpression(client, nm);
    }
    return new VDMTill(t,nm);
  }

  /**
   * Auxiliary function used within class
   */

  private int convertCardStringToId(String card){
    String idString = card.substring(5);
    int id = Integer.parseInt(idString);
    return id;
  }


  /**
   * Makes a card illegal. Sends a message to the specification that
   * a card has been made illegal.
   */
  public void reportIllegalCard(String card) throws APIError, VDMError
  {
    int id = convertCardStringToId(card);
    VDMFactory fact = app.GetVDMFactory();
    VDMNumeric numCardId = fact.MkNumeric(client, id);
    VDMSequence args = fact.MkSequence(client);
    args.ImpAppend(numCardId);
    if (ppOrSl)
      Apply(client,"ReportIllegalCard", args);
    else
      Apply(client,centralResource + ".AddIllegalCard", args);
  }

  /**
   * Inserts a card into a till. Sends a message to the specification
   * that the given card has been inserted into the given till.
   */
  public void insertCard(VDMTill till, String card) throws APIError, VDMError
  {
    int id = convertCardStringToId(card);
    if (ppOrSl) {
      VDMFactory fact = app.GetVDMFactory();
      VDMSequence args = fact.MkSequence(client);
      args.ImpAppend(cards[id-1]);
      Apply(client,"InsertCard", args);
    }
    else {
      String cardStr = VDMTextHelper.narrow(cards[id-1]).GetValue();
      execMethod(till.getName(), "InsertCard", cardStr);
    }
  }

  /**
   * Executes an arbitrary method for a given object at the
   * specification level. This is only meaningful for the VDM++
   * specification.
   */
  private void execMethod(String obj, String meth, String args)
      throws APIError, VDMError
  {
    EvalExpression(client, obj + "." + meth + "(" + args + ")");
  }

  /**
   * Requests the return of a card from a till. Sends a message to the
   * specification that card in the given till should be returned.
   */
  public void returnCard(VDMTill till) throws APIError, VDMError
  {
    if (ppOrSl) {
      VDMFactory fact = app.GetVDMFactory();
      VDMSequence args = fact.MkSequence(client);
      Apply(client,"ReturnCard", args);
    }
    else
      execMethod(till.getName(), "ReturnCard", "");
  }

  /**
   * Enters a PIN code into a till. Sends a message in clear text to
   * the specification that the given PIN code has been entered into
   * the given till.
   */
  public String enterPIN(VDMTill till, String pin) throws APIError, VDMError
  {
    VDMFactory fact = app.GetVDMFactory();
    VDMNumeric numPin = fact.MkNumeric(client, Integer.parseInt(pin));
    VDMSequence args = fact.MkSequence(client);
    args.ImpAppend(numPin);
    VDMGeneric t = null;
    if (ppOrSl)
      t = Apply(client,"Validate", args);
    else
      t = Apply(client,till.getName() + ".Validate", args);

    return (VDMQuoteHelper.narrow(t).GetValue());
  }

  /**
   * Gets the balance for the card currently in the till.
   */
  public int getBalance(VDMTill till) throws APIError, VDMError
  {
    VDMSequence args = app.GetVDMFactory().MkSequence(client);
    VDMGeneric t;
    if (ppOrSl)
      t = Apply(client,"GetBalance", args);
    else
      t = Apply(client,till.getName() + ".GetBalance", args);

    if (t.IsNumeric()){
//      int n = (int) VDMNumericHelper.narrow(t).GetValue();
      int n = GetNumeric( VDMNumericHelper.narrow(t) );
      return n;
    }
    else {
      return 0;
    }
  }

  /**
   * Makes a cash withdrawal. Requests a withdrawal of the given
   * amount at the given till.
   */
  public boolean makeWithdrawal(VDMTill till, String amount) throws APIError, VDMError
  {
    VDMFactory fact = app.GetVDMFactory();
    String amtNum = amount.substring(7);
    VDMNumeric numAmt = fact.MkNumeric(client, Integer.parseInt(amtNum));

    java.util.Date date = java.util.Calendar.getInstance().getTime();
    String dateString = DateFormat.getDateInstance(DateFormat.MEDIUM).format(date);
    VDMText txtDate = fact.MkText(client, dateString);
    VDMSequence args = fact.MkSequence(client);
    args.ImpAppend(numAmt);
    VDMGeneric t;
    if (ppOrSl) {
      args.ImpAppend(txtDate);
      t = Apply(client,"MakeWithdrawal", args);
    }
    else
      t = Apply(client,till.getName() + ".MakeWithdrawal", args);
    if (t.IsBool()){
      boolean b = VDMBoolHelper.narrow(t).GetValue();
      return b;
    }
    else {
      return false;
    }
  }

  /**
   * Requests a statement. A new window will be opened showing the
   * statement.
   */

  public Stmt requestStatement(VDMTill till) throws APIError, VDMError
  {
    Stmt result = new Stmt();
    VDMFactory fact = app.GetVDMFactory();
    VDMSequence args = fact.MkSequence(client);
    if (ppOrSl)
      {
        VDMGeneric g = Apply(client,"RequestStatement", args);
        if (g.IsTuple()){
          VDMTuple t = VDMTupleHelper.narrow(g);
          VDMGeneric f1 = null, f2 = null, f3 = null;
          try {
            f1 = t.GetField(1);
            f2 = t.GetField(2);
            f3 = t.GetField(3);
          } catch (VDMError e) { System.out.println("Exception");}
          if (f1.IsSequence() && f2.IsSequence() && f3.IsNumeric())
            result = makeStatement(VDMSequenceHelper.narrow(f1),
                                   VDMSequenceHelper.narrow(f2),
                                   VDMNumericHelper.narrow(f3));
        }
      }
    else {
      Apply(client,till.getName() + ".RequestStatement", args);
      VDMGeneric f1 = EvalExpression(client, letterBox +
                                     ".GetLastStatement().name");
      VDMGeneric f2 = EvalExpression(client, letterBox +
                                     ".GetLastStatement().transactions");
      VDMGeneric f3 = EvalExpression(client, letterBox +
                                     ".GetLastStatement().balance");
      if (f1.IsSequence() && f2.IsSequence() && f3.IsNumeric())
        result =  makeStatement(VDMSequenceHelper.narrow(f1),
                                VDMSequenceHelper.narrow(f2),
                                VDMNumericHelper.narrow(f3));
    }
    return result;
  }

  /**
   * Auxiliary function for creating a statement.
   */

  private Stmt makeStatement(VDMSequence name, VDMSequence trans, VDMNumeric bal)
      throws APIError, VDMError
  {
    StringHolder nameHolder = new StringHolder();
    String nm = "";
    if (name.GetString(nameHolder))
      nm = nameHolder.value;
    Stmt stmt = new Stmt(nm, (int) bal.GetValue());
    VDMGenericHolder g = new VDMGenericHolder();
    for (int i = trans.First(g); i != 0; i = trans.Next(g))
      {
        try {
          if(g.value.IsRecord())
            {
              VDMRecord r = VDMRecordHelper.narrow(g.value);
              VDMGeneric gDate = r.GetField(1);
              VDMGeneric gCardId = r.GetField(2);
              VDMGeneric gAmt = r.GetField(3);
              if (gDate.IsSequence() && gCardId.IsNumeric() &&
                  gAmt.IsNumeric())
                {
                  StringHolder dateHolder = new StringHolder();
                  VDMSequenceHelper.narrow(gDate).GetString(dateHolder);
                  String date = dateHolder.value;
//                  int cardId = (int)VDMNumericHelper.narrow(gCardId).GetValue();
//                  int amt = (int)VDMNumericHelper.narrow(gAmt).GetValue();
                  int cardId = GetNumeric(VDMNumericHelper.narrow(gCardId));
                  int amt = GetNumeric(VDMNumericHelper.narrow(gAmt));
                  Stmt.Transaction t = stmt.new Transaction(date, cardId,
                                                            amt);
                  stmt.AddTransaction(t);
                }
            }
        } catch (VDMError e) {}
      }
    return stmt;
  }

  /**
   * Auxiliary function for creating a card. Use to create sample
   * data.
   */

  private VDMGeneric createCard(String nm, int code, int cardId, int accId)
      throws APIError, VDMError
  {
    VDMFactory fact = app.GetVDMFactory();
    if (ppOrSl)
      { // SL
        VDMGeneric card;
        VDMNumeric numCode = fact.MkNumeric(client, code);
        VDMNumeric numCardId = fact.MkNumeric(client, cardId);
        VDMNumeric numAccId = fact.MkNumeric(client, accId);
        VDMSequence args = fact.MkSequence(client);
        args.ImpAppend(numCode);
        args.ImpAppend(numCardId);
        args.ImpAppend(numAccId);

        card = Apply(client, "mk_Card", args);
        return card;
      }
    else
      {
        String cmd = "create " + nm + " := new Card().Create(";
        cmd = cmd + code + ",";
        cmd = cmd + cardId + ",";
        cmd = cmd + accId + ")";

        EvalCmd(cmd);
        VDMText card = fact.MkText(client, nm);
        return card;
      }
  }

  /**
   * Auxiliary function for creating a card holder. Use to create sample
   * data.
   */

  private VDMGeneric createCardHolder(String nm, String name, String addr)
      throws APIError, VDMError
  {
    VDMFactory fact = app.GetVDMFactory();

    if (ppOrSl)
      { // SL
        VDMText seqName = fact.MkText(client,name);
        VDMText seqAddr = fact.MkText(client,addr);
        VDMSequence args = fact.MkSequence(client);
        args.ImpAppend(seqName);
        VDMGeneric cardHolder;
        cardHolder = Apply(client, "mk_Cardholder", args);

        return cardHolder;
      }
    else
      {
        String cmd = "create " + nm + " := new Cardholder().Create(";
        cmd = cmd + "\"" + name + "\"" + ",\"" + addr + "\")";
        EvalCmd(cmd);
        VDMText cardHolder = fact.MkText(client, nm);
        return cardHolder;
      }
  }

  /**
   * Auxiliary function for creating a single account. Use to create sample
   * data.
   */

  private VDMGeneric createAccount(String nm, VDMMap m, String ms, int bal)
      throws APIError, VDMError
  {
    VDMFactory fact = app.GetVDMFactory();

    if (ppOrSl)
      { // SL
        VDMNumeric numBal = fact.MkNumeric(client, bal);
        VDMSequence args = fact.MkSequence(client);
        args.ImpAppend(m);
        args.ImpAppend(numBal);
        VDMSequence seqTrans = fact.MkSequence(client);
        args.ImpAppend(seqTrans);
        VDMGeneric account = Apply(client, "mk_Account", args);
        return account;
      }
    else
      {
        String cmd = "create " + nm + " := new Account()";
        EvalCmd(cmd);
        EvalExpression(client, nm+".Create(" + ms + "," + bal + ")" );
        VDMText account = fact.MkText(client, nm);
        return account;
      }
  }

  /**
   * Auxiliary function for creating a number of accounts. Used to
   * create sample data.
   */

  private void createAccounts(JProgressBar progressBar, JLabel status)
      throws APIError, VDMError
  {
    VDMFactory fact = app.GetVDMFactory();
    int i;

    VDMNumeric[] numAccId = new VDMNumeric[5];
    for (i=0; i < 5; i++)
      numAccId[i] = fact.MkNumeric(client,i);

    status.setText("Creating Card Holders");
    VDMGeneric[] cardHolder = new VDMGeneric[5];
    cardHolder[0] = createCardHolder("Holder1", "Peter Gorm Larsen",
                                     "Granvej 24");
    progressBar.setValue(progressBar.getValue()+1);
    cardHolder[1] = createCardHolder("Holder2", "Paul Mukherjee",
                                     "Rugaardsvej 47,2");
    progressBar.setValue(progressBar.getValue()+1);
    cardHolder[2] = createCardHolder("Holder3", "Sten Agerholm",
                                     "Teisensvej ??");
    progressBar.setValue(progressBar.getValue()+1);
    cardHolder[3] = createCardHolder("Holder4", "Kim Sunesen",
                                     "??");
    progressBar.setValue(progressBar.getValue()+1);
    cardHolder[4] = createCardHolder("Holder5", "hoge",
                                     "Forskerparken 10A");
    progressBar.setValue(progressBar.getValue()+1);


    VDMMap[] cardMap = new VDMMap[5];
    String[] cardMapStr = new String[5];
    for (i=0; i<5; i++){
      if (ppOrSl)
        {
          cardMap[i] = fact.MkMap(client);
          try {
            cardMap[i].Insert(fact.MkNumeric(client,i+1), cardHolder[i]);
          } catch (VDMError e) { System.out.println(e.err);}
        }
      else {
        cardMapStr[i] = "{" + (i+1) + " |-> Holder" + (i+1);
        if (i!=4)
          cardMapStr[i] = cardMapStr[i] + "}";
      }
    }

    // Extra cardholders for cardMap[4]
    if (ppOrSl)
      try {
        cardMap[4].Insert(fact.MkNumeric(client,6), cardHolder[2]);
        cardMap[4].Insert(fact.MkNumeric(client,7), cardHolder[0]);
      } catch (VDMError e) {}

    else
      {
        cardMapStr[4] = cardMapStr[4] + ", 6 |-> Holder3";
        cardMapStr[4] = cardMapStr[4] + ", 7 |-> Holder1}";
      }
    VDMGeneric[] genAccounts = new VDMGeneric[5];
    status.setText("Creating Accounts");

    genAccounts[0] = createAccount("acc1", cardMap[0], cardMapStr[0], 5000);
    progressBar.setValue(progressBar.getValue()+1);
    genAccounts[1] = createAccount("acc2", cardMap[1], cardMapStr[1], 0);
    progressBar.setValue(progressBar.getValue()+1);
    genAccounts[2] = createAccount("acc3", cardMap[2], cardMapStr[2], 9000);
    progressBar.setValue(progressBar.getValue()+1);
    genAccounts[3] = createAccount("acc4", cardMap[3], cardMapStr[3], 6000);
    progressBar.setValue(progressBar.getValue()+1);
    genAccounts[4] = createAccount("acc5", cardMap[4], cardMapStr[4], 70000);
    progressBar.setValue(progressBar.getValue()+1);

    //Create accounts in central resource
    status.setText("Adding Accounts");

    for (i = 0; i<5; i++)
      {
        if (ppOrSl){
          VDMSequence args;
          args = fact.MkSequence(client);
          args.ImpAppend(fact.MkNumeric(client,i+1));
          args.ImpAppend(genAccounts[i]);
          Apply(client, "AddAccount", args);
        }
        else {
          String argstr = "(" + (i+1) + ",";
          argstr = argstr + VDMTextHelper.narrow(genAccounts[i]).GetValue();
          argstr = argstr + ")";
          EvalExpression(client, centralResource + ".AddAccount" + argstr);
        }
        progressBar.setValue(progressBar.getValue()+1);
      }
  }

  /**
   * Auxiliary function for creating a number of cards. Used to
   * create sample data.
   */

  private void createCards(JProgressBar progressBar, JLabel status)
      throws APIError, VDMError
  {
    status.setText("Creating Cards");
    for (int i=0; i < 5; i++){
      cards[i] = createCard("Card" + (i+1), PINS[i], i+1, i+1);
      progressBar.setValue(progressBar.getValue()+1);
    }

    for (int i=5; i < 7; i++){
      cards[i] = createCard("Card" + (i+1), PINS[i], i+1, 5);
      progressBar.setValue(progressBar.getValue()+1);
    }

  }

  /**
   * Method for executing a command at the Toolbox. This mirrors the
   * EvalCmd function in the Toolbox API, but also logs the command.
   */

  private void EvalCmd(String cmd) throws APIError {
    Log("EvalCmd(" + cmd + ")");
    interp.EvalCmd(cmd);
  }

  /**
   * Method for applying a method at the Toolbox. This mirrors the
   * Apply function in the Toolbox API, but also logs the command.
   */

  private VDMGeneric Apply(short cl, String method, VDMSequence args)
    throws APIError
  {
    Log("Apply(" + cl + "," + method + "," + args.toString() + ")");
    return interp.Apply(cl, method, args);
  }

  /**
   * Method for evaluating an expression at the Toolbox. This mirrors the
   * EvalExpression function in the Toolbox API, but also logs the command.
   */

  private VDMGeneric EvalExpression(short cl, String expr)
      throws APIError, VDMError
  {
    Log("EvalExpression(" + cl + "," + expr + ")");
    return interp.EvalExpression(cl, expr);
  }

  /**
   * Method for logging communications.
   */

  private void Log (String logString)
  {
    try {
      commLog.write(logString+"\n");
      commLog.flush();
    } catch (IOException e) {}
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
