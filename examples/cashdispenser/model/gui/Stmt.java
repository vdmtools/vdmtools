//
// File:        ToolboxInterface.java
// Description: Implements a graphical representation of a statement.
// Date:        $Date: 2005/05/13 00:41:45 $
//
// Copyright Kyushu University

import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;

public class Stmt {
// MEMBERS:    
    private String name;
    private int balance;
    private Transaction[] transactions = new Transaction[24];
    private int numItems = 0;

// INNER CLASSES:

    /**
     * An inner class used to represent each individual transaction. 
     */

    class Transaction {
      public String date;
      public int cardId;
      public int amount;

      /**
       * The constructor initializes the object with the given parameters. 
       */
      public Transaction(String d, int c, int a){
          date = d;
          cardId = c;
          amount = a;
      }

      /**
       * The string form of the transaction is designed to output the
       * transaction logically. 
       */
      public String toString()
      {
          String s;
          s = "Date: " + date + "\tCard: " + cardId + "\tAmount: " + amount;
          return s;
      }
    }

// METHODS:

    /**
     * This constructor merely calls that in the super class (if any).
     */
    public Stmt()
    {
        super();
    }

    /** 
     * This constructor initializes the object with the customer's
     * name and closing balance.
     */
    public Stmt(String n, int b)
    {
        balance = b;
        name = n;
    }

    /**
     * This method outputs the statement in its own window.
     */

    public void display(){
        TableModel dataModel = new AbstractTableModel() {
            public int getColumnCount() { return 3;}
            public int getRowCount() { return numItems;}
            public Object getValueAt(int row, int col) {
              if (col == 0)
                return transactions[row].date;
              else if (col == 1)
                return Integer.valueOf(transactions[row].cardId);
              else return Integer.valueOf(transactions[row].amount);
            }
            public String getColumnName(int col) {
              if (col == 0)
                return "Date";
              else if (col == 1)
                return "CardID";
              else return "Amount";
            }
        };
        JTable tableView = new JTable(dataModel);
        tableView.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
        TableColumn dateColumn = tableView.getColumn("Date");
        dateColumn.setPreferredWidth(250);
        TableColumn cardIdColumn = tableView.getColumn("CardID");
        cardIdColumn.setPreferredWidth(70);
        TableColumn amtColumn = tableView.getColumn("Amount");
        amtColumn.setPreferredWidth(70);
        JScrollPane scrollpane = new JScrollPane(tableView);
        JFrame frame = new JFrame("Statement for Cardholder " + name); 
        scrollpane.setPreferredSize(new java.awt.Dimension(390, 200));
        JTextArea balText = new JTextArea(Integer.valueOf(balance).toString());
        JLabel balLabel = new JLabel("End of Statement Balance: ");
        JPanel balPanel = new JPanel();
        balPanel.add(balLabel);
        balPanel.add(balText);
        frame.getContentPane().setLayout(new BorderLayout());
        frame.getContentPane().add(scrollpane, BorderLayout.NORTH);
        frame.getContentPane().add(balPanel, BorderLayout.SOUTH);
        frame.pack();
        frame.setVisible(true);
    }

    /**
     * This method adds the given transaction to the current
     * statement.
     */

    public void AddTransaction(Transaction t)
    {
        if (numItems < transactions.length){
            transactions[numItems] = t;
            numItems++;
        }
    }
        

}
