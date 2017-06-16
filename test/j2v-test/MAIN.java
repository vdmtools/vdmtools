//import jp.vdmtools.VDM.*;
//import java.io.*;

public class MAIN
{
    public static void main(String[] args) 
    {
        try 
        {
//          A obj = new A();
//          System.out.println(UTIL.toString(((A)(obj)).Test()));

            System.out.println(J2VUTIL.toString(new A().Test()));
        }
        catch (ClassCastException e){
            System.out.println("Run-Time Error: Another type was expected");
        }               
        catch (Exception e){
            System.out.println(e.getMessage());
        }               
    }
}
