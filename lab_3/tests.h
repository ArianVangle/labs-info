#pragma once
#include "from_lab_2/tests.h"

class Lab3TestRunner : public TestRunner {
private:
    void TestDequeBasicOps();
    void TestDequePopClearAndEdge();
    void TestDequeMapReduceIterators();
    void TestDequeSortConcatSubsequence();
    void TestDequeExceptions();

    void TestHanoiConstruction();
    void TestHanoiSolvingLogic();
    void TestHanoiHistoryAndStats();
    void TestHanoiExceptions();
    void TestRingOperations();

public:
    Lab3TestRunner(ConsoleUI* ui = nullptr) : TestRunner(ui) {}
    void RunAll();
};