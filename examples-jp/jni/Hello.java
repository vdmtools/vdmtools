package jni.test.sample;

import jni.test.sample.SomeException;

public class Hello {
  public int x = 10;
  public int y = 5;
  public static void main(String args[]) {
    System.out.println("Hello!");
  }

  public void Hello2() {
    System.out.println("Hello World!");
  }

  public void sayHello() {
    System.out.println("Hello Again!");
  }

  public int add(int a, int b) {
    return a + b;
  } 

  public int addex(int a, int b) throws SomeException  {
//  public int addex(int a, int b) {
    return addex2(a, b);
  } 

  public int addex2(int a, int b) throws SomeException  {
    return addex3(a, b);
  }

  public int addex3(int a, int b) throws SomeException  {
    if (true)
      throw new SomeException("exception !!");
    return a + b;
  }

  public static int adds(int a, int b) {
    return a + b;
  } 

  public char [] echo(char [] in)
  {
    String str = String.valueOf(in);
    str += "hello海";
    return str.toCharArray();
  }

  public java.lang.String echo2(java.lang.String in)
  {
    return in + "hello空";
  }

  public java.lang.String echo3(java.lang.String in) throws Exception
  {
    if (true)
      throw new Exception("exception is occured!!");
    return in + "hello空";
  }
}

