class  A{
  public char [] Test()
  {
    String str;

    str = "abcd" + "1234";
    str = str + str;
//!    str += "abcd";

    str = str + "abcd";

    return str.toCharArray();
  }
}