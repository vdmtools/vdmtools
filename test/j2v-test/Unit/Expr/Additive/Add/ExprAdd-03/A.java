class  A{
  public char[][] Test()
  {
    char res[][]={null,null,null};
    String str;

    str = 1 + " " + 3 + 4 + "abcd";
    res[0]=str.toCharArray();

    str = 1 + 3 + 4 + "abcd";
    res[1]=str.toCharArray();

    str = " " + 3 + 4 + "abcd";
    res[2]=str.toCharArray();

    return res;
//    return str.toCharArray();
  }
}

/*
Output:

"1 34abcd"
"8abcd"
" 34abcd"

*/