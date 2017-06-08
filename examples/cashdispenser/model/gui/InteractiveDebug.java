import javax.swing.*;
import java.awt.event.*;
import org.omg.CORBA.*;
import jp.vdmtools.api.*;
import jp.vdmtools.api.corba.ToolboxAPI.*;
import jp.vdmtools.api.corba.VDM.*;

/**
 * This class can be used, to allow interactive debbuging
 * of an Toolbox-API-client.
 */
class InteractiveDebug extends JDialog implements ActionListener
{
    class DebugSemaphore
    {
        public synchronized void block ()
        {
            try { wait () ; } catch (InterruptedException ex) { notify (); }
        }

        public synchronized void resume ()
        {
            notify ();
        }
    }

    protected VDMInterpreter _interpreter;
    protected VDMTuple       _result;
    protected short          _clientId;
    protected DebugSemaphore _semaphore;

    /**
     * Constructor for the InteractiveDebug class
     * @param interpreter Reference to the CORBA-API-Interpreter
     */
    public InteractiveDebug (VDMInterpreter interpreter)
    {
        _interpreter = interpreter;
        _semaphore = new DebugSemaphore ();

        JButton stepButton = new JButton ("Step");
        stepButton.setActionCommand ("BUTTON_Step");
        stepButton.addActionListener (this);

        JButton stepInButton = new JButton ("Step in");
        stepInButton.setActionCommand ("BUTTON_StepIn");
        stepInButton.addActionListener (this);

        JButton singleStepButton = new JButton ("Single step");
        singleStepButton.setActionCommand ("BUTTON_SingleStep");
        singleStepButton.addActionListener (this);

        JButton contButton = new JButton ("Continue");
        contButton.setActionCommand ("BUTTON_Continue");
        contButton.addActionListener (this);

        getContentPane ().setLayout (new java.awt.GridLayout (4, 1)); 

        getContentPane ().add (stepButton);
        getContentPane ().add (stepInButton);
        getContentPane ().add (singleStepButton);
        getContentPane ().add (contButton);

        pack ();
        hide ();
    }

    /**
     * This method wraps the StartDebugging call of the
     * Interpreter API. Once the call to the debugger
     * returns, the return value is checked. If the
     * evaluation was successfully finished or an error
     * occured, the result is returned. Otherwise,
     * the method blocks and waits until the user 
     * initiates a DebugStep, DebugStepIn, DebugSingleStep
     * or DebugContinue by pressing one of the buttons
     * in the dialog.
     * The result of this Step is checked again and 
     * returned on error or success.
     * THIS METHOD MUST NOT BE CALLED FROM WITHIN THE
     * SWING/AWT-EVENT-THREAD!!!
     */
    public VDMTuple StartDebugging (short id, String expr) 
        throws APIError, VDMError
    {        
        _clientId = id;
        
        VDMQuote evalState;

        _result   = _interpreter.StartDebugging (_clientId, expr);
        evalState = VDMQuoteHelper.narrow (_result.GetField (1));

        
        while (!(evalState.GetValue ().equals ("SUCCESS")) && 
               !(evalState.GetValue ().equals ("ERROR")))
            {
                show ();
                // wait for user interaction
                _semaphore.block ();
                evalState = VDMQuoteHelper.narrow (_result.GetField (1));
            }
        hide ();
        return _result;
    }


    /**
     * This method is called when the user presses
     * one of the dialog-buttons. It initiates a
     * DebugStep, DebugStepIn, DebugSingleStep or
     * DebugContinue.
     * After the Step-method returns, the StartDebugging
     * method of this class is signaled and execution
     * of it is continued
     */
    public void actionPerformed (ActionEvent e)
    {
        if (e.getActionCommand ().equals ("BUTTON_Step"))
            {
                _result = _interpreter.DebugStep (_clientId);
                _semaphore.resume ();
            }
        else if (e.getActionCommand ().equals ("BUTTON_StepIn"))
            {
                _result = _interpreter.DebugStepIn (_clientId);
                _semaphore.resume ();
            }
        else if (e.getActionCommand ().equals ("BUTTON_SingleStep"))
            {
                _result = _interpreter.DebugSingleStep (_clientId);
                _semaphore.resume ();
            }
        else if (e.getActionCommand ().equals ("BUTTON_Continue"))
            {
                _result = _interpreter.DebugContinue (_clientId);
                _semaphore.resume ();
            }
    }
    
}
