all: compiler

compiler: compiler.cpp
	g++ -std=c++11 -o compiler compiler.cpp

clean:
	rm -f *.o compiler
