package core;

public class Curve extends Calculator{
	String programmX = null;
	String programmY = null;
	
	public Curve(String prgX, String prgY) {
		super();
		programmX = prgX;	
		programmY = prgY;
	}

	public double x(double t) throws MyException {
		return calculate(programmX, t);
	}

	public double y(double t) throws MyException {
		return calculate(programmY, t);
	}
}