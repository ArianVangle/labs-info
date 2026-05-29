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

void Lab3TestRunner::TestDequeBasicOps() {
    PrintTestHeader("DequeSegmented: Construction & Segment Logic");
    const int SEG_SIZE = 4;
    DequeSegmented<int> dq(SEG_SIZE);
    
    for (int i = 1; i <= SEG_SIZE; ++i) {
        dq.PushBack(i);
    }
    Assert(dq.GetSegmentCount() == 1, "Deque: 1 segment after filling capacity");
    Assert(dq.GetLength() == SEG_SIZE, "Deque: Length matches segment size");
    
    dq.PushBack(SEG_SIZE + 1);
    dq.PushBack(SEG_SIZE + 2);
    Assert(dq.GetSegmentCount() == 2, "Deque: 2 segments after overflow");
    Assert(dq.GetLength() == SEG_SIZE + 2, "Deque: Length increased correctly");
    
    Assert(dq.GetFront() == 1, "Deque: GetFront");
    Assert(dq.GetBack() == SEG_SIZE + 2, "Deque: GetBack");
    Assert(dq.Get(0) == 1 && dq.Get(SEG_SIZE + 1) == SEG_SIZE + 2, "Deque: Get by index");
}

void Lab3TestRunner::TestDequePopClearAndEdge() {
    PrintTestHeader("DequeSegmented: Pop, Clear & Empty State");
    DequeSegmented<int> dq(2);
    dq.PushBack(1); dq.PushBack(2); dq.PushBack(3);

    Assert(dq.PopFront() == 1, "Deque: PopFront value");
    Assert(dq.GetLength() == 2, "Deque: Length after PopFront");
    Assert(dq.GetFront() == 2, "Deque: New front");

    Assert(dq.PopBack() == 3, "Deque: PopBack value");
    Assert(dq.GetLength() == 1, "Deque: Length after PopBack");
    Assert(dq.GetBack() == 2, "Deque: New back");

    dq.Clear();
    Assert(dq.GetLength() == 0, "Deque: Clear resets length");
    Assert(dq.GetSegmentCount() == 0, "Deque: Clear resets segments");
    
    dq.PushBack(99);
    Assert(dq.GetFront() == 99 && dq.GetBack() == 99, "Deque: Push to empty works");
}

void Lab3TestRunner::TestDequeMapReduceIterators() {
    PrintTestHeader("DequeSegmented: Map, Reduce, Iterators");
    DequeSegmented<int> dq(4);
    dq.PushBack(1); dq.PushBack(2); dq.PushBack(3);

    // Map
    auto* mapped = dq.Map<int>([](int x) { return x * 10; });
    Assert(mapped->Get(0) == 10 && mapped->Get(2) == 30, "Deque: Map transformation");
    Assert(mapped->GetLength() == 3, "Deque: Map preserves length");
    delete mapped;

    // Where
    auto* filtered = dq.Where([](int x) { return x > 1; });
    Assert(filtered->GetLength() == 2, "Deque: Where filter count");
    Assert(filtered->Get(0) == 2, "Deque: Where correct values");
    delete filtered;

    // Reduce
    int sum = dq.Reduce([](int acc, int v) { return acc + v; }, 0);
    Assert(sum == 6, "Deque: Reduce sum");

    // Iterator
    auto* en = dq.GetEnumerator();
    bool iterOk = true; int expected = 1;
    while (en->MoveNext()) { if (en->Current() != expected++) iterOk = false; }
    delete en;
    Assert(iterOk, "Deque: Enumerator traversal");
}

void Lab3TestRunner::TestDequeSortConcatSubsequence() {
    PrintTestHeader("DequeSegmented: Sort, Concat, Subsequence");
    DequeSegmented<int> unsorted(3);
    unsorted.PushBack(5); unsorted.PushBack(2); unsorted.PushBack(8); unsorted.PushBack(1);
    unsorted.Sort();
    Assert(unsorted.Get(0) == 1 && unsorted.Get(3) == 8, "Deque: Sort ascending");

    DequeSegmented<int> dq1(2); dq1.PushBack(1); dq1.PushBack(2);
    DequeSegmented<int> dq2(2); dq2.PushBack(3); dq2.PushBack(4);
    auto* conc = dq1.Concat(dq2);
    Assert(conc->GetLength() == 4 && conc->Get(2) == 3, "Deque: Concat order & length");
    delete conc;

    auto* sub = unsorted.GetSubsequence(1, 3);
    Assert(sub->GetLength() == 3 && sub->Get(0) == 2, "Deque: GetSubsequence");
    delete sub;

    DequeSegmented<int> pat(2); pat.PushBack(2); pat.PushBack(5);
    unsorted.PushBack(5); unsorted.PushBack(9);
    Assert(unsorted.FindSubsequence(pat) == 1, "Deque: FindSubsequence");
}

void Lab3TestRunner::TestDequeExceptions() {
    PrintTestHeader("DequeSegmented: Exceptions & Bounds");
    DequeSegmented<int> dq(2);
    dq.PushBack(10);

    try { dq.Get(-1); Assert(false, "Deque: Get(-1) should throw"); }
    catch (...) { Assert(true, "Deque: Get(-1) throws"); }

    try { dq.Get(5); Assert(false, "Deque: Get(out) should throw"); }
    catch (...) { Assert(true, "Deque: Get(out) throws"); }

    try { dq.Set(5, 99); Assert(false, "Deque: Set(out) should throw"); }
    catch (...) { Assert(true, "Deque: Set(out) throws"); }

    dq.Clear();
    try { dq.GetFront(); Assert(false, "Deque: Front on empty should throw"); }
    catch (...) { Assert(true, "Deque: Front on empty throws"); }

    try { dq.PopBack(); Assert(false, "Deque: Pop on empty should throw"); }
    catch (...) { Assert(true, "Deque: Pop on empty throws"); }
}

void Lab3TestRunner::TestHanoiConstruction() {
    PrintTestHeader("HanoiTower: Construction & State");
    HanoiTower h(3);
    Assert(h.GetDiskCount() == 3, "Hanoi: Disk count");
    Assert(h.GetMoveCount() == 0, "Hanoi: Initial moves");
    Assert(h.GetTower(0)->GetLength() == 3, "Hanoi: Start tower full");
    Assert(h.GetTower(1)->GetLength() == 0, "Hanoi: Aux tower empty");
    Assert(h.GetTower(2)->GetLength() == 0, "Hanoi: Target tower empty");

    try { h.GetTower(5); Assert(false, "Hanoi: Invalid tower index"); }
    catch (...) { Assert(true, "Hanoi: Invalid tower throws"); }
}

void Lab3TestRunner::TestHanoiSolvingLogic() {
    PrintTestHeader("HanoiTower: Solving Logic");
    HanoiTower h3(3);
    h3.Solve();
    Assert(h3.IsComplete(), "Hanoi(3): IsComplete after Solve");
    Assert(h3.GetMoveCount() == 7, "Hanoi(3): Move count = 2^3-1");

    HanoiTower h4(4);
    h4.Solve();
    Assert(h4.IsComplete(), "Hanoi(4): IsComplete after Solve");
    Assert(h4.GetMoveCount() == 15, "Hanoi(4): Move count = 2^4-1");

    HanoiTower hCustom(3, 2); 
    hCustom.SolveCustom(2, 1); 
    
    Assert(hCustom.IsComplete(), "Hanoi: Custom solve complete");
    Assert(hCustom.GetTower(1)->GetLength() == 0, "Hanoi: Custom start empty (Tower 2)");
    Assert(hCustom.GetTower(0)->GetLength() == 3, "Hanoi: Custom target full (Tower 1)");
    Assert(hCustom.GetMoveCount() == 7, "Hanoi: Custom move count");
}

void Lab3TestRunner::TestHanoiHistoryAndStats() {
    PrintTestHeader("HanoiTower: History & Statistics");
    HanoiTower h(3);
    h.Solve();
    
    auto* hist = h.GetMoveHistory();
    Assert(hist->GetLength() == 7, "Hanoi: History length matches moves");
    Assert(hist->Get(0).find("Ход 1") != std::string::npos, "Hanoi: History format valid");
    Assert(hist->Get(6).find("Ход 7") != std::string::npos, "Hanoi: Last move logged");
}

void Lab3TestRunner::TestHanoiExceptions() {
    PrintTestHeader("HanoiTower: Exceptions & Invalid Moves");
    HanoiTower h(3);
    
    try { h.Move(0, 5); Assert(false, "Hanoi: Invalid tower index"); }
    catch (...) { Assert(true, "Hanoi: Invalid tower throws"); }

    try { h.Move(1, 2); Assert(false, "Hanoi: Move from empty tower"); }
    catch (...) { Assert(true, "Hanoi: Move from empty throws"); }

    h.Move(0, 1);
    try { h.Move(0, 1); Assert(false, "Hanoi: Larger on smaller"); }
    catch (...) { Assert(true, "Hanoi: Larger on smaller throws"); }
}

void Lab3TestRunner::TestRingOperations() {
    PrintTestHeader("Ring: Properties & Comparisons");
    Ring r1(3, "\033[91m");
    Ring r2(5, "\033[92m");
    Ring r3(3, "\033[94m");

    Assert(r1.GetSize() == 3, "Ring: Size getter");
    Assert(r1.GetColor() == "\033[91m", "Ring: Color getter");
    Assert(r1.GetId() != r2.GetId(), "Ring: Unique IDs");
    Assert(r1.GetId() + 1 == r2.GetId() || r1.GetId() < r2.GetId(), "Ring: IDs increment");

    Assert(r1 < r2, "Ring: operator<");
    Assert(r2 > r1, "Ring: operator>");
    Assert(r1 == r3, "Ring: operator== (size only)");
    Assert(r1 != r2, "Ring: operator!=");
    
    r1.SetSize(10); r1.SetColor("RED");
    Assert(r1.GetSize() == 10 && r1.GetColor() == "RED", "Ring: Setters work");
}

void Lab3TestRunner::RunAll() {
    PrintTestHeader("ЗАПУСК ВСЕХ МОДУЛЬНЫХ ТЕСТОВ (LAB 2 + LAB 3)");
    
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
    
    TestDequeBasicOps();
    TestDequePopClearAndEdge();
    TestDequeMapReduceIterators();
    TestDequeSortConcatSubsequence();
    TestDequeExceptions();
    
    TestHanoiConstruction();
    TestHanoiSolvingLogic();
    TestHanoiHistoryAndStats();
    TestHanoiExceptions();
    TestRingOperations();
    
    PrintSummary();
}