//
// Narrowing Primitive Conversions to char
//
class  A{
  public char [] Test()
  {
/*
byte to char 
short to byte or char 
char to byte or short 
int to byte, short, or char 
long to byte, short, char, or int 
float to byte, short, char, int, or long 
double to byte, short, char, int, long, or float 
*/
    char   c='A';
//    byte   b=65;
//    short  s=65;
//    int    i=65;
//    long   l=65;
//    float  f=65.5f;
//    double d=65.5;

    char cc[] = {
      (char)c,
//      (char)b,
//      (char)s,
//      (char)i,
//      (char)l,
//      (char)f,
//      (char)d
    };
   
    return cc;
  }
}
