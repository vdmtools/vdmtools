class  A{
  public char [][] Test()
  {
    char cc[][]=
    {
      String.valueOf('t').toCharArray(),
      String.valueOf(new char[]{'t','f'}).toCharArray(),
      String.valueOf(true).toCharArray(),
      String.valueOf(Character.MIN_VALUE).toCharArray(),
    };
    
    return cc;
  }
}