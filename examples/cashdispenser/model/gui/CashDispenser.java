//
// File:        ToolboxInterface.java
// Description: Top level class containing program entry point.
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


public class CashDispenser
       extends JFrame
       implements ActionListener
{
// MEMBERS:
  /**
   * @serial The main panel.
   */
  private JPanel topPanel;

  /**
   * @serial A button to make a card illegal.
   */
  private JButton illegalCardButton;

  /**
   * @serial A button to add new tills (VDM++ only).
   */
  private JButton newTillButton;

  /**
   * @serial A flag indicating whether the Toolbox is for VDM++ or VDM-SL. 
   */
  private boolean ppOrSl = false; // false = PP, true = SL

  /**
   * @serial A reference to the interface with the executing Toolbox.
   */
  private ToolboxInterface toolbox;

  /**
   * @serial The number of tills currently created (VDM++ only).
   */
  private int numTills = 0;
  
// METHODS:

  /**
   * The constructor creates a GUI.
   */
  public CashDispenser () 
  {
    String tooltype = "pp";
    try {
        toolbox = new ToolboxInterface(tooltype);
    } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
        SignalAPIError(e.msg);
    } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
      SignalAPIError("VDMErrCode: " + e.err);
    }

    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
            toolbox.close();
            System.exit(0);}});
    setTitle( "Cash Dispenser" );
  
    ppOrSl = false;

    topPanel = new JPanel();
    topPanel.setLayout( new GridLayout(2,1) );
    topPanel.setBorder(BorderFactory.createEmptyBorder(50,70,50,70));
    getContentPane().add ( topPanel );

    if (ppOrSl)
    {
        //SL
      Till till = new Till(toolbox,"Till");
      till.pack();
      till.setVisible(true);
    }
    else
    {
        //PP
        newTillButton = new JButton("New till");
        topPanel.add(newTillButton);
        newTillButton.addActionListener(this);
    }

    illegalCardButton = new JButton("Illegal card");
    topPanel.add(illegalCardButton);
    illegalCardButton.addActionListener(this);

    pack();
    setVisible( true );

  }

  /**
   * Event handler for the GUI.
   */

  public void actionPerformed( ActionEvent event) 
  {
    if(event.getSource() == newTillButton)
    {
      Till till = new Till(toolbox,"Till" + numTills);
      numTills++;
      till.pack();
      till.setVisible(true);
    }
    else if(event.getSource() == illegalCardButton)
    {
      JOptionPane dialog = new JOptionPane();
      Object selectedCard = dialog.showInputDialog( this,
        "Select illegal card", "Illegal card", JOptionPane.QUESTION_MESSAGE,
        null, Till.CARDS, Till.CARDS[0]);

      if (selectedCard instanceof String)
          try {
              toolbox.reportIllegalCard((String)selectedCard);
          } catch (jp.vdmtools.api.corba.ToolboxAPI.APIError e) {
              SignalAPIError(e.msg);
          } catch (jp.vdmtools.api.corba.VDM.VDMError e) {
            SignalAPIError("VDMErrCode: " + e.err);
          }
    }
  }

  /**
   * Main program entry point. 
   */
  public static void main( String args[] )
  {
      CashDispenser cd = new CashDispenser();
  }
   

  /**
   * Method for handling API exceptions.
   */
  public static void SignalAPIError(String m) {
      Till.Message(m);
  }

} 
