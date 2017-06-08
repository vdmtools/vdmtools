import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;

class Stmt {
    
    class Transaction {
      public String date;
      public int cardId;
      public int amount;

      public Transaction(String d, int c, int a){
          date = d;
          cardId = c;
          amount = a;
      }

      public String toString()
      {
          String s;
          s = "Date: " + date + "\tCard: " + cardId + "\tAmount: " + amount;
          return s;
      }
    }

    private String name;
    private int balance;
    private Transaction[] transactions = new Transaction[24];
    private int numItems = 0;

    public Stmt()
    {
        super();
    }

    public Stmt(String n, int b)
    {
        balance = b;
        name = n;
    }

    public void display(){
        TableModel dataModel = new AbstractTableModel() {
            public int getColumnCount() { return 3;}
            public int getRowCount() { return numItems;}
            public Object getValueAt(int row, int col) {
              if (col == 0)
                return transactions[row].date;
              else if (col == 1)
                return new Integer(transactions[row].cardId);
              else return new Integer(transactions[row].amount);
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
        JTextArea balText = new JTextArea(new Integer(balance).toString());
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

    public void AddTransaction(Transaction t)
    {
        if (numItems < transactions.length){
            transactions[numItems] = t;
            numItems++;
        }
    }
        

}
