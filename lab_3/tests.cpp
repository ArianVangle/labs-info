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

void TestRunner::SetColor(const std::string& color) { std::cout << color; }

void TestRunner::ResetColor() { std::cout << COLOR_RESET; }

void TestRunner::PrintChars(char c, int count) {
    for (int i = 0; i < count; i++) std::cout << c;
}

void TestRunner::PrintLine(char c) {
    SetColor(COLOR_INFO);
    PrintChars(c, WIDTH);
    std::cout << std::endl;
    ResetColor();
}

void TestRunner::PrintTestHeader(const std::string& header) {
    std::cout << std::endl;
    SetColor(COLOR_INFO);
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;

    int visibleLen = VisibleLength(header);
    int padding = (WIDTH - 2 - visibleLen) / 2;

    std::cout << "|";
    PrintChars(' ', padding);
    std::cout << header;
    PrintChars(' ', WIDTH - 2 - visibleLen - padding);
    std::cout << "|" << std::endl;

    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    ResetColor();
}

void TestRunner::PrintTestResult(bool success, const std::string& testName) {
    if (success) {
        SetColor(COLOR_PASS);
        std::cout << "| ";
        std::cout << "✓ [PASS] ";
        std::cout << testName;

        int textLen = 9 + VisibleLength(testName);
        int padding = WIDTH - 2 - textLen - 1;

        PrintChars(' ', padding);
        std::cout << "|" << std::endl;
        ResetColor();
        passed++;
    } else {
        SetColor(COLOR_FAIL);
        std::cout << "| ";
        std::cout << "✗ [FAIL] ";
        std::cout << testName;

        int textLen = 9 + VisibleLength(testName);
        int padding = WIDTH - 2 - textLen - 1;

        PrintChars(' ', padding);
        std::cout << "|" << std::endl;
        ResetColor();
        failed++;
    }
}

void TestRunner::PrintSummary() {
    std::cout << std::endl;
    SetColor(COLOR_INFO);
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;

    std::string title = "ИТОГОВЫЙ ОТЧЁТ";
    int visibleTitle = VisibleLength(title);
    int titlePad = (WIDTH - 2 - visibleTitle) / 2;

    std::cout << "|";
    PrintChars(' ', titlePad);
    std::cout << title;
    PrintChars(' ', WIDTH - 2 - visibleTitle - titlePad);
    std::cout << "|" << std::endl;

    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;

    SetColor(COLOR_PASS);
    std::string passedText = "Пройдено тестов: " + std::to_string(passed);
    int visiblePassed = VisibleLength(passedText);
    int passedPad = WIDTH - 3 - visiblePassed;

    std::cout << "| ";
    std::cout << passedText;
    PrintChars(' ', passedPad);
    std::cout << "|" << std::endl;

    SetColor(COLOR_FAIL);
    std::string failedText = "Провалено тестов: " + std::to_string(failed);
    int visibleFailed = VisibleLength(failedText);
    int failedPad = WIDTH - 3 - visibleFailed;

    std::cout << "| ";
    std::cout << failedText;
    PrintChars(' ', failedPad);
    std::cout << "|" << std::endl;

    SetColor(COLOR_INFO);
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;

    int total = passed + failed;
    double percentage = total > 0 ? (double)passed / total * 100 : 0;

    std::string percentText = "Успешность: " + std::to_string((int)percentage) + "%";
    int visiblePercent = VisibleLength(percentText);
    int percentPad = WIDTH - 3 - visiblePercent;

    std::string color = percentage >= 80 ? COLOR_PASS : (percentage >= 50 ? COLOR_WARN : COLOR_FAIL);
    SetColor(color);

    std::cout << "| ";
    std::cout << percentText;
    PrintChars(' ', percentPad);
    std::cout << "|" << std::endl;

    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    ResetColor();
}

TestRunner::TestRunner(ConsoleUI* ui) : passed(0), failed(0), ui(ui) {}

void TestRunner::Assert(bool condition, const std::string& testName) { PrintTestResult(condition, testName); }

void TestRunner::RunAll() {
    PrintTestHeader("ЗАПУСК ВСЕХ МОДУЛЬНЫХ ТЕСТОВ");

    // ========== DynamicArray Tests ==========
    PrintTestHeader("DynamicArray");
    int arrData[] = {1, 2, 3};
    DynamicArray<int> da(arrData, 3);
    Assert(da.Get(0) == 1, "DynamicArray Get(0)");
    Assert(da.Get(1) == 2, "DynamicArray Get(1)");
    Assert(da.Get(2) == 3, "DynamicArray Get(2)");
    Assert(da.GetSize() == 3, "DynamicArray Size");
    da.Set(0, 10);
    Assert(da.Get(0) == 10, "DynamicArray Set");

    try {
        da.Get(5);
        Assert(false, "DynamicArray Exception");
    } catch (...) {
        Assert(true, "DynamicArray Exception");
    }

    da.Resize(5);
    Assert(da.GetSize() == 5, "DynamicArray Resize");

    // ========== LinkedList Tests ==========
    PrintTestHeader("LinkedList");
    LinkedList<int> ll;
    ll.Append(1);
    ll.Append(2);
    ll.Append(3);
    Assert(ll.Get(0) == 1, "LinkedList Append/Get");
    Assert(ll.GetFirst() == 1, "LinkedList GetFirst");
    Assert(ll.GetLast() == 3, "LinkedList GetLast");
    Assert(ll.GetLength() == 3, "LinkedList Length");

    ll.Prepend(0);
    Assert(ll.GetFirst() == 0, "LinkedList Prepend");

    ll.InsertAt(5, 2);
    Assert(ll.Get(2) == 5, "LinkedList InsertAt");

    // ========== Sequence Mutable Tests ==========
    PrintTestHeader("Sequence (Mutable)");
    int seqData[] = {5, 6};
    Sequence<int>* seq = new MutableArraySequence<int>(seqData, 2);
    Assert(seq->GetLength() == 2, "Sequence Length");

    IEnumerator<int>* seqEn = seq->GetEnumerator();
    int expected = 5;
    while (seqEn->MoveNext()) {
        Assert(seqEn->Current() == expected++, "Sequence Iterator");
    }
    delete seqEn;

    Sequence<int>* newSeq = seq->Append(7);
    Assert(newSeq->GetLength() == 3, "Sequence Append Mutable");
    Assert(seq->GetLength() == 3, "Sequence Mutable State Change");
    delete seq;

    // ========== Sequence Immutable Tests ==========
    PrintTestHeader("Sequence (Immutable)");
    Sequence<int>* iSeq = new ImmutableArraySequence<int>(seqData, 2);
    Sequence<int>* iNewSeq = iSeq->Append(8);
    Assert(iNewSeq->GetLength() == 3, "Sequence Append Immutable New");
    Assert(iSeq->GetLength() == 2, "Sequence Immutable State Preserved");

    Option<int> opt = iSeq->TryGet(0);
    Assert(opt.IsSome(), "Option Some");
    Assert(opt.GetValue() == 5, "Option GetValue");
    Option<int> optNone = iSeq->TryGet(10);
    Assert(optNone.IsNone(), "Option None");

    delete iSeq;
    delete iNewSeq;

    // ========== Map-Reduce Tests ==========
    PrintTestHeader("Map-Reduce Operations");
    MutableArraySequence<int>* mapSeq = new MutableArraySequence<int>(seqData, 2);
    Sequence<int>* mapped = mapSeq->Map<int>([](int x) { return x * 2; });

    IEnumerator<int>* mapEn = mapped->GetEnumerator();
    int mapExpected = 10;
    while (mapEn->MoveNext()) {
        Assert(mapEn->Current() == mapExpected, "Map Iterator");
        mapExpected += 2;
    }
    delete mapEn;

    delete mapSeq;
    delete mapped;

    int whereData[] = {1, 2, 3, 4, 5};
    Sequence<int>* whereSeq = new MutableArraySequence<int>(whereData, 5);
    Sequence<int>* filtered = whereSeq->Where([](int x) { return x > 2; });
    Assert(filtered->GetLength() == 3, "Where Filter (x>2)");

    IEnumerator<int>* whereEn = filtered->GetEnumerator();
    int whereExpected = 3;
    while (whereEn->MoveNext()) {
        Assert(whereEn->Current() == whereExpected++, "Where Iterator");
    }
    delete whereEn;

    delete whereSeq;
    delete filtered;

    Sequence<int>* reduceSeq = new MutableArraySequence<int>(whereData, 5);
    int sum = reduceSeq->Reduce([](int acc, int x) { return acc + x; }, 0);
    Assert(sum == 15, "Reduce Sum (1+2+3+4+5=15)");
    int product = reduceSeq->Reduce([](int acc, int x) { return acc * x; }, 1);
    Assert(product == 120, "Reduce Product (1*2*3*4*5=120)");
    delete reduceSeq;

    // ========== Iterator Tests ==========
    PrintTestHeader("Iterators");
    Sequence<int>* iterSeq = new MutableArraySequence<int>(seqData, 2);
    IEnumerator<int>* en = iterSeq->GetEnumerator();
    Assert(en->MoveNext(), "Iterator MoveNext");
    Assert(en->Current() == 5, "Iterator Current");
    en->Reset();
    Assert(en->MoveNext(), "Iterator Reset + MoveNext");
    delete iterSeq;
    delete en;

    // ========== Operator[] Tests ==========
    PrintTestHeader("Operator Overloading");
    Sequence<int>* opSeq = new MutableArraySequence<int>(seqData, 2);
    Assert((*opSeq)[0] == 5, "Operator[] [0]");
    Assert((*opSeq)[1] == 6, "Operator[] [1]");
    delete opSeq;

    // ========== Subsequence Tests ==========
    PrintTestHeader("Subsequence");
    Sequence<int>* subSeq = new MutableArraySequence<int>(whereData, 5);
    Sequence<int>* sub = subSeq->GetSubsequence(1, 3);
    Assert(sub->GetLength() == 3, "Subsequence Length");

    IEnumerator<int>* subEn = sub->GetEnumerator();
    int subExpected = 2;
    while (subEn->MoveNext()) {
        Assert(subEn->Current() == subExpected++, "Subsequence Iterator");
    }
    delete subEn;

    delete subSeq;
    delete sub;

    // ========== Sequence Set Tests ==========
    PrintTestHeader("Sequence Set");
    int setData[] = {1, 2, 3};
    Sequence<int>* setSeq = new MutableArraySequence<int>(setData, 3);

    setSeq->Set(1, 999);
    Assert(setSeq->Get(1) == 999, "Sequence Set(1, 999)");

    setSeq->Set(0, 100);
    Assert(setSeq->Get(0) == 100, "Sequence Set(0, 100)");

    delete setSeq;
    // ========== ListSequence Tests ==========
    PrintTestHeader("ListSequence");
    Sequence<int>* listSeq = new MutableListSequence<int>(seqData, 2);
    Assert(listSeq->GetLength() == 2, "ListSequence Length");

    IEnumerator<int>* listEn = listSeq->GetEnumerator();
    int listExpected = 5;
    while (listEn->MoveNext()) {
        Assert(listEn->Current() == listExpected++, "ListSequence Iterator");
    }
    delete listEn;

    Sequence<int>* listAppended = listSeq->Append(7);
    Assert(listAppended->GetLength() == 3, "ListSequence Append");
    delete listSeq;

    // ========== Zip/Unzip Tests ==========
    PrintTestHeader("Zip/Unzip");
    int zip1[] = {1, 2, 3};
    int zip2[] = {10, 20, 30};
    Sequence<int>* zipSeq1 = new MutableArraySequence<int>(zip1, 3);
    Sequence<int>* zipSeq2 = new MutableArraySequence<int>(zip2, 3);

    auto* zipped = Zip(zipSeq1, zipSeq2);
    Assert(zipped->GetLength() == 3, "Zip Length");

    IEnumerator<Tuple2<int, int>>* zipEn = zipped->GetEnumerator();
    int zipExpected = 1;
    while (zipEn->MoveNext()) {
        Assert(zipEn->Current().item1 == zipExpected, "Zip Iterator item1");
        Assert(zipEn->Current().item2 == zipExpected * 10, "Zip Iterator item2");
        zipExpected++;
    }
    delete zipEn;

    delete zipped;

    auto unzipped = Unzip(zipped);
    delete unzipped.first;
    delete unzipped.second;
    delete zipSeq1;
    delete zipSeq2;

    // ========== Split Tests ==========
    PrintTestHeader("Split");
    int splitData[] = {1, 2, 0, 3, 4, 0, 5};
    Sequence<int>* splitSeq = new MutableArraySequence<int>(splitData, 7);

    auto* splitted = Split(splitSeq, [](int x) { return x == 0; });
    Assert(splitted->GetLength() == 3, "Split Count (3 fragments)");

    IEnumerator<Sequence<int>*>* splitEn = splitted->GetEnumerator();
    int splitIdx = 0;
    int expectedLengths[] = {2, 2, 1};
    while (splitEn->MoveNext()) {
        Assert(splitEn->Current()->GetLength() == expectedLengths[splitIdx], "Split Fragment Iterator");
        splitIdx++;
    }
    delete splitEn;

    delete splitted;
    delete splitSeq;

    // ========== Option/Find Tests ==========
    PrintTestHeader("Option/Find");
    Sequence<int>* findSeq = new MutableArraySequence<int>(whereData, 5);

    Option<int> found = Find(findSeq, [](int x) { return x > 3; });
    Assert(found.IsSome(), "Find Some");
    Assert(found.GetValue() == 4, "Find Value");

    Option<int> notFound = Find(findSeq, [](int x) { return x > 100; });
    Assert(notFound.IsNone(), "Find None");
    delete findSeq;

    // ========== From Tests ==========
    PrintTestHeader("From");
    int fromData[] = {1, 2, 3, 4, 5};
    Sequence<int>* fromSeq = From(fromData, 5);
    Assert(fromSeq->GetLength() == 5, "From Length");

    IEnumerator<int>* fromEn = fromSeq->GetEnumerator();
    int fromExpected = 1;
    while (fromEn->MoveNext()) {
        Assert(fromEn->Current() == fromExpected++, "From Iterator");
    }
    delete fromEn;

    delete fromSeq;

    // ========== Concat Tests ==========
    PrintTestHeader("Concat");
    int concat1[] = {1, 2, 3};
    int concat2[] = {4, 5, 6};
    Sequence<int>* concatSeq1 = From(concat1, 3);
    Sequence<int>* concatSeq2 = From(concat2, 3);

    Sequence<int>* concatenated = Concat(concatSeq1, concatSeq2);
    Assert(concatenated->GetLength() == 6, "Concat Length");

    IEnumerator<int>* concatEn = concatenated->GetEnumerator();
    int concatExpected = 1;
    while (concatEn->MoveNext()) {
        Assert(concatEn->Current() == concatExpected++, "Concat Iterator");
    }
    delete concatEn;

    delete concatSeq1;
    delete concatSeq2;
    delete concatenated;

    // ========== Slice Tests ==========
    PrintTestHeader("Slice");
    int sliceData[] = {1, 2, 3, 4, 5};
    Sequence<int>* sliceSeq = new MutableArraySequence<int>(sliceData, 5);
    int sliceInsert[] = {9, 10};
    Sequence<int>* sliceInsertSeq = new MutableArraySequence<int>(sliceInsert, 2);

    Sequence<int>* sliced = Slice(sliceSeq, 1, 2, sliceInsertSeq);
    Assert(sliced->GetLength() == 5, "Slice Length");

    IEnumerator<int>* sliceEn = sliced->GetEnumerator();
    int sliceExpected[] = {1, 9, 10, 4, 5};
    int sliceIdx = 0;
    while (sliceEn->MoveNext()) {
        Assert(sliceEn->Current() == sliceExpected[sliceIdx++], "Slice Iterator");
    }
    delete sliceEn;

    delete sliced;
    delete sliceSeq;
    delete sliceInsertSeq;

    // ========== ICollection Tests ==========
    PrintTestHeader("ICollection");
    Sequence<int>* collSeq = new MutableArraySequence<int>(seqData, 2);

    Assert(collSeq->Get(0) == 5, "ICollection Get");
    Assert(collSeq->GetCount() == 2, "ICollection GetCount");

    IEnumerator<int>* collEn = collSeq->GetEnumerator();
    int collExpected = 5;
    while (collEn->MoveNext()) {
        Assert(collEn->Current() == collExpected++, "ICollection Iterator");
    }
    delete collEn;

    Sequence<int>* cloned = collSeq->Clone();
    Assert(cloned->GetCount() == 2, "ICollection Clone");

    delete collSeq;
    delete cloned;
    // ========== DequeSegmented Tests ==========
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

    Sequence<int>* dequeCloned = deque.Clone();
    Assert(dequeCloned->GetLength() == 3, "DequeSegmented Clone Length");
    delete dequeCloned;

    Sequence<int>* dequeMapped = deque.Map<int>([](int x) { return x * 2; });
    Assert(dequeMapped->Get(0) == 2, "DequeSegmented Map");
    delete dequeMapped;

    Sequence<int>* dequeFiltered = deque.Where([](int x) { return x > 1; });
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
    Sequence<int>* dequeConcatenated = deque1.Concat(&deque2);
    Assert(dequeConcatenated->GetLength() == 2, "DequeSegmented Concat Length");
    delete dequeConcatenated;

    Sequence<int>* dequeSubseq = deque.GetSubsequence(0, 1);
    Assert(dequeSubseq->GetLength() == 2, "DequeSegmented Subsequence Length");
    delete dequeSubseq;

    // ========== Hanoi Tower Tests ==========
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

    // ========== Ring Tests ==========
    PrintTestHeader("Ring");

    Ring r1(3, "\033[91m");
    Ring r2(5, "\033[92m");

    Assert(r1.GetSize() == 3, "Ring Size");
    Assert(r1 < r2, "Ring Comparison");
    Assert(r1 != r2, "Ring Inequality");
    // ========== Итог ==========
    PrintSummary();
}