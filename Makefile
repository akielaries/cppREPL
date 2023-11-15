CXX		= g++
FLGS	= -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare
SRCS	= cppREPL.cpp
BIN		= cppREPL

compile:
	$(CXX) $(FLGS) $(SRCS) -lncurses -o $(BIN)

clean:
	rm -f $(BIN) temp_code.cpp temp_executable

