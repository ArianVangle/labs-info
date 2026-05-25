#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "array_sequence.h"
#include "list_sequence.h"
#include "map_reduce.h"
#include "tests.h"
#include "utils.h"

namespace Color {
inline const std::string RESET = "\033[0m";
inline const std::string RED = "\033[31m";
inline const std::string GREEN = "\033[32m";
inline const std::string YELLOW = "\033[33m";
inline const std::string BLUE = "\033[34m";
inline const std::string MAGENTA = "\033[35m";
inline const std::string CYAN = "\033[36m";
inline const std::string WHITE = "\033[37m";
inline const std::string BRIGHT_BLACK = "\033[90m";
inline const std::string BRIGHT_RED = "\033[91m";
inline const std::string BRIGHT_GREEN = "\033[92m";
inline const std::string BRIGHT_YELLOW = "\033[93m";
inline const std::string BRIGHT_BLUE = "\033[94m";
inline const std::string BRIGHT_MAGENTA = "\033[95m";
inline const std::string BRIGHT_CYAN = "\033[96m";
inline const std::string BRIGHT_WHITE = "\033[97m";
}


static int ReadKey() {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224) {
        int dir = _getch();
        if (dir == 72) return 38;
        if (dir == 80) return 40;
    }
    return (ch == 13 || ch == '\r') ? 13 : ch;
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int ch = getchar();
    if (ch == 27) {
        getchar();
        ch = getchar();
        if (ch == 'A') ch = 38;
        else if (ch == 'B') ch = 40;
    }
    if (ch == '\n' || ch == '\r') ch = 13;

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

class ConsoleUI {
private:
    static const int WIDTH = 70;
    void ClearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    void SetColor(const std::string& c) { std::cout << c; }
    void ResetColor() { std::cout << Color::RESET; }
    void PrintChars(char c, int count) { for (int i = 0; i < count; i++) std::cout << c; }
    void PrintRow(const std::string& text, const std::string& color) {
        SetColor(color); std::cout << "| "; std::cout << text;
        int padding = WIDTH - 3 - VisibleLength(text);
        PrintChars(' ', padding); std::cout << "|" << std::endl; ResetColor();
    }
    void PrintRowIndented(const std::string& text, const std::string& color) {
        SetColor(color); std::cout << "|   "; std::cout << text;
        int padding = WIDTH - 5 - VisibleLength(text);
        PrintChars(' ', padding); std::cout << "|" << std::endl; ResetColor();
    }
    void PrintEmpty() {
        SetColor(Color::BRIGHT_BLACK); std::cout << "|";
        PrintChars(' ', WIDTH - 2); std::cout << "|" << std::endl; ResetColor();
    }
    void OpenBox() {
        SetColor(Color::BRIGHT_BLACK); std::cout << "+";
        PrintChars('-', WIDTH - 2); std::cout << "+" << std::endl; ResetColor();
    }
    void CloseBox() { OpenBox(); }
    void PrintLine(char c) {
        SetColor(Color::BRIGHT_BLACK); PrintChars(c, WIDTH);
        std::cout << std::endl; ResetColor();
    }
    void PrintHeader(const std::string& text) {
        std::cout << std::endl; PrintLine('=');
        int visibleLen = VisibleLength(text);
        int padding = (WIDTH - 2 - visibleLen) / 2;
        SetColor(Color::BRIGHT_CYAN); std::cout << "|";
        PrintChars(' ', padding); std::cout << text;
        PrintChars(' ', WIDTH - 2 - visibleLen - padding);
        std::cout << "|" << std::endl; PrintLine('='); ResetColor();
    }
    void PrintSection(const std::string& text) {
        std::cout << std::endl; SetColor(Color::BRIGHT_YELLOW); PrintLine('-');
        std::cout << "| "; std::cout << text;
        int visibleLen = VisibleLength(text);
        int padding = WIDTH - 3 - visibleLen;
        PrintChars(' ', padding); std::cout << "|" << std::endl; PrintLine('-'); ResetColor();
    }
    void Sleep(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

public:
    ConsoleUI() {}

    std::string PrintSequence(Sequence<int>* seq) {
        IEnumerator<int>* en = seq->GetEnumerator();
        std::string result = "[";
        bool first = true;
        while (en->MoveNext()) {
            if (!first) result += ", ";
            result += std::to_string(en->Current());
            first = false;
        }
        result += "]";
        delete en;
        return result;
    }

    void ShowWelcome() {
        ClearScreen(); std::cout << std::endl; PrintLine('#'); PrintEmpty();
        std::string title = "ЛАБОРАТОРНАЯ РАБОТА №2";
        int visibleTitle = VisibleLength(title);
        int pad1 = (WIDTH - 2 - visibleTitle) / 2;
        SetColor(Color::BRIGHT_CYAN); std::cout << "|"; PrintChars(' ', pad1);
        SetColor(Color::BRIGHT_WHITE); std::cout << title;
        SetColor(Color::BRIGHT_CYAN); PrintChars(' ', WIDTH - 2 - visibleTitle - pad1);
        std::cout << "|" << std::endl;
        std::string sub = "Линейные структуры данных";
        int visibleSub = VisibleLength(sub);
        int pad2 = (WIDTH - 2 - visibleSub) / 2;
        SetColor(Color::BRIGHT_CYAN); std::cout << "|"; PrintChars(' ', pad2);
        SetColor(Color::BRIGHT_BLACK); std::cout << sub;
        SetColor(Color::BRIGHT_CYAN); PrintChars(' ', WIDTH - 2 - visibleSub - pad2);
        std::cout << "|" << std::endl;
        PrintEmpty(); PrintLine('#'); ResetColor();
        std::cout << std::endl; SetColor(Color::BRIGHT_BLACK);
        std::cout << "  Нажмите Enter для продолжения..."; ResetColor();
        std::cin.ignore(10000, '\n'); std::cin.get();
    }

    void ShowMenu() {
        struct MenuItem { int key; std::string text; std::string color; };
        std::vector<MenuItem> items = {
            {1, "1. Запустить все тесты", Color::BRIGHT_GREEN},
            {2, "2. Тестировать DynamicArray", Color::BRIGHT_BLUE},
            {3, "3. Тестировать LinkedList", Color::BRIGHT_BLUE},
            {4, "4. Тестировать Sequence (Mutable)", Color::BRIGHT_MAGENTA},
            {5, "5. Тестировать Sequence (Immutable)", Color::BRIGHT_MAGENTA},
            {6, "6. Тестировать Map/Reduce/Where", Color::BRIGHT_YELLOW},
            {7, "7. Тестировать From/Concat/Zip/Split", Color::BRIGHT_CYAN},
            {8, "8. Тестировать Итераторы", Color::BRIGHT_CYAN},
            {9, "9. Тестировать Option<T>", Color::BRIGHT_CYAN},
            {0, "0. Выход", Color::BRIGHT_RED}
        };
        size_t selected = 0;

        while (true) {
            ClearScreen();
            PrintHeader("ГЛАВНОЕ МЕНЮ");
            OpenBox(); PrintEmpty();
            for (size_t i = 0; i < items.size(); i++) {
                std::string prefix = (i == selected) ? "▶ " : "  ";
                std::string clr = (i == selected) ? Color::BRIGHT_WHITE : items[i].color;
                PrintRowIndented(prefix + items[i].text, clr);
            }
            PrintEmpty(); CloseBox();
            std::cout << "\n  Используйте ↑↓ для навигации, Enter для выбора\n";

            int key = ReadKey();
            if (key == 38) selected = (selected + items.size() - 1) % items.size();
            else if (key == 40) selected = (selected + 1) % items.size();
            else if (key == 13) {
                int choice = items[selected].key;
                ClearScreen();
                switch (choice) {
                    case 1: {
                        PrintHeader("ЗАПУСК ВСЕХ ТЕСТОВ");
                        TestRunner runner(this);
                        runner.RunAll();
                        break;
                    }
                    case 2: PrintHeader("ТЕСТИРОВАНИЕ DYNAMICARRAY"); TestDynamicArray(); break;
                    case 3: PrintHeader("ТЕСТИРОВАНИЕ LINKEDLIST"); TestLinkedList(); break;
                    case 4: PrintHeader("ТЕСТИРОВАНИЕ SEQUENCE (MUTABLE)"); TestMutableSequence(); break;
                    case 5: PrintHeader("ТЕСТИРОВАНИЕ SEQUENCE (IMMUTABLE)"); TestImmutableSequence(); break;
                    case 6: PrintHeader("ТЕСТИРОВАНИЕ MAP/REDUCE/WHERE"); TestMapReduce(); break;
                    case 7: PrintHeader("FROM/CONCAT/ZIP/SPLIT"); TestFromConcat(); TestZipUnzip(); TestSplit(); TestSlice(); break;
                    case 8: PrintHeader("ТЕСТИРОВАНИЕ ИТЕРАТОРОВ"); TestIterators(); break;
                    case 9: PrintHeader("ТЕСТИРОВАНИЕ OPTION<T>"); TestOption(); break;
                    case 0: ClearScreen(); PrintHeader("ЗАВЕРШЕНИЕ РАБОТЫ"); SetColor(Color::BRIGHT_GREEN); ResetColor(); Sleep(1500); return;
                }
                std::cout << std::endl; SetColor(Color::BRIGHT_YELLOW);
                std::cout << "  Нажмите Enter..."; ResetColor();
                std::cin.get();
            }
        }
    }

    void TestSplit() {
        OpenBox(); PrintSection("SPLIT"); PrintEmpty();
        int data[] = {1, 2, 0, 3, 4, 0, 5};
        Sequence<int>* seq = new MutableArraySequence<int>(data, 7);
        PrintRowIndented("Исходный: [1, 2, 0, 3, 4, 0, 5]", Color::BRIGHT_BLUE);
        PrintRowIndented("Разделитель: 0", Color::BRIGHT_MAGENTA); PrintEmpty();
        
        auto* splitted = Split(*seq, [](int x) { return x == 0; });
        PrintRowIndented("Результат: [[1,2], [3,4], [5]]", Color::BRIGHT_GREEN);
        PrintRowIndented("Фрагментов: 3", Color::BRIGHT_CYAN);

        IEnumerator<Sequence<int>*>* cleanupEn = splitted->GetEnumerator();
        while (cleanupEn->MoveNext()) {
            delete cleanupEn->Current();
        }
        delete cleanupEn;

        delete splitted; 
        delete seq; 
        CloseBox();
    }
    void TestZipUnzip() {
        OpenBox(); PrintSection("ZIP/UNZIP"); PrintEmpty();
        int data1[] = {1, 2, 3}; int data2[] = {10, 20, 30};
        Sequence<int>* seq1 = new MutableArraySequence<int>(data1, 3);
        Sequence<int>* seq2 = new MutableArraySequence<int>(data2, 3);
        PrintRowIndented("Seq1: [1, 2, 3]", Color::BRIGHT_BLUE);
        PrintRowIndented("Seq2: [10, 20, 30]", Color::BRIGHT_BLUE); PrintEmpty();
        auto* zipped = Zip(*seq1, *seq2);
        PrintRowIndented("Zip Result: [(1,10), (2,20), (3,30)]", Color::BRIGHT_GREEN); PrintEmpty();
        delete zipped; delete seq1; delete seq2; CloseBox();
    }
    void TestSlice() {
        OpenBox(); PrintSection("SLICE"); PrintEmpty();
        int data[] = {1, 2, 3, 4, 5};
        Sequence<int>* seq = new MutableArraySequence<int>(data, 5);
        int insertData[] = {9, 10};
        Sequence<int>* insertSeq = new MutableArraySequence<int>(insertData, 2);
        PrintRowIndented("Исходный: [1, 2, 3, 4, 5]", Color::BRIGHT_BLUE);
        PrintRowIndented("Slice(1, 2, [9, 10])", Color::BRIGHT_MAGENTA); PrintEmpty();
        Sequence<int>* sliced = Slice(*seq, 1, 2, insertSeq);
        PrintRowIndented("Результат: [1, 9, 10, 4, 5]", Color::BRIGHT_GREEN);
        PrintRowIndented("(удалили 2 элемента с позиции 1, вставили [9,10])", Color::BRIGHT_BLACK);
        delete sliced; delete seq; delete insertSeq; CloseBox();
    }
    void TestDynamicArray() {
        int data[] = {10, 20, 30, 40, 50};
        DynamicArray<int> da(data, 5);
        OpenBox(); PrintSection("DYNAMIC ARRAY"); PrintEmpty();
        PrintRowIndented("[ДО] Массив: [10, 20, 30, 40, 50]", Color::BRIGHT_BLUE);
        PrintRowIndented("Размер: " + std::to_string(da.GetSize()), Color::BRIGHT_CYAN); PrintEmpty();
        PrintRowIndented("-> Set(2, 999)", Color::BRIGHT_MAGENTA); da.Set(2, 999);
        PrintRowIndented("[ПОСЛЕ] Массив: [10, 20, 999, 40, 50]", Color::BRIGHT_GREEN); PrintEmpty();
        PrintRowIndented("-> Resize(7)", Color::BRIGHT_MAGENTA); da.Resize(7);
        PrintRowIndented("[ПОСЛЕ] Размер: 7", Color::BRIGHT_GREEN); CloseBox();
    }
    void TestLinkedList() {
        LinkedList<int> ll;
        OpenBox(); PrintSection("LINKED LIST"); PrintEmpty();
        PrintRowIndented("[ДО] Список: []", Color::BRIGHT_BLUE);
        ll.Append(100); ll.Append(200); ll.Append(300);
        PrintRowIndented("[ПОСЛЕ] Append: [100, 200, 300]", Color::BRIGHT_GREEN);
        ll.Prepend(50);
        PrintRowIndented("[ПОСЛЕ] Prepend: [50, 100, 200, 300]", Color::BRIGHT_GREEN);
        ll.InsertAt(150, 2);
        PrintRowIndented("[ПОСЛЕ] InsertAt: [50, 100, 150, 200, 300]", Color::BRIGHT_GREEN); CloseBox();
    }
    void TestMutableSequence() {
        int data[] = {1, 2, 3};
        Sequence<int>* seq = new MutableArraySequence<int>(data, 3);
        OpenBox(); PrintSection("MUTABLE SEQUENCE"); PrintEmpty();
        PrintRowIndented("[ДО] " + PrintSequence(seq), Color::BRIGHT_BLUE);
        seq->Append(4);
        PrintRowIndented("[ПОСЛЕ] Append(4): " + PrintSequence(seq), Color::BRIGHT_GREEN);
        seq->Prepend(0);
        PrintRowIndented("[ПОСЛЕ] Prepend(0): " + PrintSequence(seq), Color::BRIGHT_GREEN);
        seq->Set(1, 999);
        PrintRowIndented("[ПОСЛЕ] Set(1, 999): " + PrintSequence(seq), Color::BRIGHT_MAGENTA);
        CloseBox(); delete seq;
    }
    void TestImmutableSequence() {
        int data[] = {1, 2, 3};
        Sequence<int>* seq = new ImmutableArraySequence<int>(data, 3);
        OpenBox(); PrintSection("IMMUTABLE SEQUENCE"); PrintEmpty();
        PrintRowIndented("Оригинал: [1, 2, 3]", Color::BRIGHT_BLUE);
        Sequence<int>* newSeq = seq->Append(4);
        PrintRowIndented("Оригинал (без изменений): [1, 2, 3]", Color::BRIGHT_BLUE);
        PrintRowIndented("Новый объект: [1, 2, 3, 4]", Color::BRIGHT_GREEN);
        delete seq; delete newSeq; CloseBox();
    }
    void TestMapReduce() {
        int data[] = {1, 2, 3, 4, 5};
        MutableArraySequence<int>* seq = new MutableArraySequence<int>(data, 5);
        OpenBox(); PrintSection("MAP-REDUCE"); PrintEmpty();
        PrintRowIndented("Исходный: [1, 2, 3, 4, 5]", Color::BRIGHT_WHITE);
        Sequence<int>* mapped = seq->Map<int>([](int x) { return x * 2; });
        PrintRowIndented("Map(x*2): [2, 4, 6, 8, 10]", Color::BRIGHT_GREEN); delete mapped;
        Sequence<int>* filtered = seq->Where([](int x) { return x > 2; });
        PrintRowIndented("Where(x>2): [3, 4, 5]", Color::BRIGHT_GREEN); delete filtered;
        int sum = seq->Reduce([](int acc, int x) { return acc + x; }, 0);
        PrintRowIndented("Reduce(сумма): " + std::to_string(sum), Color::BRIGHT_CYAN);
        CloseBox(); delete seq;
    }
    void TestIterators() {
        int data[] = {10, 20, 30, 40, 50};
        Sequence<int>* seq = new MutableArraySequence<int>(data, 5);
        OpenBox(); PrintSection("ИТЕРАТОРЫ"); PrintEmpty();
        IEnumerator<int>* en = seq->GetEnumerator();
        std::string elements = ""; bool first = true;
        while (en->MoveNext()) {
            if (!first) elements += ", ";
            elements += std::to_string(en->Current()); first = false;
        }
        SetColor(Color::BRIGHT_BLACK); std::cout << "|   Элементы: "; ResetColor();
        SetColor(Color::BRIGHT_CYAN); std::cout << elements; ResetColor();
        int visibleElems = VisibleLength(elements);
        int padding = WIDTH - 15 - visibleElems;
        SetColor(Color::BRIGHT_BLACK); PrintChars(' ', padding);
        std::cout << "|" << std::endl; ResetColor();
        PrintEmpty(); PrintRowIndented("[OK] IEnumerator работает", Color::BRIGHT_GREEN);
        CloseBox(); delete en; delete seq;
    }
    void TestOption() {
        int data[] = {1, 2, 3};
        Sequence<int>* seq = new MutableArraySequence<int>(data, 3);
        OpenBox(); PrintSection("OPTION<T>"); PrintEmpty();
        Option<int> opt1 = seq->TryGet(0);
        if (opt1.IsSome()) PrintRowIndented("TryGet(0): IsSome=true, Value=" + std::to_string(opt1.GetValue()), Color::BRIGHT_GREEN);
        Option<int> opt2 = seq->TryGet(100);
        if (opt2.IsNone()) PrintRowIndented("TryGet(100): IsNone=true (без исключения!)", Color::BRIGHT_GREEN);
        CloseBox(); delete seq;
    }
    void TestFromConcat() {
        OpenBox(); PrintSection("FROM / CONCAT"); PrintEmpty();
        int data1[] = {1, 2, 3};
        Sequence<int>* fromSeq = From(data1, 3);
        PrintRowIndented("From([1,2,3]): " + PrintSequence(fromSeq), Color::BRIGHT_GREEN); delete fromSeq;
        PrintEmpty(); int data2[] = {4, 5, 6};
        Sequence<int>* seq1 = From(data1, 3); Sequence<int>* seq2 = From(data2, 3);
        Sequence<int>* concatenated = Concat(*seq1, *seq2);
        PrintRowIndented("Seq1: " + PrintSequence(seq1), Color::BRIGHT_BLUE);
        PrintRowIndented("Seq2: " + PrintSequence(seq2), Color::BRIGHT_BLUE);
        PrintRowIndented("Concat: " + PrintSequence(concatenated), Color::BRIGHT_GREEN);
        delete seq1; delete seq2; delete concatenated; CloseBox();
    }
};