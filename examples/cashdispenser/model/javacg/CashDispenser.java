import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;
import jp.vdmtools.VDM.CGException;


class CashDispenser
       extends JFrame
       implements ActionListener
{
  private JPanel topPanel;
  private JButton illegalCardButton;
  private JButton failTillButton;
  private JButton newTillButton;
  private ToolboxInterface toolbox;
  private HashSet currentTills = new HashSet();

  private int numTills = 0;
  
  public static final String CARDS[] = 
     {"Card 1", "Card 2", "Card 3", "Card 4", "Card 5", "Card 6", "Card 7"};

  public static final String PINS[] = 
     {"1234", "5678", "9012", "3456", "7890", "1357", "2469"};

  public static final String AMOUNTS[] = 
  {"Amount 100", "Amount 300", "Amount 500", "Amount 1000", "Amount 2000"};

  public CashDispenser () throws CGException
  {
    toolbox = new ToolboxInterface();

    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
            System.exit(0);}});
    setTitle( "CashDispenser " );
  

    topPanel = new JPanel();
    topPanel.setLayout( new GridLayout(3,1) );
    topPanel.setBorder(BorderFactory.createEmptyBorder(50,70,50,70));
    getContentPane().add ( topPanel );

    newTillButton = new JButton("New till");
    topPanel.add(newTillButton);
    newTillButton.addActionListener(this);

    illegalCardButton = new JButton("Illegal card");
    topPanel.add(illegalCardButton);
    illegalCardButton.addActionListener(this);

    failTillButton = new JButton("Fail Till");
    topPanel.add(failTillButton);
    failTillButton.addActionListener(this);

    pack();
    setVisible( true );

  }

  public void actionPerformed( ActionEvent event)
  {
    try{
    if(event.getSource() == newTillButton)
    {
      TillGUI till = new TillGUI(toolbox,numTills, currentTills);
      currentTills.add(new Integer(numTills));
      numTills++;
      till.pack();
      till.setVisible(true);
    }
    else if(event.getSource() == illegalCardButton)
    {
      JOptionPane dialog = new JOptionPane();
      Object selectedCard = dialog.showInputDialog( this,
        "Select illegal card", "Illegal card", JOptionPane.QUESTION_MESSAGE,
        null, CashDispenser.CARDS, CashDispenser.CARDS[0]);

      if (selectedCard instanceof String)
	  toolbox.reportIllegalCard((String)selectedCard);
    }
    else if(event.getSource() == failTillButton)
    {
	if (!currentTills.isEmpty()){
	    JOptionPane dialog = new JOptionPane();
	    String tills[]= new String[currentTills.size()];
	    int i = currentTills.size() - 1;
	    for (Iterator iter = currentTills.iterator(); iter.hasNext();){
		tills[i] = ((Integer) iter.next()).toString();
		i--;
	    }
	    Object selectedTill = dialog.showInputDialog( this,
						  "Select Till To Fail",
						  "Fail Till", 
						  JOptionPane.QUESTION_MESSAGE,
						  null, tills, tills[0]);
      
	    if (selectedTill instanceof String){
		toolbox.failTill(Integer.parseInt((String)selectedTill));
		currentTills.remove(new Integer((String)selectedTill));
	    }
	}
    }
    }
    catch (Exception e ) {}

  }

  public static void main( String args[] )
  {
    try
    {
      CashDispenser cd = new CashDispenser();
    }
    catch( Exception e ) {}
  }
   
}
