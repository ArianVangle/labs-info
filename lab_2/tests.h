#pragma once

#include <iostream>
#include <string>

#include "array_sequence.h"
#include "dynamic_array.h"
#include "icollection.h"
#include "linked_list.h"
#include "list_sequence.h"
#include "map_reduce.h"
#include "utils.h"

class ConsoleUI;

class TestRunner {
   private:
    int passed = 0;
    int failed = 0;
    ConsoleUI* ui;

    static const int WIDTH = 70;

    const std::string COLOR_PASS = "\033[92m";
    const std::string COLOR_FAIL = "\033[91m";
    const std::string COLOR_INFO = "\033[96m";
    const std::string COLOR_WARN = "\033[93m";
    const std::string COLOR_RESET = "\033[0m";

    void SetColor(const std::string& color) { std::cout << color; }

    void ResetColor() { std::cout << COLOR_RESET; }

    void PrintChars(char c, int count) {
        for (int i = 0; i < count; i++) std::cout << c;
    }

    void PrintLine(char c) {
        SetColor(COLOR_INFO);
        PrintChars(c, WIDTH);
        std::cout << std::endl;
        ResetColor();
    }

    void PrintTestHeader(const std::string& header) {
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

    void PrintTestResult(bool success, const std::string& testName) {
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

    void PrintSummary() {
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

        std::string percentText =
            "Успешность: " + std::to_string((int)percentage) + "%";
        int visiblePercent = VisibleLength(percentText);
        int percentPad = WIDTH - 3 - visiblePercent;

        std::string color = percentage >= 80
                                ? COLOR_PASS
                                : (percentage >= 50 ? COLOR_WARN : COLOR_FAIL);
        SetColor(color);

        std::cout << "| ";
        std::cout << percentText;
        PrintChars(' ', percentPad);
        std::cout << "|" << std::endl;

        std::cout << "+" << std::string(WIDTH - 2, '-') << "+" << std::endl;
        ResetColor();
    }

   public:
    TestRunner(ConsoleUI* ui = nullptr) : ui(ui) {}

    void Assert(bool condition, const std::string& testName) {
        PrintTestResult(condition, testName);
    }

    void RunAll() {
        PrintTestHeader("ЗАПУСК ВСЕХ МОДУЛЬНЫХ ТЕСТОВ");

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

        PrintTestHeader("Sequence (Mutable)");
        int seqData[] = {5, 6};
        Sequence<int>* seq = new MutableArraySequence<int>(seqData, 2);
        Assert(seq->GetLength() == 2, "Sequence Length");
        Assert(seq->Get(0) == 5, "Sequence Get(0)");
        Assert(seq->Get(1) == 6, "Sequence Get(1)");

        Sequence<int>* newSeq = seq->Append(7);
        Assert(newSeq->GetLength() == 3, "Sequence Append Mutable");
        Assert(seq->GetLength() == 3, "Sequence Mutable State Change");
        delete seq;

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

        PrintTestHeader("Map-Reduce Operations");
        MutableArraySequence<int>* mapSeq =
            new MutableArraySequence<int>(seqData, 2);
        Sequence<int>* mapped = mapSeq->Map<int>([](int x) { return x * 2; });
        Assert(mapped->Get(0) == 10, "Map Function (x*2)");
        Assert(mapped->Get(1) == 12, "Map Function (x*2)");
        delete mapSeq;
        delete mapped;

        int whereData[] = {1, 2, 3, 4, 5};
        Sequence<int>* whereSeq = new MutableArraySequence<int>(whereData, 5);
        Sequence<int>* filtered = whereSeq->Where([](int x) { return x > 2; });
        Assert(filtered->GetLength() == 3, "Where Filter (x>2)");
        Assert(filtered->Get(0) == 3, "Where Result[0]");
        Assert(filtered->Get(2) == 5, "Where Result[2]");
        delete whereSeq;
        delete filtered;

        Sequence<int>* reduceSeq = new MutableArraySequence<int>(whereData, 5);
        int sum = reduceSeq->Reduce([](int acc, int x) { return acc + x; }, 0);
        Assert(sum == 15, "Reduce Sum (1+2+3+4+5=15)");
        int product =
            reduceSeq->Reduce([](int acc, int x) { return acc * x; }, 1);
        Assert(product == 120, "Reduce Product (1*2*3*4*5=120)");
        delete reduceSeq;

        PrintTestHeader("Iterators");
        Sequence<int>* iterSeq = new MutableArraySequence<int>(seqData, 2);
        IEnumerator<int>* en = iterSeq->GetEnumerator();
        Assert(en->MoveNext(), "Iterator MoveNext");
        Assert(en->Current() == 5, "Iterator Current");
        en->Reset();
        Assert(en->MoveNext(), "Iterator Reset + MoveNext");
        delete iterSeq;
        delete en;

        PrintTestHeader("Operator Overloading");
        Sequence<int>* opSeq = new MutableArraySequence<int>(seqData, 2);
        Assert((*opSeq)[0] == 5, "Operator[] [0]");
        Assert((*opSeq)[1] == 6, "Operator[] [1]");
        delete opSeq;

        PrintTestHeader("Subsequence");
        Sequence<int>* subSeq = new MutableArraySequence<int>(whereData, 5);
        Sequence<int>* sub = subSeq->GetSubsequence(1, 3);
        Assert(sub->GetLength() == 3, "Subsequence Length");
        Assert(sub->Get(0) == 2, "Subsequence Get(0)");
        Assert(sub->Get(2) == 4, "Subsequence Get(2)");
        delete subSeq;
        delete sub;

        PrintTestHeader("ListSequence");
        Sequence<int>* listSeq = new MutableListSequence<int>(seqData, 2);
        Assert(listSeq->GetLength() == 2, "ListSequence Length");
        Sequence<int>* listAppended = listSeq->Append(7);
        Assert(listAppended->GetLength() == 3, "ListSequence Append");
        delete listSeq;

        PrintTestHeader("Zip/Unzip");
        int zip1[] = {1, 2, 3};
        int zip2[] = {10, 20, 30};
        Sequence<int>* zipSeq1 = new MutableArraySequence<int>(zip1, 3);
        Sequence<int>* zipSeq2 = new MutableArraySequence<int>(zip2, 3);

        auto* zipped = Zip(zipSeq1, zipSeq2);
        Assert(zipped->GetLength() == 3, "Zip Length");
        Assert(zipped->Get(0).item1 == 1, "Zip Element 0.1");
        Assert(zipped->Get(0).item2 == 10, "Zip Element 0.2");
        delete zipped;

        auto unzipped = Unzip(zipped);
        delete unzipped.first;
        delete unzipped.second;
        delete zipSeq1;
        delete zipSeq2;

        PrintTestHeader("Split");
        int splitData[] = {1, 2, 0, 3, 4, 0, 5};
        Sequence<int>* splitSeq = new MutableArraySequence<int>(splitData, 7);

        auto* splitted = Split(splitSeq, [](int x) { return x == 0; });
        Assert(splitted->GetLength() == 3, "Split Count (3 fragments)");
        Assert(splitted->Get(0)->GetLength() == 2, "Split Fragment 0 Length");
        Assert(splitted->Get(1)->GetLength() == 2, "Split Fragment 1 Length");
        Assert(splitted->Get(2)->GetLength() == 1, "Split Fragment 2 Length");

        delete splitted;
        delete splitSeq;

        PrintTestHeader("Option/Find");
        Sequence<int>* findSeq = new MutableArraySequence<int>(whereData, 5);

        Option<int> found = Find(findSeq, [](int x) { return x > 3; });
        Assert(found.IsSome(), "Find Some");
        Assert(found.GetValue() == 4, "Find Value");

        Option<int> notFound = Find(findSeq, [](int x) { return x > 100; });
        Assert(notFound.IsNone(), "Find None");
        delete findSeq;
        PrintTestHeader("From");
        int fromData[] = {1, 2, 3, 4, 5};
        Sequence<int>* fromSeq = From(fromData, 5);
        Assert(fromSeq->GetLength() == 5, "From Length");
        Assert(fromSeq->Get(0) == 1, "From Get(0)");
        Assert(fromSeq->Get(4) == 5, "From Get(4)");
        delete fromSeq;

        PrintTestHeader("Concat");
        int concat1[] = {1, 2, 3};
        int concat2[] = {4, 5, 6};
        Sequence<int>* concatSeq1 = From(concat1, 3);
        Sequence<int>* concatSeq2 = From(concat2, 3);

        Sequence<int>* concatenated = Concat(concatSeq1, concatSeq2);
        Assert(concatenated->GetLength() == 6, "Concat Length");
        Assert(concatenated->Get(0) == 1, "Concat [0]");
        Assert(concatenated->Get(2) == 3, "Concat [2]");
        Assert(concatenated->Get(3) == 4, "Concat [3]");
        Assert(concatenated->Get(5) == 6, "Concat [5]");

        delete concatSeq1;
        delete concatSeq2;
        delete concatenated;

        PrintTestHeader("Slice");
        int sliceData[] = {1, 2, 3, 4, 5};
        Sequence<int>* sliceSeq = new MutableArraySequence<int>(sliceData, 5);
        int sliceInsert[] = {9, 10};
        Sequence<int>* sliceInsertSeq =
            new MutableArraySequence<int>(sliceInsert, 2);

        Sequence<int>* sliced = Slice(sliceSeq, 1, 2, sliceInsertSeq);
        Assert(sliced->GetLength() == 5, "Slice Length");
        Assert(sliced->Get(0) == 1, "Slice [0]");
        Assert(sliced->Get(1) == 9, "Slice [1] (inserted)");
        Assert(sliced->Get(2) == 10, "Slice [2] (inserted)");
        Assert(sliced->Get(3) == 4, "Slice [3]");
        Assert(sliced->Get(4) == 5, "Slice [4]");

        delete sliced;
        delete sliceSeq;
        delete sliceInsertSeq;
        PrintTestHeader("ICollection");
        Sequence<int>* collSeq = new MutableArraySequence<int>(seqData, 2);

        Assert(collSeq->Get(0) == 5, "ICollection Get");
        Assert(collSeq->GetCount() == 2, "ICollection GetCount");

        Sequence<int>* cloned = collSeq->Clone();
        Assert(cloned->GetCount() == 2, "ICollection Clone");

        delete collSeq;
        delete cloned;

        PrintSummary();
    }
};