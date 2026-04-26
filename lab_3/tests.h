#pragma once
#include "from_lab_2/tests.h"

class Lab3TestRunner : public TestRunner {
   private:
    void TestDeque();
    void TestHanoiAndRing();

   public:
    Lab3TestRunner(ConsoleUI *ui) : TestRunner(ui) {}

    void RunAll() override;
};