CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

INCLUDES = -Iinclude -Iinclude/lab3_base -Iinclude/lab4_new

SRCDIR = src
INCDIR = include
TESTDIR = tests
SOURCES = $(SRCDIR)/main.cpp
HEADERS = $(wildcard $(INCDIR)/lab3_base/*.hpp) $(wildcard $(INCDIR)/lab4_new/*.hpp)
TPPS = $(wildcard $(INCDIR)/lab4_new/*.tpp)

all: main

main: $(SOURCES) $(HEADERS) $(TPPS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o main $(SOURCES)

test: $(TESTDIR)/tests.cpp $(HEADERS) $(TPPS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o test $(TESTDIR)/tests.cpp
	./test

clean:
	rm -f main test *.o

.PHONY: all test clean