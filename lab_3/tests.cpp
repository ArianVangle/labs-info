#include "tests.h"

#include <iostream>

#include "array_sequence.h"
#include "deque_segmented.h"
#include "dynamic_array.h"
#include "hanoi.h"
#include "icollection.h"
#include "iterators.h"
#include "linked_list.h"
#include "list_sequence.h"
#include "map_reduce.h"
#include "option.h"
#include "ui.h"
#include "utils.h"

void Lab3TestRunner::TestDeque() {
	PrintTestHeader("DequeSegmented");
	DequeSegmented<int> deque(4);
	deque.PushBack(1);
	deque.PushBack(2);
	deque.PushBack(3);
	deque.PushFront(0);
	Assert(deque.GetLength() == 4, "DequeSegmented Length");
	Assert(deque.GetFront() == 0, "DequeSegmented GetFront");
	Assert(deque.GetBack() == 3, "DequeSegmented GetBack");

	int popped = deque.PopFront();
	Assert(popped == 0, "DequeSegmented PopFront");
	Assert(deque.GetLength() == 3, "DequeSegmented Length After Pop");

	Sequence<int> *dequeCloned = deque.Clone();
	Assert(dequeCloned->GetLength() == 3, "DequeSegmented Clone Length");
	delete dequeCloned;

	Sequence<int> *dequeMapped = deque.Map<int>([](int x) { return x * 2; });
	Assert(dequeMapped->Get(0) == 2, "DequeSegmented Map");
	delete dequeMapped;

	Sequence<int> *dequeFiltered = deque.Where([](int x) { return x > 1; });
	Assert(dequeFiltered->GetLength() == 2, "DequeSegmented Where");
	delete dequeFiltered;

	DequeSegmented<int> sortDeque(4);
	sortDeque.PushBack(3);
	sortDeque.PushBack(1);
	sortDeque.PushBack(4);
	sortDeque.PushBack(2);
	sortDeque.Sort();

	Assert(sortDeque.Get(0) == 1, "DequeSegmented Sort[0]");
	Assert(sortDeque.Get(3) == 4, "DequeSegmented Sort[3]");

	DequeSegmented<int> deque1(4);
	deque1.PushBack(1);
	DequeSegmented<int> deque2(4);
	deque2.PushBack(2);

	Sequence<int> *dequeConcatenated = deque1.Concat(deque2);
	Assert(dequeConcatenated->GetLength() == 2, "DequeSegmented Concat Length");
	delete dequeConcatenated;

	Sequence<int> *dequeSubseq = deque.GetSubsequence(0, 1);
	Assert(dequeSubseq->GetLength() == 2, "DequeSegmented Subsequence Length");
	delete dequeSubseq;
}

void Lab3TestRunner::TestHanoiAndRing() {
	PrintTestHeader("Hanoi Tower");
	HanoiTower hanoi(3);
	Assert(hanoi.GetDiskCount() == 3, "Hanoi Disk Count");
	Assert(hanoi.GetTower(0)->GetLength() == 3, "Hanoi Initial State");
	hanoi.Solve();

	Assert(hanoi.IsComplete(), "Hanoi Complete");
	Assert(hanoi.GetMoveCount() == 7, "Hanoi Move Count (2^3 - 1)");
	HanoiTower hanoi4(4);
	hanoi4.Solve();
    
	Assert(hanoi4.GetMoveCount() == 15, "Hanoi Move Count (2^4 - 1)");

	PrintTestHeader("Ring");
	Ring r1(3, "\033[91m");
	Ring r2(5, "\033[92m");
	Assert(r1.GetSize() == 3, "Ring Size");
	Assert(r1 < r2, "Ring Comparison");
	Assert(r1 != r2, "Ring Inequality");
}
void Lab3TestRunner::RunAll() {
    PrintTestHeader("ЗАПУСК ВСЕХ МОДУЛЬНЫХ ТЕСТОВ");
    
    TestDynamicArray();
    TestLinkedList();
    
    TestMutableArraySequence();
    TestImmutableArraySequence();
    TestMutableListSequence();
    TestSequenceOperators();
    
    TestMapOperation();
    TestWhereOperation();
    TestReduceOperation();
    
    TestZipUnzip();
    TestSplitOperation();
    TestSliceOperation();
    TestFromAndConcat();
    TestFindAndOption();
    TestIterators();
    TestSubsequenceAndClone();
    TestExceptionsAndEdges();
    
    TestDeque();
    TestHanoiAndRing();
    
    PrintSummary();
}