//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Fri 27-May-2005 by the VDM++ JAVA Code Generator
// (v6.8.5 - Wed 25-May-2005 13:41:53)
//
// Supported compilers:
// jdk1.3
//

// ***** VDMTOOLS START Name=HeaderComment KEEP=NO

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=NO
// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=NO

import jp.vdmtools.VDM.*;
import java.util.*;
import jp.vdmtools.VDM.jdk.*;
// ***** VDMTOOLS END Name=imports



public class Card implements EvaluatePP {

// ***** VDMTOOLS START Name=vdmComp KEEP=NO
  static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();
// ***** VDMTOOLS END Name=vdmComp

// ***** VDMTOOLS START Name=code KEEP=NO
  private volatile Integer code = null;
// ***** VDMTOOLS END Name=code

// ***** VDMTOOLS START Name=cardId KEEP=NO
  private volatile Integer cardId = null;
// ***** VDMTOOLS END Name=cardId

// ***** VDMTOOLS START Name=accountId KEEP=NO
  private volatile Integer accountId = null;
// ***** VDMTOOLS END Name=accountId

// ***** VDMTOOLS START Name=sentinel KEEP=NO
  volatile Sentinel sentinel;
// ***** VDMTOOLS END Name=sentinel


// ***** VDMTOOLS START Name=CardSentinel KEEP=NO
  class CardSentinel extends Sentinel {

    public final int Create = 0;

    public final int GetCode = 1;

    public final int GetCardId = 2;

    public final int GetAccountId = 3;

    public final int nr_functions = 4;


    public CardSentinel () throws CGException {}


    public CardSentinel (EvaluatePP instance) throws CGException {
      init(nr_functions, instance);
    }

  }
// ***** VDMTOOLS END Name=CardSentinel
;

// ***** VDMTOOLS START Name=evaluatePP KEEP=NO
  public Boolean evaluatePP (int fnr) throws CGException {
    return new Boolean(true);
  }
// ***** VDMTOOLS END Name=evaluatePP


// ***** VDMTOOLS START Name=setSentinel KEEP=NO
  public void setSentinel () {
    try {
      sentinel = new CardSentinel(this);
    }
    catch (CGException e) {
      System.out.println(e.getMessage());
    }
  }
// ***** VDMTOOLS END Name=setSentinel


// ***** VDMTOOLS START Name=Card KEEP=NO
  public Card () throws CGException {
    try {
      setSentinel();
    }
    catch (Exception e){
      e.printStackTrace(System.out);
      System.out.println(e.getMessage());
    }
  }
// ***** VDMTOOLS END Name=Card


// ***** VDMTOOLS START Name=Create KEEP=NO
  public Card Create (final Integer c, final Integer cid, final Integer a) throws CGException {
    sentinel.entering(((CardSentinel) sentinel).Create);
    try {
      code = UTIL.NumberToInt(UTIL.clone(c));
      cardId = UTIL.NumberToInt(UTIL.clone(cid));
      accountId = UTIL.NumberToInt(UTIL.clone(a));
      return (Card) this;
    }
    finally {
      sentinel.leaving(((CardSentinel) sentinel).Create);
    }
  }
// ***** VDMTOOLS END Name=Create


// ***** VDMTOOLS START Name=GetCode KEEP=NO
  public Integer GetCode () throws CGException {
    sentinel.entering(((CardSentinel) sentinel).GetCode);
    try {
      return code;
    }
    finally {
      sentinel.leaving(((CardSentinel) sentinel).GetCode);
    }
  }
// ***** VDMTOOLS END Name=GetCode


// ***** VDMTOOLS START Name=GetAccountId KEEP=NO
  public Integer GetAccountId () throws CGException {
    sentinel.entering(((CardSentinel) sentinel).GetAccountId);
    try {
      return accountId;
    }
    finally {
      sentinel.leaving(((CardSentinel) sentinel).GetAccountId);
    }
  }
// ***** VDMTOOLS END Name=GetAccountId


// ***** VDMTOOLS START Name=GetCardId KEEP=NO
  public Integer GetCardId () throws CGException {
    sentinel.entering(((CardSentinel) sentinel).GetCardId);
    try {
      return cardId;
    }
    finally {
      sentinel.leaving(((CardSentinel) sentinel).GetCardId);
    }
  }
// ***** VDMTOOLS END Name=GetCardId

}
;
