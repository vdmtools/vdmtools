////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   class Sentinel
//

//  This class implements the stuff necessary for enforcing permission predicates.
//  It does _not_ implement debugging aids. This is done in class PermPredSentinelDbg
//  Object State guards are supported by simply including them in the local
//  evaluatePP() function.



package jp.vdmtools.VDM;

abstract public class Sentinel {

  // These variables model the current state of the object

  public volatile int[] act;    
  public volatile int[] fin;                                           
  public volatile int[] req;                                           
  public volatile int[] active;     // assert( active  ==  act - fin )
  public volatile int[] waiting;    // assert( waiting ==  req - act )

  EvaluatePP instance;

  public void init(int nrf, EvaluatePP inst) {
    
    instance = inst;    

    act = new int[nrf];       // Java: all initial values are zero
    fin = new int[nrf];                                           
    req = new int[nrf];                                           
    active = new int[nrf];  
    waiting = new int[nrf]; 

  }


  //////////////////////////////////////////////////
  //
  //  Evaluates permission predicates and can block
  //                     

  //  Is called by operations that DO implement a
  //  permission predicate at entering 


  public synchronized void entering (int fnr) throws CGException {

    requesting(fnr);         // (!) entering can be inlined!

    try {
      
      if  ( ! instance.evaluatePP(fnr).booleanValue() ) {
	
	waiting(fnr, +1);
	
	while ( ! instance.evaluatePP(fnr).booleanValue() )
	  {
	    this.wait();      // the actual wait-statement
	  }
	
	waiting(fnr, -1);  
	
      } //eoi
      
    } catch (InterruptedException e) {}
    
    activating(fnr);
    
  } // eof


  //////////////////////////////////////////////////
  //
  //  leaving
  //

  //  Is called by operations that DO implement a
  //  permission predicate before they leave

  public synchronized void leaving(int fn) {  
     fin[fn]++;
     active[fn]--;
     stateChanged(); 
  }


  //////////////////////////////////////////////////
  //
  //  stateChanged()
  //

  //  Can be called by operations that do NOT implement
  //  permission predicates to signal a change to 
  //  instance variables

  public synchronized void stateChanged() {
    notifyAll();
  }

  //////////////////////////////////////////////////
  //
  //  Auxiliary private functions
  //
   
  private synchronized void requesting(int fn ) {
    req[fn]++;
    stateChanged();  
  }

  private synchronized void activating(int fn ) {
    act[fn] ++;
    active[fn]++;
    stateChanged();  
  }

  private  synchronized void waiting(int fn, int offset ){
    waiting[fn] += offset;
    stateChanged();  
  }

} // eoc PermPredSentinel














