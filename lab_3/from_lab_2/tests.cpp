#include "tests.h"
#include <iostream>
#include "array_sequence.h"
#include "dynamic_array.h"
#include "iterators.h"
#include "linked_list.h"
#include "list_sequence.h"
#include "map_reduce.h"
#include "option.h"
#include "utils.h"

void TestRunner::SetColor(const std::string& color) { std::cout << color; }
void TestRunner::ResetColor() { std::cout << COLOR_RESET; }
void TestRunner::PrintChars(char c, int count) { for (int i = 0; i < count; ++i) std::cout << c; }
void TestRunner::PrintLine(char c) { SetColor(COLOR_INFO); PrintChars(c, WIDTH); std::cout << std::endl; ResetColor(); }

void TestRunner::PrintTestHeader(const std::string& header) {
    std::cout << std::endl;
    SetColor(COLOR_INFO);
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    int visibleLen = VisibleLength(header);
    int padding = (WIDTH - 2 - visibleLen) / 2;
    std::cout << "|"; PrintChars(' ', padding); std::cout << header;
    PrintChars(' ', WIDTH - 2 - visibleLen - padding); std::cout << "|" << std::endl;
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    ResetColor();
}

void TestRunner::PrintTestResult(bool success, const std::string& testName) {
    if (success) { SetColor(COLOR_PASS); passed++; } 
    else { SetColor(COLOR_FAIL); failed++; }
    
    std::cout << "| " << (success ? "✓ [PASS] " : "✗ [FAIL] ") << testName;
    int textLen = 9 + VisibleLength(testName);
    int padding = WIDTH - 3 - textLen;
    if (padding < 1) padding = 1;
    PrintChars(' ', padding); std::cout << "|" << std::endl;
    ResetColor();
}

void TestRunner::PrintSummary() {
    std::cout << std::endl; SetColor(COLOR_INFO);
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    std::cout << "| ИТОГОВЫЙ ОТЧЁТ                                                    |" << std::endl;
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    
    SetColor(COLOR_PASS);
    std::cout << "| Пройдено: " << passed;
    int pad1 = WIDTH - 3 - VisibleLength("Пройдено: ") - std::to_string(passed).length();
    PrintChars(' ', pad1); std::cout << "|" << std::endl;
    
    SetColor(failed > 0 ? COLOR_FAIL : COLOR_PASS);
    std::cout << "| Провалено: " << failed;
    int pad2 = WIDTH - 3 - VisibleLength("Провалено: ") - std::to_string(failed).length();
    PrintChars(' ', pad2); std::cout << "|" << std::endl;
    
    int total = passed + failed;
    double pct = total > 0 ? (double)passed / total * 100 : 0;
    SetColor(pct >= 80 ? COLOR_PASS : (pct >= 50 ? COLOR_WARN : COLOR_FAIL));
    std::cout << "| Успешность: " << (int)pct << "%";
    int pad3 = WIDTH - 3 - VisibleLength("Успешность: ") - std::to_string((int)pct).length() - 1;
    PrintChars(' ', pad3); std::cout << "|" << std::endl;
    
    std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
    ResetColor();
}

TestRunner::TestRunner(ConsoleUI* ui) : passed(0), failed(0), ui(ui) {}

void TestRunner::Assert(bool condition, const std::string& testName) {
    PrintTestResult(condition, testName);
}

void TestRunner::TestDynamicArray() {
    PrintTestHeader("DynamicArray");
    int arr[] = {10, 20, 30};
    DynamicArray<int> da(arr, 3);
    Assert(da.Get(0) == 10 && da.Get(2) == 30, "DynamicArray: Initial Get");
    da.Set(1, 99); Assert(da.Get(1) == 99, "DynamicArray: Set");
    da.Resize(5); Assert(da.GetSize() == 5, "DynamicArray: Resize");
    try { da.Get(10); Assert(false, "DynamicArray: Exception Handled"); } 
    catch (...) { Assert(true, "DynamicArray: Exception Handled"); }
}

void TestRunner::TestLinkedList() {
    PrintTestHeader("LinkedList");
    LinkedList<int> ll;
    ll.Append(10); ll.Append(20);
    Assert(ll.GetLength() == 2 && ll.GetLast() == 20, "LinkedList: Append/GetLast");
    ll.Prepend(5); Assert(ll.GetFirst() == 5, "LinkedList: Prepend");
    ll.InsertAt(15, 1); Assert(ll.Get(1) == 15, "LinkedList: InsertAt");
    try { ll.Get(10); Assert(false, "LinkedList: Exception"); } catch (...) { Assert(true, "LinkedList: Exception"); }
}

void TestRunner::TestMutableArraySequence() {
    PrintTestHeader("MutableArraySequence");
    int data[] = {1, 2, 3};
    Sequence<int>* seq = new MutableArraySequence<int>(data, 3);
    Assert(seq->GetLength() == 3, "MutableArray: Initial Length");
    
    Sequence<int>* appended = seq->Append(4);
    Assert(appended == seq && seq->GetLength() == 4, "MutableArray: Append In-Place");
    
    seq->Prepend(0); Assert(seq->GetFirst() == 0, "MutableArray: Prepend In-Place");
    seq->Set(2, 999); Assert(seq->Get(2) == 999, "MutableArray: Set In-Place");
    delete seq;
}

void TestRunner::TestImmutableArraySequence() {
    PrintTestHeader("ImmutableArraySequence");
    int data[] = {1, 2, 3};
    Sequence<int>* seq = new ImmutableArraySequence<int>(data, 3);
    int originalLen = seq->GetLength();
    
    Sequence<int>* newSeq = seq->Append(4);
    Assert(seq->GetLength() == originalLen, "ImmutableArray: Original Unchanged");
    Assert(newSeq != seq && newSeq->GetLength() == originalLen + 1, "ImmutableArray: New Instance Created");
    
    Sequence<int>* prepended = newSeq->Prepend(0);
    Assert(prepended != newSeq && prepended->GetFirst() == 0, "ImmutableArray: Prepend Creates New");
    
    try { seq->Set(0, 99); Assert(false, "ImmutableArray: Set Throws"); } 
    catch (...) { Assert(true, "ImmutableArray: Set Throws Exception"); }
    
    delete seq; delete newSeq; delete prepended;
}

void TestRunner::TestMutableListSequence() {
    PrintTestHeader("MutableListSequence");
    int data[] = {10, 20};
    Sequence<int>* list = new MutableListSequence<int>(data, 2);
    Assert(list->GetLength() == 2, "MutableList: Length");
    
    Sequence<int>* prepended = list->Prepend(5);
    Assert(prepended == list && list->GetFirst() == 5, "MutableList: Prepend In-Place");
    
    list->InsertAt(15, 1); Assert(list->Get(1) == 15, "MutableList: InsertAt In-Place");
    delete list;
}

void TestRunner::TestSequenceOperators() {
    PrintTestHeader("Sequence Operators");
    int d1[] = {1, 2, 3}; Sequence<int>* s1 = new MutableArraySequence<int>(d1, 3);
    Assert((*s1)[1] == 2, "Operator[]: Index Access");
    
    int d2[] = {1, 2, 3}; Sequence<int>* s2 = new MutableArraySequence<int>(d2, 3);
    Assert(*s1 == *s2, "Operator==: Equality Check");
    
    int d3[] = {4, 5}; Sequence<int>* s3 = new MutableArraySequence<int>(d3, 2);
    Sequence<int>* combined = *s1 + *s3;
    Assert(combined->GetLength() == 5 && combined->Get(3) == 4, "Operator+: Concatenation");
    
    delete s1; delete s2; delete s3; delete combined;
}

void TestRunner::TestMapOperation() {
    PrintTestHeader("Map Operation");
    int data[] = {1, 2, 3};
    auto* seq = new MutableArraySequence<int>(data, 3);
    Sequence<int>* mapped = seq->Map<int>([](int x) { return x * 10; });
    
    Assert(mapped->Get(0) == 10 && mapped->Get(2) == 30, "Map: Transformation");
    Assert(mapped->GetLength() == seq->GetLength(), "Map: Length Preserved");
    delete seq; delete mapped;
}

void TestRunner::TestWhereOperation() {
    PrintTestHeader("Where Operation");
    int data[] = {1, 2, 3, 4, 5};
    auto* seq = new MutableArraySequence<int>(data, 5);
    Sequence<int>* filtered = seq->Where([](int x) { return x % 2 != 0; });
    
    Assert(filtered->GetLength() == 3, "Where: Filter Count");
    Assert(filtered->Get(0) == 1 && filtered->Get(2) == 5, "Where: Correct Values");
    delete seq; delete filtered;
}

void TestRunner::TestReduceOperation() {
    PrintTestHeader("Reduce Operation");
    int data[] = {1, 2, 3, 4};
    auto* seq = new MutableArraySequence<int>(data, 4);
    int sum = seq->Reduce([](int acc, int x) { return acc + x; }, 0);
    int product = seq->Reduce([](int acc, int x) { return acc * x; }, 1);
    
    Assert(sum == 10, "Reduce: Sum");
    Assert(product == 24, "Reduce: Product");
    delete seq;
}

void TestRunner::TestZipUnzip() {
    PrintTestHeader("Zip / Unzip");
    int a[] = {1, 2}; char b[] = {'a', 'b'};
    auto* s1 = From(a, 2);
    auto* s2 = From<char>(b, 2);
    Sequence<Tuple2<int, char>>* zipped = Zip(*s1, *s2);
    
    Assert(zipped->Get(0).item1 == 1 && zipped->Get(1).item2 == 'b', "Zip: Pairing");
    
    auto [unz1, unz2] = Unzip(*zipped);
    Assert(unz1->GetLength() == 2 && unz2->Get(0) == 'a', "Unzip: Decomposition");
    delete s1; delete s2; delete zipped; delete unz1; delete unz2;
}

void TestRunner::TestSplitOperation() {
    PrintTestHeader("Split Operation");
    int data[] = {1, 0, 2, 3, 0, 4};
    auto* seq = From(data, 6);
    auto* parts = Split(*seq, [](int x) { return x == 0; });
    
    Assert(parts->GetLength() == 3, "Split: Part Count");
    Assert(parts->Get(0)->GetLength() == 1, "Split: First Part Length");
    Assert(parts->Get(2)->GetLength() == 1, "Split: Last Part Length");
    
    for (int i = 0; i < parts->GetLength(); ++i) delete parts->Get(i);
    delete parts; delete seq;
}

void TestRunner::TestSliceOperation() {
    PrintTestHeader("Slice Operation");
    int data[] = {1, 2, 3, 4, 5};
    auto* seq = From(data, 5);
    int insertData[] = {9, 10};
    auto* ins = From(insertData, 2);
    
    Sequence<int>* sliced = Slice(*seq, 1, 2, ins);
    int expected[] = {1, 9, 10, 4, 5};
    bool match = true;
    for (int i = 0; i < 5; ++i) if (sliced->Get(i) != expected[i]) match = false;
    Assert(match, "Slice: Correct Replacement");
    
    try { Slice(*seq, 10, 1); Assert(false, "Slice: Exception on Bad Index"); }
    catch (...) { Assert(true, "Slice: Exception Handled"); }
    
    delete seq; delete ins; delete sliced;
}

void TestRunner::TestFromAndConcat() {
    PrintTestHeader("From / Concat");
    int a[] = {1, 2}; int b[] = {3, 4};
    auto* s1 = From(a, 2); auto* s2 = From(b, 2);
    Sequence<int>* c = Concat(*s1, *s2);
    Assert(c->Get(3) == 4 && c->GetLength() == 4, "Concat: Merge");
    Assert(c->Get(0) == 1, "Concat: Order Preserved");
    delete s1; delete s2; delete c;
}

void TestRunner::TestFindAndOption() {
    PrintTestHeader("Find & Option<T>");
    int data[] = {10, 20, 30};
    auto* seq = From(data, 3);
    
    Option<int> found = Find(*seq, [](int x) { return x > 15; });
    Assert(found.IsSome() && found.GetValue() == 20, "Find: Existing Value");
    
    Option<int> missing = Find(*seq, [](int x) { return x > 100; });
    Assert(missing.IsNone(), "Find: None Returned");
    
    Option<int> tryOk = seq->TryGet(1); Assert(tryOk.IsSome(), "TryGet: Valid Index");
    Option<int> tryBad = seq->TryGet(5); Assert(tryBad.IsNone(), "TryGet: Invalid Index");
    delete seq;
}

void TestRunner::TestIterators() {
    PrintTestHeader("Iterators (IEnumerable/IEnumerator)");
    int data[] = {5, 10, 15};
    auto* seq = new MutableArraySequence<int>(data, 3);
    IEnumerator<int>* en = seq->GetEnumerator();
    
    bool allOk = true; int expected = 5;
    while (en->MoveNext()) { if (en->Current() != expected) allOk = false; expected += 5; }
    Assert(allOk, "IEnumerator: Full Traversal");
    
    en->Reset(); Assert(en->MoveNext(), "IEnumerator: Reset Works");
    delete seq; delete en;
}

void TestRunner::TestSubsequenceAndClone() {
    PrintTestHeader("Subsequence & Clone");
    int data[] = {1, 2, 3, 4, 5};
    auto* seq = From(data, 5);
    
    Sequence<int>* sub = seq->GetSubsequence(1, 3);
    Assert(sub->GetLength() == 3 && sub->Get(0) == 2, "Subsequence: Extraction");
    
    Sequence<int>* cloned = seq->Clone();
    Assert(cloned->GetCount() == 5, "Clone: Deep Copy Length");
    cloned->Append(99);
    Assert(seq->GetLength() == 5, "Clone: Independence Check");
    
    delete seq; delete sub; delete cloned;
}

void TestRunner::TestExceptionsAndEdges() {
    PrintTestHeader("Exceptions & Edge Cases");
    int data[] = {42};
    auto* seq = From(data, 1);
    
    Assert(seq->GetFirst() == 42 && seq->GetLast() == 42, "Edge: Single Element GetFirst/Last");
    
    try { seq->GetSubsequence(0, 1); Assert(false, "Edge: Subsequence OutOfBounds"); }
    catch (...) { Assert(true, "Edge: Subsequence Exception"); }
    
    try { seq->InsertAt(10, 5); Assert(false, "Edge: InsertAt OutOfBounds"); }
    catch (...) { Assert(true, "Edge: InsertAt Exception"); }
    
    int* emptyArr = new int[0];
    auto* emptySeq = From(emptyArr, 0);
    delete[] emptyArr;
    
    Assert(emptySeq->GetLength() == 0, "Edge: Empty Sequence Length");
    try { emptySeq->GetFirst(); Assert(false, "Edge: Empty GetFirst"); }
    catch (...) { Assert(true, "Edge: Empty Exception"); }
    
    delete seq; 
    delete emptySeq;
}

void TestRunner::RunAll() {
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
    
    PrintSummary();
}