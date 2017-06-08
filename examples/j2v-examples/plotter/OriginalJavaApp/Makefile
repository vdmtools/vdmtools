JAVAC = javac
RM = rm

all: MyException.java Parser.java Calculator.java Curve.java Plotter.java
	$(JAVAC) -d . -classpath . MyException.java Parser.java Calculator.java Curve.java Plotter.java

clean:
	$(RM) -rf gui/*.class core/*.class