CXX		= g++
FLGS	= -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare
SRCS	= test.cpp
BIN		= cppREPL

compile:
	$(CXX) $(FLGS) $(SRCS) -o $(BIN)

clean:
	rm -f $(BIN)

