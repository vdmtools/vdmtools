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
					push(new Double(t));
				break;
				case Lex.e: 
					push(new Double(Math.E));
				break;
				case Lex.pi: 
					push(new Double(Math.PI));
				break;
				case Lex.ABS: 
					push(new Double(Math.abs(Pop())));
				break;
				case Lex.ACOS: 
					push(new Double(Math.acos(Pop())));
				break;
				case Lex.ASIN: 
					push(new Double(Math.asin(Pop())));
				break;
				case Lex.ATAN: 
					push(new Double(Math.atan(Pop())));
				break;
				case Lex.CEIL: 
					push(new Double(Math.ceil(Pop())));
				break;
				case Lex.COS: 
					push(new Double(Math.cos(Pop())));
				break;
				case Lex.EXP: 
					push(new Double(Math.exp(Pop())));
				break;
				case Lex.FLOOR: 
					push(new Double(Math.floor(Pop())));
				break;
				case Lex.LOG: 
					push(new Double(Math.log(Pop())));
				break;
				case Lex.SIN: 
					push(new Double(Math.sin(Pop())));
				break;
				case Lex.SQRT: 
					push(new Double(Math.sqrt(Pop())));
				break;
				case Lex.TAN: 
					push(new Double(Math.tan(Pop())));
				break;
				case Lex.NUM:
					push(new Double(lx.number));
				break;
				case Lex.PLUS: 
					push(new Double(Pop()+Pop()));
				break;
				case Lex.MINUS: 
					push(new Double(-Pop()+Pop()));
				break;
				case Lex.MUL: 
					push(new Double(Pop()*Pop()));
				break;
				case Lex.DIV: 
					push(new Double((1./Pop())*Pop()));
				break;
				case Lex.POW:
					double pwr = Pop(); 
					push(new Double(Math.pow(Pop(),pwr)));
				break;
				case Lex.UMINUS:
					push(new Double(-Pop()));
				break;
				case Lex.EOL:
					cont = false;
				break;
			}
		}
		return Pop();
	}
}
