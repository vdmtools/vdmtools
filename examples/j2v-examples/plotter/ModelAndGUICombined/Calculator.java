package core;

import TB.*;

import jp.vdmtools.api.*;
import jp.vdmtools.api.corba.ToolboxAPI.*;
import jp.vdmtools.api.corba.VDM.*;

public class Calculator
{
	public Calculator(){
    if(TB.interpreter==null) return;
    try {
      // synchronized
      name = "c" + Calculator.idx;
      Calculator.idx++;
      // end synchronized
      TB.interpreter.EvalCmd("cr "+name+":=new Calculator()");
    }
    catch(APIError e) {
     ;
    }
	}

	public double calculate(String prg, double t) throws MyException {
    if(TB.interpreter==null) throw new MyException("Toolbox error!");
    String res = null;
    String arg = null;
    try {
      arg = name+".calculate(new JavaLangString("+prg+"),"+t+")";
      res =
        TB.interpreter
          .EvalExpression((short)0,arg).ToAscii();
    }
    catch(APIError e) {
     throw new MyException("Calculator.calculate: Toolbox error! expr="+arg);
    }
    System.out.println(arg+" : "+res+"\n");
    return Double.valueOf(res).doubleValue();
	}

	private String name = null;
      private static int idx = 0;
}
