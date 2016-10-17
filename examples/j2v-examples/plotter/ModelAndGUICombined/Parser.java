package core;

import TB.*;

import jp.vdmtools.api.*;
import jp.vdmtools.api.corba.ToolboxAPI.*;
import jp.vdmtools.api.corba.VDM.*;

public class Parser {

	public Parser()
	{
    if(TB.interpreter==null) return;
    try {
      // synchronized
      name = "p" + Parser.idx;
      Parser.idx++;
      // end synchronized
      TB.interpreter.EvalCmd("cr "+name+":=new Parser()");
    }
    catch(APIError e) {
      ;
    }
	}
/*
 *
 *	Grammar:
 *
 *		S : {-} T {+ T} | {-} T {- T}
 *		T : P {* P} | P {/ P}
 *		P : E {^ E}
 *		E : number | arg | func ( S ) | ( S )
 *		func : sin | cos | ...
 *		number : double
 *
 *	post-parse(str) == (str is valid => programm == postfix form of str)
 */
	public String parse(String str) throws MyException {
    if(TB.interpreter==null) throw new MyException("Toolbox error!");
    String programm = null;
    String arg = null;
    try {
      arg = name+".parse(new JavaLangString(\""+str+"\")).toSeqOfChar()";
      programm =
        TB.interpreter.EvalExpression((short)0,arg).ToAscii();
    }
    catch(APIError e) {
     throw new MyException("Calculator.calculate: Toolbox error! "+e.getMessage());
    }
    System.out.println(arg+" : "+programm+"\n");
    return programm;
	}

	private String name = null;
  private static int idx = 0;
}
