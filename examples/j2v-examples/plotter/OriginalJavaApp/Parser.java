package core;

import java.util.*;

class Lex {
	public static final int NULL		= -2;
	public static final int EOL			= -1;

	public static final int PLUS		= 0;
	public static final int MINUS		= 1;
	public static final int MUL			= 2;
	public static final int DIV			= 3;
	public static final int POW			= 4;
	public static final int LEFT		= 5;
	public static final int RIGHT		= 6;
	public static final int ARG			= 7;
	public static final int e			= 8;
	public static final int pi			= 9;

	public static final int ABS			= 10;
	public static final int ACOS		= 11;
	public static final int ASIN		= 12;
	public static final int ATAN		= 13;
	public static final int CEIL		= 14;
	public static final int COS			= 15;
	public static final int EXP			= 16;
	public static final int FLOOR		= 17;
	public static final int LOG			= 18;
	public static final int SIN			= 19;
	public static final int SQRT		= 20;
	public static final int TAN			= 21;

	public static final int UMINUS		= 22;

	public static final int NUM			= 23;

	public String lexem = null;
	public double number;
	
	static final String [] lexems = {
		"+",
		"-",
		"*",
		"/",
		"^",
		"(",
		")",
		"arg",
		"E", 
		"PI", 
		"abs",
		"acos",
		"asin",
		"atan",
		"ceil",
		"cos",
		"exp",
		"floor",
		"log",
		"sin",
		"sqrt",
		"tan",	
		"u-"	
	};

	String string = null;
	
	public Lex(String str) {
		string = str;
	}

	public int getLex() throws MyException {
		int lexcode = -1;
		int lexlen = 0;
		string = string.trim();

		if(string.equals("")) return -1;

		for(int i=0; i<lexems.length; i++) {
			if(string.startsWith(lexems[i])) {
				lexcode=i;
				lexlen=lexems[i].length();
				break;
			}
		}
		if(lexcode==-1) {
			if(Character.isDigit(string.charAt(0))) {
				int i;
				for(i=0; i<string.length() &&
					(Character.isDigit(string.charAt(i)) ||
					string.charAt(i)=='.'); i++);
				lexlen = i;
				lexcode = NUM;
				number = (Double.valueOf(string.substring(0, lexlen))).doubleValue();
			} else {
				throw new MyException("Unknown lexem, " + string);
			}
		}
		lexem = string.substring(0, lexlen);
		string = string.substring(lexlen, string.length());
		return lexcode;
	}
}

public class Parser {
	Lex lx = null;
	String programm = null;
	public Parser() {}
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
		lx = new Lex(str);
		programm = "";
		if(parseS() != Lex.EOL) throw new MyException("EOL expected.");
//		System.out.println(programm);
		return programm;
	}

	protected int parseS() throws MyException {
		int lex = lx.getLex();
		if(lex==Lex.MINUS) {
			lex = parseT(Lex.NULL);
			programm = programm + "u-" + " ";
		} else {
			lex = parseT(lex);
		}
		while(true) {
			if(lex!=Lex.PLUS && lex!=Lex.MINUS) return lex;
			String op = lx.lexem;
			lex = parseT(Lex.NULL);
			programm = programm + op + " ";
		}
	}
	
	protected int parseT(int lexem) throws MyException {
		int lex = parseP(lexem);
		while(true) {
			if(lex!=Lex.MUL && lex!=Lex.DIV) return lex;
			String op = lx.lexem;
			lex = parseP(Lex.NULL);
			programm = programm + op + " ";
		}
	}
	
	protected int parseP(int lexem) throws MyException {
		int lex = parseE(lexem);
		while(true) {
			if(lex!=Lex.POW) return lex;
			String op = lx.lexem;
			lex = parseE(Lex.NULL);
			programm = programm + op + " ";
		}
	}
	
	protected int parseE(int lexem) throws MyException {
		int lex = lexem;
		if( lex==Lex.NULL ) lex = lx.getLex();
		switch(lex) {
			case Lex.ARG:
			case Lex.NUM:
			case Lex.pi:
			case Lex.e:
				programm = programm + lx.lexem + " ";
			break;
			case Lex.LEFT:
				lex = parseS();
				if(lex!=Lex.RIGHT) {
					throw new MyException(") expected");
				}
			break;
			case Lex.ABS:
			case Lex.ACOS:
			case Lex.ASIN:
			case Lex.ATAN:
			case Lex.CEIL:
			case Lex.COS:
			case Lex.EXP:
			case Lex.FLOOR:
			case Lex.LOG:
			case Lex.SIN:
			case Lex.SQRT:
			case Lex.TAN:
				String op = lx.lexem;
				lex=lx.getLex();
				if(lex!=Lex.LEFT) {
					throw new MyException("( expected");
				}
				lex = parseS();
				if(lex!=Lex.RIGHT) {
					throw new MyException(") expected");
				}
				programm = programm + op + " ";
			break;
			case Lex.EOL:
				if(true) throw new MyException("Unexpected end");
      break;
			default:
				throw new MyException("Unexpected " + lx.lexem);
		}
		lex = lx.getLex();
		return lex;
	}
}




