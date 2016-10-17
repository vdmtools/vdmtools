all: random

random: random.cc random.h
	g++ -DTESTRAND -o $@ $<

clean:
	rm -rf random jp *.class

test: random java
	./$<

java: RANDOM.java TestRandom.java
	javac -d . $^
	java -cp . TestRandom
