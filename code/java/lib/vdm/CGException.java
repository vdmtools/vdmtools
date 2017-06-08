package jp.vdmtools.VDM;

/**
 * is superclass of all exceptions thrown by the code generator
 */

public class CGException extends Exception {
  public CGException() {
    super();
  }

  public CGException(String message) {
    super(message);
  }
  
  public CGException(Throwable cause) {
    super(cause);
  }

  public CGException(String message, Throwable cause) {
    super(message, cause);
  }
}

