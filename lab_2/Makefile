CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

main: main.cpp ui.h tests.h array_sequence.h list_sequence.h sequence.h \
      dynamic_array.h linked_list.h iterators.h option.h exceptions.h
	$(CXX) $(CXXFLAGS) -o main main.cpp

clean:
	rm -f main

.PHONY: clean