class WatchDog extends Thread {

  private Finished f;

  WatchDog (Finished nf){
      f = nf;
  }

  public void run() { 
    try {
      synchronized(f){
        f.wait(800);
        if(!f.Get())
            System.out.println("Timed out");
        System.exit(0);
      }
    }
    catch (InterruptedException e) {
    }
  }
}
 
