package jni.test.sample;

public class SomeException extends Exception
{
  public SomeException(Throwable cause) {
    super(cause);
  }

  public SomeException(String message) {
    super(message);
  }
}
