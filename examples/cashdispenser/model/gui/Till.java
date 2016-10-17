//
// File:        Till.java
// Description: Implements the GUI representing a Till.
// Date:        $Date: 2005/05/13 00:41:45 $
//
// Copyright Kyushu University

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;


public class Till 
       extends JFrame
       implements ActionListener
{
// MEMBERS:

  /**
   * Array of strings used to identify possible cards.
   */
  public static final String CARDS[] = 
     {"Card 1", "Card 2", "Card 3", "Card 4", "Card 5", "Card 6", "Card 7"};

  /**
   * Array of strings used to select a PIN code.
   */
  public static final String PINS[] = 
     {"1234", "5678", "9012", "3456", "7890", "1357", "2469"};

  /**
   * Array of strings used to select a withdrawal amount.
   */
  public static final String AMOUNTS[] = 
  {"Amount 100", "Amount 300", "Amount 500", "Amount 1000", "Amount 2000"};

  /**
   * The main Till panel.
   */
  private JPanel topPanel;

  /**
   * The button to insert a card.
   */
  private JButton insertCardButton;

  /**
   * The button to request return of a card.
   */
  private JButton returnCardButton;

  /**
   * The button to enter a PIN code.
   */
  private JButton enterPINButton;

  /**
   * The button to retrieve the current balance.
   */
  private JButton getBalanceButton;

  /**
   * The button to make a withdrawal.
   */
  private JButton makeWithdrawalButton;

  /**
   * The button to request a statement.
   */
  private JButton requestStatementButton;

  /**
   * A reference to the interface with the executing Toolbox.
   */
  private ToolboxInterface toolbox;

  /**
   * A reference to a VDMTill. This is used to keep track of various
   * tills in operation.  
   */
  private ToolboxInterface.VDMTill vdmTill;

  /**
   * The card currently in the till (if any).
   */
  private Object currentCard;  // don't know if needed, ask toolbox

// METHODS:

  /**
   * Creates a Till GUI.
   */
  public Till (ToolboxInterface toolbox, String tillName) 
  {
    setTitle( tillName );
    this.toolbox = toolbox;
    try {
        vdmTill = this.toolbox.getTill(tillName);
    } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
      SignalAPIError("New Till", e.msg);
    } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
      SignalAPIError("New Till", "VDMErrCode"+e.err);
    }

    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
            vdmTill.delete();
        }
    });

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
  }

  /**
   * Event handler for the Till's buttons.
   */
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

  /**
   * Method to insert a card into the Till. Uses the CARDS array to
   * offer a selection.
   */
  private void insertCard()
  {
    JOptionPane dialog = new JOptionPane();
    Object selectedCard = dialog.showInputDialog( this,
      "Insert a card", "Insert card", JOptionPane.QUESTION_MESSAGE,
      null, CARDS, CARDS[0]);

    if (selectedCard == null)
    {
      System.out.println( "No card was inserted" );
    }
    else 
    {
      currentCard = selectedCard;
      if (selectedCard instanceof String)
          try {
              toolbox.insertCard(vdmTill,(String)selectedCard);
              Till.Message(selectedCard + " inserted successfully");
              setTitle( vdmTill.getName() + ": " + selectedCard );

          } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
              SignalAPIError("Insert Card", e.msg);
          } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
              SignalAPIError("Insert Card", "VDMErrCode: " + e.err);
          }
    }
  }

  /**
   * Requests the return of a card from the Till.
   */
  private void returnCard()
  {
    // check precondition in VDM model (some card must be in machine)

    JOptionPane dialog = new JOptionPane();
    String s = currentCard + " has been returned";
    try {
        toolbox.returnCard(vdmTill);      
        dialog.showConfirmDialog( this,
                                  s, "Information", 
                                  JOptionPane.DEFAULT_OPTION,
                                  JOptionPane.INFORMATION_MESSAGE,null);
        setTitle( vdmTill.getName());
        currentCard = null;
    } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
        SignalAPIError("Return Card", e.msg);
    } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
      SignalAPIError("Return Card", "VDMErrCode: " + e.err);
    }
  }

  /**
   * Enters a PIN code. Uses the PINS array to select a PIN code.
   */
  private void enterPIN()
  {
    JOptionPane dialog = new JOptionPane();
    Object selectedPIN = dialog.showInputDialog( this,
      "Enter PIN code", "Enter PIN", JOptionPane.QUESTION_MESSAGE,
      null, PINS, PINS[0]);

    if (selectedPIN == null)
    {
      System.out.println( "No PIN was entered" );
    }
    else {
        String pinOk = "";
        if (selectedPIN instanceof String)
            try {
                pinOk = toolbox.enterPIN(vdmTill,(String) selectedPIN);
                if (pinOk.equals("PinOk"))
                    Till.Message("PIN entered successfully");
                else if (pinOk.equals("PinNotOk"))
                    Till.Message("Invalid PIN entered");
                else if (pinOk.equals("Retained")) { 
                    Till.Message("Card Retained");
                    setTitle( vdmTill.getName());
                    currentCard = null;
                }
                else Till.Message(pinOk);
            } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
                SignalAPIError("Enter PIN", e.msg);
            } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
              SignalAPIError("Enter PIN", "VDMErrCode: " + e.err);
            }
    }
  }
  

  /**
   * Gets the current balance.
   */
  private void getBalance()
  {
      try {
          int bal = toolbox.getBalance(vdmTill);
          JOptionPane dialog = new JOptionPane();
          dialog.showConfirmDialog( this,
                                    "The balance of " + currentCard + 
                                    " is " + bal, "Information", 
                                    JOptionPane.DEFAULT_OPTION,
                                    JOptionPane.INFORMATION_MESSAGE,null);
      } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
          SignalAPIError("Get Balance", e.msg);
      } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
        SignalAPIError("Get Balance", "VDMErrCode: " + e.err);
      }
  }

  /**
   * Makes a withdrawal. Uses the AMOUNTS array to specify the amount
   * to withdraw.
   */
  private void makeWithdrawal()
  {
    JOptionPane dialog = new JOptionPane();
    Object selectedAmount = dialog.showInputDialog( this,
      "Select amount to withdraw", "Make Withdrawal", 
      JOptionPane.QUESTION_MESSAGE,
      null, AMOUNTS, AMOUNTS[0]);

    if (selectedAmount == null)
    {
      System.out.println( "No amount was entered" );
    }
    else 
    {
      if (selectedAmount instanceof String)
          try {
              boolean withdrawalOk = toolbox.makeWithdrawal(vdmTill,
                                                    (String) selectedAmount);
              if (withdrawalOk)
                  Till.Message(selectedAmount + 
                               " Withdrawal made successfully");
              else
                  Till.Message(selectedAmount + " Withdrawal failed");
          } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
              SignalAPIError("Withdrawal", e.msg);
          } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
            SignalAPIError("Withdrawal", "VDMErrCode: " + e.err);
          }
    
    }

  }

  /**
   * Requests a statement. Requires confirmation before proceeding.
   */
  private void requestStatement()
  {
    JOptionPane dialog = new JOptionPane();
    int answer = dialog.showConfirmDialog( this,
      "Do you want to receive an account statement by post?", 
      "Request Statement", 
      JOptionPane.YES_NO_OPTION,JOptionPane.INFORMATION_MESSAGE,null);

    if (answer == 0) {
        Stmt stmt = null;
        try {
              stmt = toolbox.requestStatement(vdmTill);
              stmt.display();
        } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
            SignalAPIError("Request Statement",e.msg);
        } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
          SignalAPIError("Request Statement", "VDMErrCode: " + e.err);
        }
    }    
  }

  /**
   * Method to handle API exceptions.
   */
  private void SignalAPIError (String op, String msg){
      Object[] options = { "OK"};
      String s = "Operation " + op + " failed at runtime\n" 
                 + "Check VDMTools Interpreter for details"; 
      JOptionPane.showOptionDialog(null, s, "Warning", 
                                   JOptionPane.DEFAULT_OPTION, 
                                   JOptionPane.WARNING_MESSAGE,
                                   null, options, options[0]);
  }
        
 
  /**
   * Method to show a dialog with a specific message.
   */
  public static void Message(String s) {
      Object[] options = { "OK"};
      JOptionPane.showOptionDialog(null, s, "Information", 
                                   JOptionPane.DEFAULT_OPTION, 
                                   JOptionPane.INFORMATION_MESSAGE,
                                   null, options, options[0]);
  
  }




}


