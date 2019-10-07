package core;

import java.util.*;

public class Calculator extends Stack {
	public Calculator(){
	}

	public double Pop () {
		return ((Double)pop()).doubleValue();
	}

	public double calculate(String prg, double t) throws MyException {
		Lex lx = new Lex(prg);

		empty();
		
		boolean cont = true;
		while(cont) {
			int lex = lx.getLex();
			switch(lex) {
				case Lex.ARG: 
					push(Double.valueOf(t));
				break;
				case Lex.e: 
					push(Double.valueOf(Math.E));
				break;
				case Lex.pi: 
					push(Double.valueOf(Math.PI));
				break;
				case Lex.ABS: 
					push(Double.valueOf(Math.abs(Pop())));
				break;
				case Lex.ACOS: 
					push(Double.valueOf(Math.acos(Pop())));
				break;
				case Lex.ASIN: 
					push(Double.valueOf(Math.asin(Pop())));
				break;
				case Lex.ATAN: 
					push(Double.valueOf(Math.atan(Pop())));
				break;
				case Lex.CEIL: 
					push(Double.valueOf(Math.ceil(Pop())));
				break;
				case Lex.COS: 
					push(Double.valueOf(Math.cos(Pop())));
				break;
				case Lex.EXP: 
					push(Double.valueOf(Math.exp(Pop())));
				break;
				case Lex.FLOOR: 
					push(Double.valueOf(Math.floor(Pop())));
				break;
				case Lex.LOG: 
					push(Double.valueOf(Math.log(Pop())));
				break;
				case Lex.SIN: 
					push(Double.valueOf(Math.sin(Pop())));
				break;
				case Lex.SQRT: 
					push(Double.valueOf(Math.sqrt(Pop())));
				break;
				case Lex.TAN: 
					push(Double.valueOf(Math.tan(Pop())));
				break;
				case Lex.NUM:
					push(Double.valueOf(lx.number));
				break;
				case Lex.PLUS: 
					push(Double.valueOf(Pop()+Pop()));
				break;
				case Lex.MINUS: 
					push(Double.valueOf(-Pop()+Pop()));
				break;
				case Lex.MUL: 
					push(Double.valueOf(Pop()*Pop()));
				break;
				case Lex.DIV: 
					push(Double.valueOf((1./Pop())*Pop()));
				break;
				case Lex.POW:
					double pwr = Pop(); 
					push(Double.valueOf(Math.pow(Pop(),pwr)));
				break;
				case Lex.UMINUS:
					push(Double.valueOf(-Pop()));
				break;
				case Lex.EOL:
					cont = false;
				break;
			}
		}
		return Pop();
	}
}
