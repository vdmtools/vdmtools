package jp.vdmtools.VDM;

/**
 * 
 */

public class NotSupportedConstructException extends CGException
{
  public NotSupportedConstructException() {
    super("The construct is not supported.");
  }

  public NotSupportedConstructException(Object obj) {
    super(obj.toString()); 
  }
  
  public NotSupportedConstructException(Throwable cause) {
    super(cause);
  }
}
