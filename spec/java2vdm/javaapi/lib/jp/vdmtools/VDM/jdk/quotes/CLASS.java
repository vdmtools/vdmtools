//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Tue 30-Oct-2001 by the VDM++ JAVA Code Generator
// (v6.7.19 - Mon 29-Oct-2001)
//
// Supported compilers:
// jdk1.3
//

// ***** VDMTOOLS START Name=HeaderComment KEEP=NO

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=NO
package quotes;

// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=NO
// ***** VDMTOOLS END Name=imports



public class CLASS {

// ***** VDMTOOLS START Name=hc KEEP=NO
  static private int hc = 0;
// ***** VDMTOOLS END Name=hc


// ***** VDMTOOLS START Name=CLASS KEEP=NO
  public CLASS () {
    if (hc == 0) 
      hc = super.hashCode();
  }
// ***** VDMTOOLS END Name=CLASS


// ***** VDMTOOLS START Name=hashCode KEEP=NO
  public int hashCode () {
    return hc;
  }
// ***** VDMTOOLS END Name=hashCode


// ***** VDMTOOLS START Name=equals KEEP=NO
  public boolean equals (Object obj) {
    return obj instanceof CLASS;
  }
// ***** VDMTOOLS END Name=equals


// ***** VDMTOOLS START Name=toString KEEP=NO
  public String toString () {
    return "<CLASS>";
  }
// ***** VDMTOOLS END Name=toString

}
;
