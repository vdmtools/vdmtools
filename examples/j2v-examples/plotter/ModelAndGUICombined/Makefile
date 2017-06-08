JAVAC = javac
RM = rm

all: ../OriginalJavaApp/MyException.java TB.java Parser.java Calculator.java ../OriginalJavaApp/Curve.java Plotter.java
	$(JAVAC) -d . -classpath .:../../../../api/corba/ToolboxAPI.jar ../OriginalJavaApp/MyException.java TB.java Parser.java Calculator.java ../OriginalJavaApp/Curve.java Plotter.java 

clean:
	$(RM) -rf gui/*.class core/*.class TB/*.class