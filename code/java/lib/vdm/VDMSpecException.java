package jp.vdmtools.VDM;

/**
 * 
 */

public class VDMSpecException extends CGException
{
  private Object value = null;
  public VDMSpecException() {
    super("VDMSpecException");
  }

  public VDMSpecException(Object obj) {
    super("VDMSpecException: " + obj.toString()); 
    value = obj;
  }
  
  public VDMSpecException(Throwable cause) {
    super(cause);
  }

  public VDMSpecException(Object obj, Throwable cause) {
    super("VDMSpecException: " + obj.toString(), cause);
    value = obj;
  }

  public Object getValue() {
    return value;
  }
}
