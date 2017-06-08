//==================================================================================
//                                  PeriodicThread                            
//==================================================================================

// This class is a periodic thread implementation for the VDMCONC library
// This version does not have incremental drift, but suffers from the scheduler problem
// described in chapter Concjava.

// Periodic Threads are NOT started in the constructor, but only through a call to invoke.
// This is necessary to avoid a periodic thread being started on an half-initialized object
// i.e. if the periodic thread is defined in the constructor of a base class and the
// constructor of the derived classes was not yet completed (and therefor the instance
// of the *Sentinel class was not yet assigned). When the constructor of the lowest-
// level class was finished, it calls "perThread.invoke()" as its last operation and 
// actually starts all the "chained" periodic threads then.

package jp.vdmtools.VDM;

abstract public class PeriodicThread extends VDMThread {

  protected final int period;

  PeriodicThread next;   // this points to the periodic thread object
 
  public PeriodicThread (Integer m, PeriodicThread p) {
    next = p;           // This chaining is only necessary so that the object is not garbage collected
    period = m.intValue();
  }
 
  public void run() {
    long nextTime = System.currentTimeMillis();
    while (true) {

      try { threadDef(); }
      catch (CGException e) { System.out.println(e.getMessage()); };

      nextTime += period;
      int delay = (int) (nextTime - System.currentTimeMillis() );

      try { Thread.sleep( delay ); }
      catch (InterruptedException e) {}
    }
  }

  public void invoke() {
     this.start();
     if (next!=null) next.invoke();     // start the other periodic threads
  }

   abstract public void threadDef() throws CGException;
}



