package jp.vdmtools.VDM;

/**
 * 
 */

public class VDMRunTimeException extends CGException
{
  public VDMRunTimeException() {
    super("VDMRunTimeException");
  }

  public VDMRunTimeException(Object obj) {
    super(obj.toString()); 
  }
  
  public VDMRunTimeException(Throwable cause) {
    super(cause);
  }
}
