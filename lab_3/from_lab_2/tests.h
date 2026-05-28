#pragma once
#include <string>
class ConsoleUI;

class TestRunner {
protected:
    int passed = 0;
    int failed = 0;
    ConsoleUI* ui;
    
    static const int WIDTH = 70;
    const std::string COLOR_PASS = "\033[92m";
    const std::string COLOR_FAIL = "\033[91m";
    const std::string COLOR_INFO = "\033[96m";
    const std::string COLOR_WARN = "\033[93m";
    const std::string COLOR_RESET = "\033[0m";

    void SetColor(const std::string& color);
    void ResetColor();
    void PrintChars(char c, int count);
    void PrintLine(char c);
    void PrintTestHeader(const std::string& header);
    void PrintTestResult(bool success, const std::string& testName);
    void PrintSummary();

    void TestDynamicArray();
    void TestLinkedList();

    void TestMutableArraySequence();
    void TestImmutableArraySequence();
    void TestMutableListSequence();
    void TestSequenceOperators();

    void TestMapOperation();
    void TestWhereOperation();
    void TestReduceOperation();

    void TestZipUnzip();
    void TestSplitOperation();
    void TestSliceOperation();
    void TestFromAndConcat();
    void TestFindAndOption();

    void TestIterators();
    void TestSubsequenceAndClone();
    void TestExceptionsAndEdges();

public:
    TestRunner(ConsoleUI* ui = nullptr);
    void Assert(bool condition, const std::string& testName);
    void RunAll();
};