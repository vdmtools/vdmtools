import java.io.*;
import java.awt.*;
import java.text.*;
import javax.swing.*;
import java.util.*;
import jp.vdmtools.VDM.CGException;


class ToolboxInterface {

  private final int PINS[] =
     {1234, 5678, 9012, 3456, 7890, 1357, 2469};

  private final String names[] =
  {"Peter Gorm Larsen", "Paul Mukherjee", "Sten Agerholm", "Kim Sunesen",
   "hoge"};

  private final String addresses[] =
  {"Granvej 24", "Rugaardsvej 47", "Teisenvej ??", "??", "Forskerparken 10A"};

  private final int balances[] = {5000, 0, 9000, 6000, 3000};

  private Card[] cards = new Card[7];
  private HashMap cardMap = new HashMap();


  private HashMap ltills = new HashMap();
  private HashMap tills = new HashMap();
  private CentralResource resource = null;
  private Clock clock = null;
  private Letterbox letterbox = null;

  public class VDMTill {
      Till t;
      Integer index;

      public VDMTill(Till nt, Integer i) throws CGException {
        t = nt;
	index = i;
	LocalTill lt = new LocalTill();
	Channel c = new Channel();
	LocalResource lr = new LocalResource();
	try {
	    lr.Create(c);
	} catch (CGException e) {
	    TillGUI.Message("CGException");
	}


	try {
	    lt.Create(c, resource);
	    lt.start();
	    ltills.put(i, lt);
	    t.Create(lr);
	    tills.put(i, t);
	}
	catch (CGException e) {
	    TillGUI.Message("CGException");
	}

      }

      public Till getVal() {
        return t;
      }

      public Integer getIndex() {
	return index;
      }

      public String getName() {
	return "Till " + index;
      }


  }

   public ToolboxInterface() throws CGException
    {

      JProgressBar progressBar = new JProgressBar(0, 21);
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


      java.util.Date date = java.util.Calendar.getInstance().getTime();
      String dateString =
	  DateFormat.getDateInstance(DateFormat.MEDIUM).format(date);

      ltills = new HashMap();
      tills = new HashMap();
      resource = new CentralResource();
      clock = new Clock();
      letterbox = new Letterbox();
      try {
	  clock.SetDate(dateString);
	  resource.Create(clock, letterbox);
      }
      catch (CGException e) {
	  TillGUI.Message("CGException");
      }

      createAccounts(progressBar, status);
      createCards(progressBar, status);
      frame.setVisible(false);
    }

    public VDMTill getTill(Integer i) throws CGException
    {
      Till t = new Till();
      return new VDMTill(t,i);
    }

    public void failTill(int index) throws CGException
    {
	LocalTill lt = (LocalTill) ltills.get(new Integer(index));
	try {
	    lt.Fail();
	}
	catch (CGException e) {
	    TillGUI.Message("CGException");
	}


    }


    public void reportIllegalCard(String card)
    {
	Integer cardId = new Integer(((Integer)cardMap.get(card)).intValue() + 1);

	try {
	    resource.AddIllegalCard(cardId);
	}
	catch (CGException e) {
	    TillGUI.Message("CGException: reportIllegalCard");
	}

    }

    public void insertCard(VDMTill vdmTill, String card)
    {
	Till t = (Till) tills.get(vdmTill.getIndex());
	int cardInd = ((Integer) cardMap.get(card)).intValue();


	try {
	    t.InsertCard(cards[cardInd]);
	}
	catch (CGException e) {
	    TillGUI.Message("CGException");
	}

    }

    public void returnCard(VDMTill vdmTill)
    {
	Till t = (Till) tills.get(vdmTill.getIndex());
	try {
	    t.ReturnCard();
	}
	catch (CGException e) {
	    TillGUI.Message("CGException");
	}

    }

    public String enterPIN(VDMTill vdmTill, String pin)
    {
        Till t = (Till) tills.get(vdmTill.getIndex());
	Object validRes=null;

	try {
	    validRes = t.Validate(new Integer(pin));
	}
	catch (CGException e) {
	    TillGUI.Message("CGException");
	}
	return validRes.toString();

    }

      public Object getBalance(VDMTill vdmTill)
      {
	  Object bal = null;
	  Till t = (Till) tills.get(vdmTill.getIndex());
	  try {
	      bal =  t.GetBalance();
	  }
	  catch (CGException e) {
	      TillGUI.Message("CGException: getBalance");
	  }
	  return bal;


      }

      public Object makeWithdrawal(VDMTill vdmTill, String amount)
      {
          String amtNum = amount.substring(7);
	  Object wd = null;
	  Till t = (Till) tills.get(vdmTill.getIndex());
	  try {
	      wd =  t.MakeWithdrawal(new Integer(amtNum));
	  }
	  catch (CGException e) {
	      TillGUI.Message("CGException: makeWithdrawal");
	  }
	  return wd;


      }

      public Object requestStatement(VDMTill vdmTill)
      {
	  Till t = (Till) tills.get(vdmTill.getIndex());
	  Object res = null;
	  try {
	      res =  t.RequestStatement();
	  }
	  catch (CGException e) {
	      TillGUI.Message("CGException: makeWithdrawal");
	  }

	  if (res instanceof quotes.Fail)
	      return res;
	  else if ((res instanceof Boolean) && !((Boolean) res).booleanValue())
	      return res;
	  else {
	      Stmt result = new Stmt();
	      Letter l = null;
              try {
		  l = letterbox.GetLastStatement();
	      }
	      catch (CGException e) {
		  TillGUI.Message("CGException: requestStatement");
	      }

              return makeStatement(l);
	  }

      }


      private Stmt makeStatement(Letter l)
      {
          Stmt stmt = new Stmt(l.getName(), l.getBalance().intValue());
          Vector trans = l.getTransactions();

          for (Iterator iter = trans.iterator(); iter.hasNext(); )
          {
	      Account.Transaction acc_t = (Account.Transaction) iter.next();

              Stmt.Transaction t = stmt.new Transaction(acc_t.date,
							acc_t.cardId.intValue(),
                                                        acc_t.amount.intValue());
	      stmt.AddTransaction(t);
	  }
          return stmt;
      }



    private Card createCard(int code, int cardId, int accId) throws jp.vdmtools.VDM.CGException
    {
	Card c = new Card();
	try {
	    c.Create(new Integer(code), new Integer(cardId), new Integer(accId));
	} catch (CGException e) {
	    TillGUI.Message("CGException");
	}

        return c;
    }

    private Cardholder createCardHolder(String name, String addr) throws CGException
    {
      Cardholder ch = new Cardholder();
      try {
	  ch.Create(name, addr);
      } catch (CGException e) {
	  TillGUI.Message("CGException");
      }

      return ch;
    }

    private Account createAccount(HashMap m, int bal) throws CGException
    {
      Account acc = new Account();
      try {
	  acc.Create(m, new Integer(bal));
      } catch (CGException e) {
	  TillGUI.Message("CGException");
      }

      return acc;
    }

    private void createAccounts(JProgressBar progressBar, JLabel status) throws CGException
    {
      Cardholder[] cardHolders = new Cardholder[5];
      int i;

      status.setText("Creating Card Holders");
      for (i=0; i<5; i++){
	  cardHolders[i] = createCardHolder(names[i],
                                            addresses[i]);
	  progressBar.setValue(progressBar.getValue()+1);
      }


      HashMap[] cardHolderMap = new HashMap[5];
      for (i=0; i<5; i++){
	  cardHolderMap[i] = new HashMap();

	  cardHolderMap[i].put(new Integer(i+1), cardHolders[i]);
      }

      // Extra cardholders for cardMap[4]
      cardHolderMap[4].put(new Integer(6), cardHolders[2]);
      cardHolderMap[4].put(new Integer(7), cardHolders[0]);
      Account[] accounts = new Account[5];

      status.setText("Creating Accounts");
      for (i=0; i<5; i++) {
	  accounts[i] = createAccount(cardHolderMap[i], balances[i]);
	  progressBar.setValue(progressBar.getValue()+1);
      }

      //Create accounts in central resource
      status.setText("Adding Accounts");

      for (i = 0; i<5; i++)
      {
	  try {
	      resource.AddAccount(new Integer(i+1), accounts[i]);
	  } catch (CGException e) {
	      TillGUI.Message("CGException");
	  }
          progressBar.setValue(progressBar.getValue()+1);
      }
    }

    private void createCards(JProgressBar progressBar, JLabel status) throws CGException
    {
        status.setText("Creating Cards");
        for (int i=0; i < 5; i++){
          cards[i] = createCard(PINS[i], i+1, i+1);
          progressBar.setValue(progressBar.getValue()+1);
        }

        for (int i=5; i < 7; i++){
          cards[i] = createCard(PINS[i], i+1, 5);
          progressBar.setValue(progressBar.getValue()+1);
        }

	// create map from card strings to corresponding indices
	for (int i = 0; i < 7; i++){
	    cardMap.put(CashDispenser.CARDS[i], new Integer(i));
	}


    }




}
