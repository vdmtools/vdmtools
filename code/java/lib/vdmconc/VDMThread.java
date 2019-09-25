//==================================================================================
//                                  VDMThread                            
//==================================================================================

// This class extends Thread in order to supply the VDM notion of threadid
// The class is used both for periodic threads and procedural threads.

package jp.vdmtools.VDM;

public class VDMThread extends Thread {

  protected long id; 

  protected static int count = 0;   // static counter to get unique ids for individual threads

  public VDMThread(Runnable target) { 
    super(target);
    id = count++;
  }

  public VDMThread() {      // This constructor is used by procedural threads
     id = count++;
  }

  public long getId() { return id; }

  public static Integer getThreadId() {
    return Integer.valueOf((int)((VDMThread)(Thread.currentThread())).getId());
  }
}

  

