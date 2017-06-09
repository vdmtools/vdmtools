TEST_CLASSES=A.java
TEST_PACKAGE=B.java C.java

all: package compile

package:
	javac -classpath .:$$CLASSPATH -d . $(TEST_PACKAGE)

compile:
	javac -classpath .:$$CLASSPATH -d . $(TEST_CLASSES)
