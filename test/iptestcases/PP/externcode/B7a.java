//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Thu 12-Oct-2000 by the VDM++ JAVA Code Generator
//
// Supported compilers:
// jdk1.2
//

// ***** VDMTOOLS START Name=HeaderComment KEEP=NO

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=NO
// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=NO

import jp.vdmtools.VDM.*;
import java.util.*;
// ***** VDMTOOLS END Name=imports



public class B7a implements EvaluatePP {

// ***** VDMTOOLS START Name=vdmComp KEEP=NO
  static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();
// ***** VDMTOOLS END Name=vdmComp

// ***** VDMTOOLS START Name=sentinel KEEP=NO
  volatile Sentinel sentinel;
// ***** VDMTOOLS END Name=sentinel


// ***** VDMTOOLS START Name=B7aSentinel KEEP=NO
  class B7aSentinel extends Sentinel {

    public final int GetClass = 0;

    public final int nr_functions = 1;


    public B7aSentinel () throws CGException{}


    public B7aSentinel (EvaluatePP instance) throws CGException{
      init(nr_functions, instance);
    }

  }
// ***** VDMTOOLS END Name=B7aSentinel
;

// ***** VDMTOOLS START Name=evaluatePP KEEP=NO
  public Boolean evaluatePP (int fnr) throws CGException{
    return Boolean.TRUE;
  }
// ***** VDMTOOLS END Name=evaluatePP


// ***** VDMTOOLS START Name=setSentinel KEEP=NO
  public void setSentinel () {
    try{
      sentinel = new B7aSentinel(this);
    }
    catch (CGException e) {
      System.out.println(e.getMessage());
    }
  }
// ***** VDMTOOLS END Name=setSentinel


// ***** VDMTOOLS START Name=B7a KEEP=NO
  public B7a () {
    try{
      setSentinel();
    }
    catch (Throwable e) {
      System.out.println(e.getMessage());
    }
  }
// ***** VDMTOOLS END Name=B7a


// ***** VDMTOOLS START Name=GetClass KEEP=YES
  public String GetClass () throws CGException{
    sentinel.entering(((B7aSentinel) sentinel).GetClass);
    try{
          return this.getClass().getName();
    }
    finally {
      sentinel.leaving(((B7aSentinel) sentinel).GetClass);
    }
  }
// ***** VDMTOOLS END Name=GetClass

}
;
