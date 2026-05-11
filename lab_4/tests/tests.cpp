#include <iostream>
#include <chrono>
#include "../include/lab4_new/cardinal.hpp"
#include "../include/lab4_new/lazy_sequence.hpp"
#include "../include/lab4_new/stream.hpp"
#include "../include/lab4_new/read_only_stream.hpp"
#include "../include/lab4_new/algorithms.hpp"
#include "../include/lab4_new/generator.hpp"
#include "../include/lab4_new/write_only_stream.hpp"
#include "../include/lab3_base/array_sequence.h"

void PrintHeader(const std::string& title) {
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ";
    std::cout << title;
    int padding = 58 - title.length();
    for (int i = 0; i < padding; i++) std::cout << " ";
    std::cout << "║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
}

void Assert(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "  ✓ [PASS] " << testName << "\n";
    } else {
        std::cout << "  ✗ [FAIL] " << testName << "\n";
    }
}

void TestCardinal() {
    PrintHeader("CARDINAL TESTS");
    
    Cardinal finite(42);
    Cardinal infinite = Cardinal::Infinity();
    
    Assert(finite.IsFinite(), "IsFinite");
    Assert(infinite.IsInfinite(), "IsInfinite");
    Assert(finite.GetValue() == 42, "GetValue");
    Assert(finite < infinite, "Comparison: finite < infinite");
    Assert(!(infinite < finite), "Comparison: !(infinite < finite)");
    
    Cardinal sum = finite + infinite;
    Assert(sum.IsInfinite(), "Addition with Infinity");
    
    Cardinal sum2 = finite + Cardinal(8);
    Assert(sum2.GetValue() == 50, "Addition: 42 + 8 = 50");
    
    Assert(finite.ToString() == "42", "ToString finite");
    Assert(infinite.ToString() == "∞", "ToString infinite");
    
    std::cout << "\n";
}

void TestLazySequence() {
    PrintHeader("LAZY SEQUENCE TESTS");
    
    auto* factorials = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            if (prev->GetLength() == 0) return 1;
            return prev->Get(prev->GetLength() - 1) * (int)prev->GetLength();
        },
        Cardinal(10)
    );
    
    Assert(factorials->Get(0) == 1, "Factorial[0] = 1");
    Assert(factorials->Get(1) == 1, "Factorial[1] = 1");
    Assert(factorials->Get(2) == 2, "Factorial[2] = 2");
    Assert(factorials->Get(3) == 6, "Factorial[3] = 6");
    Assert(factorials->Get(4) == 24, "Factorial[4] = 24");
    
    Assert(factorials->GetMaterializedCount() == 5, "Materialized count = 5 (elements 0-4)");
    Assert(factorials->GetCardinalLength().IsFinite(), "Length is finite");
    
    delete factorials;
    

    auto* fib = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            if (prev->GetLength() < 2) return 1;
            return prev->Get(prev->GetLength() - 1) + prev->Get(prev->GetLength() - 2);
        },
        Cardinal::Infinity()
    );
    
    Assert(fib->Get(0) == 1, "Fibonacci[0]");
    Assert(fib->Get(1) == 1, "Fibonacci[1]");
    Assert(fib->Get(2) == 2, "Fibonacci[2]");
    Assert(fib->Get(6) == 13, "Fibonacci[6]");
    Assert(fib->GetCardinalLength().IsInfinite(), "Fibonacci is infinite");
    
    auto* doubled = fib->Map<int>([](int x) -> int { return x * 2; });
    Assert(doubled->Get(3) == 6, "Map: double");
    delete doubled;
    
    auto* filtered = fib->Where([](int x) -> bool { return x % 2 == 0; });
    Assert(filtered->GetCount() > 0, "Where: filter even");
    delete filtered;
    
    auto* appended = fib->Append(999);
    Assert(appended->GetCount() > fib->GetCount(), "Append");
    delete appended;
    
    delete fib;
    
    auto* empty = new LazySequence<int>();
    Assert(empty->GetLength() == 0, "Empty sequence length");
    delete empty;
    
    std::cout << "\n";
}

void TestGenerator() {
    PrintHeader("GENERATOR TESTS");
    
    auto* seq = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            return prev->GetLength() + 1;
        },
        Cardinal(100)
    );
    
    auto* gen = new Generator<int>(seq, 
        [](Sequence<int>* prev) -> int {
            return prev->GetLength() + 1;
        },
        10
    );
    
    Assert(gen->HasNext(), "HasNext");
    Assert(gen->GetNext() > 0, "GetNext");
    Assert(gen->GetCacheSize() <= 10, "Cache size limit");
    
    gen->Reset();
    Assert(gen->GetPosition() == 0, "Reset position");
    
    delete gen;
    delete seq;
    
    std::cout << "\n";
}

void TestStreams() {
    PrintHeader("STREAM TESTS");
    
    int data[] = {1, 2, 3, 4, 5};
    auto* seq = new MutableArraySequence<int>(data, 5);
    auto* stream = new SequenceReadStream<int>(seq);
    
    stream->Open();
    Assert(!stream->IsEndOfStream(), "Not at end");
    Assert(stream->Read() == 1, "Read[0]");
    Assert(stream->Read() == 2, "Read[1]");
    Assert(stream->GetPosition() == 2, "Position");
    Assert(stream->IsCanSeek(), "Can seek");
    Assert(stream->IsCanGoBack(), "Can go back");
    
    stream->Seek(0);
    Assert(stream->GetPosition() == 0, "Seek");
    
    stream->Close();
    
    delete stream;
    delete seq;
    
    auto* lazy = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            return prev->GetLength() + 1;
        },
        Cardinal(10)
    );
    
    auto* lazyStream = new LazySequenceReadStream<int>(lazy);
    lazyStream->Open();
    Assert(!lazyStream->IsEndOfStream(), "LazyStream not at end");
    Assert(lazyStream->Read() == 1, "LazyStream Read[0]");
    lazyStream->Close();
    
    delete lazyStream;
    delete lazy;
    
    std::cout << "\n";
}

void TestAlgorithms() {
    PrintHeader("ALGORITHM TESTS");
    
    OnlineStatistics<int> stats;
    int data[] = {10, 25, 3, 47, 19, 33, 8};
    
    for (int val : data) {
        stats.Add(val);
    }
    
    Assert(stats.GetCount() == 7, "Stats Count");
    Assert(stats.GetMean() == 20, "Stats Mean");
    Assert(stats.GetMin() == 3, "Stats Min");
    Assert(stats.GetMax() == 47, "Stats Max");
    
    int unsorted[] = {5, 2, 8, 1, 9};
    auto* input = new MutableArraySequence<int>(unsorted, 5);
    auto* inputStream = new SequenceReadStream<int>(input);
    
    StreamSorter<int> sorter([](int a, int b) -> bool { return a < b; }, 10);
    auto* sorted = sorter.SortWithHeap(inputStream);
    
    sorted->Open();
    Assert(sorted->Read() == 1, "HeapSort[0]");
    Assert(sorted->Read() == 2, "HeapSort[1]");
    sorted->Close();
    
    delete sorted;
    delete inputStream;
    delete input;
    
    std::cout << "\n";
}

void TestPerformance() {
    PrintHeader("PERFORMANCE TESTS (Large Data)");
    
    const int SIZE = 50000;
    
    std::cout << "  Testing with " << SIZE << " elements...\n\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto* lazy = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            return prev->GetLength() + 1;
        },
        Cardinal(SIZE)
    );
    
    volatile int dummy = lazy->Get(SIZE - 1);
    (void)dummy;
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  LazySequence " << SIZE << " elements: " 
              << duration.count() << " ms\n";
    std::cout << "  Materialized: " << lazy->GetMaterializedCount() << "\n";
    
    Assert(lazy->GetMaterializedCount() == (size_t)SIZE, "Full materialization");
    Assert(duration.count() < 10000, "Performance < 10 sec");
    
    delete lazy;
}

int RunAllTests() {
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║              LAB 4 — AUTOMATED TESTS                      ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    
    TestCardinal();
    TestLazySequence();
    TestGenerator();
    TestStreams();
    TestAlgorithms();
    TestPerformance();
    
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                  ALL TESTS COMPLETED                      ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
    
    return 0;
}

int main() {
    return RunAllTests();
}