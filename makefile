CXX = g++
CFLAGS = -Wall -Werror
CXXFLAGS = -std=c++11 $(CFLAGS)

all: runme

runme: runme.o
	$(CXX) $(CXXFLAGS) -o $@ $^

runme.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f main.o


test: runme
	./runme [1.0 0.0 0.0] [0.0 1.0 0.0] [0.0 0.0 1.0]
