import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;
import jp.vdmtools.VDM.CGException;


class TillGUI
       extends JFrame
    implements ActionListener
{
  private JPanel topPanel;
  private JButton insertCardButton;
  private JButton returnCardButton;
  private JButton enterPINButton;
  private JButton getBalanceButton;
  private JButton makeWithdrawalButton;
  private JButton requestStatementButton;

  private ToolboxInterface toolbox;
  private ToolboxInterface.VDMTill vdmTill;
  private final HashSet currentTills;
  private Object currentCard;  // don't know if needed, ask toolbox
  Thread tillThread;

  public Integer getIndex()
  {
    return vdmTill.getIndex();
  }

  public TillGUI (ToolboxInterface toolbox, int index, final HashSet s) throws CGException
  {
    this.toolbox = toolbox;
    vdmTill = this.toolbox.getTill(new Integer(index));
    setTitle( vdmTill.getName() );
    currentTills = s;


    topPanel = new JPanel();
    topPanel.setLayout( new GridLayout(2,1) );
    topPanel.setBorder(BorderFactory.createEmptyBorder(20,20,20,20));
    getContentPane().add ( topPanel );

    currentCard = null;

    JPanel tillPanel = new JPanel();
    tillPanel.setLayout(new GridLayout(3,1));
    tillPanel.setBorder(BorderFactory.createEmptyBorder(0,0,10,0));
    topPanel.add(tillPanel);
    insertCardButton = new JButton("Insert card");
    tillPanel.add(insertCardButton);
    enterPINButton = new JButton("Enter PIN");
    tillPanel.add(enterPINButton);
    returnCardButton = new JButton("Return card");
    tillPanel.add(returnCardButton);

    JPanel functionsPanel = new JPanel();
    functionsPanel.setLayout(new GridLayout(3,1));
    topPanel.add(functionsPanel);
    getBalanceButton = new JButton("Get balance");
    functionsPanel.add(getBalanceButton);
    makeWithdrawalButton = new JButton("Make withdrawal");
    functionsPanel.add(makeWithdrawalButton);
    requestStatementButton = new JButton("Request statement");
    functionsPanel.add(requestStatementButton);

    insertCardButton.addActionListener(this);
    returnCardButton.addActionListener(this);
    enterPINButton.addActionListener(this);
    getBalanceButton.addActionListener(this);
    makeWithdrawalButton.addActionListener(this);
    requestStatementButton.addActionListener(this);

    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
	    currentTills.remove(getIndex());
           }});
  }



  public void actionPerformed( ActionEvent event) 
  {
    if(event.getSource() == insertCardButton)
    {
      insertCard();
    }
    else if (event.getSource() == returnCardButton)
    {
      returnCard();
    }
    else if (event.getSource() == enterPINButton)
    {
      enterPIN();
    }
    else if (event.getSource() == getBalanceButton)
    {
      getBalance();
    }
    else if (event.getSource() == makeWithdrawalButton)
    {
      makeWithdrawal();
    }
    else if (event.getSource() == requestStatementButton)
    {
      requestStatement();
    }
  }

  private void insertCard()
  {
    JOptionPane dialog = new JOptionPane();
    Object selectedCard = dialog.showInputDialog( this,
      "Insert a card", "Insert card", JOptionPane.QUESTION_MESSAGE,
      null, CashDispenser.CARDS, CashDispenser.CARDS[0]);

    if (selectedCard == null)
    {
      System.out.println( "No card was inserted" );
    }
    else 
    {
      currentCard = selectedCard;
      if (selectedCard instanceof String)
      {
	  toolbox.insertCard(vdmTill,(String)selectedCard);
	  TillGUI.Message(selectedCard + " inserted successfully");
	  setTitle( vdmTill.getName() + ": " + selectedCard );
      }
    }
  }

  private void returnCard()
  {
    // check precondition in VDM model (some card must be in machine)

    JOptionPane dialog = new JOptionPane();
    String s = currentCard + " has been returned";
    toolbox.returnCard(vdmTill);      
    dialog.showConfirmDialog( this,
			      s, "Information", 
			      JOptionPane.DEFAULT_OPTION,
			      JOptionPane.INFORMATION_MESSAGE,null);
    setTitle( vdmTill.getName());
    currentCard = null;
  }

  private void enterPIN()
  {
    JOptionPane dialog = new JOptionPane();
    Object selectedPIN = dialog.showInputDialog( this,
      "Enter PIN code", "Enter PIN", JOptionPane.QUESTION_MESSAGE,
      null, CashDispenser.PINS, CashDispenser.PINS[0]);

    if (selectedPIN == null)
    {
      System.out.println( "No PIN was entered" );
    }
    else {
        String pinOk = "";
        if (selectedPIN instanceof String)
        {
	    pinOk = toolbox.enterPIN(vdmTill,(String) selectedPIN);
	    if (pinOk.equals("<PinOk>"))
		TillGUI.Message("PIN entered successfully");
	    else if (pinOk.equals("<PinNotOk>"))
		TillGUI.Message("Invalid PIN entered");
	    else if (pinOk.equals("<Fail>"))
		TillGUI.Message("PIN Entry Failed");
	    else if (pinOk.equals("<Retained>")) { 
		TillGUI.Message("Card Retained");
		setTitle( vdmTill.getName());
		currentCard = null;
	    }
	    else TillGUI.Message(pinOk);
	}
    }
  }
  

    private void getBalance()
    {
	Object bal = toolbox.getBalance(vdmTill);
	String msg;
	if (bal instanceof Integer)
	    msg = "The balance of " + currentCard + " is " + ((Integer) bal).intValue();
	else if (bal instanceof quotes.Fail)
	    msg = "Balance failed";
	else if (bal == null)
	    msg = "Unknown account";
	else msg = "Unknown response";
	JOptionPane dialog = new JOptionPane();
	dialog.showConfirmDialog( this,
				  msg, "Information", 
				  JOptionPane.DEFAULT_OPTION,
				  JOptionPane.INFORMATION_MESSAGE,null);
    }

    private void makeWithdrawal()
    {
      JOptionPane dialog = new JOptionPane();
      Object selectedAmount = dialog.showInputDialog( this,
        "Select amount to withdraw", "Make Withdrawal", 
        JOptionPane.QUESTION_MESSAGE,
        null, CashDispenser.AMOUNTS, CashDispenser.AMOUNTS[0]);

      if (selectedAmount == null)
      {
        System.out.println( "No amount was entered" );
      }
      else 
      {
        if (selectedAmount instanceof String){
	    Object wd = toolbox.makeWithdrawal(vdmTill,
					       (String) selectedAmount);
	    if (wd instanceof Boolean){
		boolean withdrawalOk = ((Boolean) wd).booleanValue();
		if (withdrawalOk)
		    TillGUI.Message(selectedAmount + 
				    " Withdrawal made successfully");
		else
		    TillGUI.Message(selectedAmount + " Withdrawal rejected");
	    }
	    else if (wd instanceof quotes.Fail)
		TillGUI.Message("Withdrawal Failed");
	    else
		TillGUI.Message("Unknown response");
	}
      }

    }

    private void requestStatement()
    {
      JOptionPane dialog = new JOptionPane();
      int answer = dialog.showConfirmDialog( this,
        "Do you want to receive an account statement by post?", 
        "Request Statement", 
        JOptionPane.YES_NO_OPTION,JOptionPane.INFORMATION_MESSAGE,null);

      if (answer == 0) {
          Object resp = toolbox.requestStatement(vdmTill);
	  if (resp instanceof quotes.Fail)
	      TillGUI.Message("Statement Request Failed");
	  else if (resp instanceof Boolean)
	      TillGUI.Message("Request Statement: Account Not Known");
	  else {
	      Stmt stmt = (Stmt) resp;
	      stmt.display();
	  }
      }    
    }

 
  public static void Message(String s) {
      Object[] options = { "OK"};
      JOptionPane.showOptionDialog(null, s, "Information", 
                                   JOptionPane.DEFAULT_OPTION, 
                                   JOptionPane.INFORMATION_MESSAGE,
                                   null, options, options[0]);
  
  }




}


