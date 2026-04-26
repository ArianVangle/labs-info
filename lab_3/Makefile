CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./from_lab_2

main: main.cpp tests.cpp ui.h from_lab_2/tests.h from_lab_2/array_sequence.h from_lab_2/list_sequence.h from_lab_2/sequence.h \
      from_lab_2/dynamic_array.h from_lab_2/linked_list.h from_lab_2/iterators.h from_lab_2/option.h from_lab_2/exceptions.h from_lab_2/utils.h \
      from_lab_2/icollection.h from_lab_2/map_reduce.h   ring.h deque_segmented.h hanoi.h
	$(CXX) $(CXXFLAGS) -o main main.cpp tests.cpp from_lab_2/tests.cpp

clean:
	rm -f main

.PHONY: clean