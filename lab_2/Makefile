CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

main: main.cpp tests.cpp ui.h tests.h array_sequence.h list_sequence.h sequence.h \
      dynamic_array.h linked_list.h iterators.h option.h exceptions.h utils.h \
      icollection.h map_reduce.h
	$(CXX) $(CXXFLAGS) -o main main.cpp tests.cpp

clean:
	rm -f main

.PHONY: clean