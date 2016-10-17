/////////////////////////////////////////////////////////////////////////////////////
//
//  interface EvaluatePP
//

//  This interface is used so that the sentinels can evaluate the
//  Permission predicates at will. It is necessary for the object state guards.
//  It must be used in all user classes implementing permission predicates
 

package jp.vdmtools.VDM;

import jp.vdmtools.VDM.CGException;

public interface EvaluatePP {

  public Boolean evaluatePP(int fnr) throws CGException;

}
  

