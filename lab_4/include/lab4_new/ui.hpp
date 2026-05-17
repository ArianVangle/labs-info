#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include "../include/lab4_new/cardinal.hpp"
#include "../include/lab4_new/lazy_sequence.hpp"
#include "../include/lab4_new/stream.hpp"
#include "../include/lab4_new/stream_adapters.hpp"
#include "../include/lab4_new/read_only_stream.hpp"
#include "../include/lab4_new/algorithms.hpp"
#include "../include/lab3_base/array_sequence.h"
#include "../include/lab3_base/utils.h"

class ConsoleUI {
private:
    static const int WIDTH = 70;
    
    void PrintHeader(const std::string& text) {
        std::cout << "\n╔";
        for (int i = 0; i < WIDTH - 2; i++) std::cout << "═";
        std::cout << "╗\n║";
        
        int padding = (WIDTH - 2 - VisibleLength(text)) / 2;
        for (int i = 0; i < padding; i++) std::cout << " ";
        std::cout << text;
        for (int i = 0; i < WIDTH - 2 - VisibleLength(text) - padding; i++) std::cout << " ";
        std::cout << "║\n╚";
        for (int i = 0; i < WIDTH - 2; i++) std::cout << "═";
        std::cout << "╝\n\n";
    }
    
    void PrintOption(int num, const std::string& text) {
        std::cout << "  " << num << ". " << text << "\n";
    }
    
    void PrintSeparator() {
        std::cout << "  ";
        for (int i = 0; i < WIDTH - 4; i++) std::cout << "─";
        std::cout << "\n";
    }
    
    void WaitForEnter() {
        std::cout << "\n  Нажмите Enter для продолжения...";
        std::cin.get();
    }
    
    int GetIntInput(const std::string& prompt, int minVal, int maxVal) {
        int value;
        while (true) {
            std::cout << "  " << prompt << " (" << minVal << "-" << maxVal << "): ";
            std::cin >> value;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            if (std::cin.fail() || value < minVal || value > maxVal) {
                std::cout << "  Ошибка: введите число от " << minVal << " до " << maxVal << "\n";
                std::cin.clear();
            } else {
                return value;
            }
        }
    }
    
    std::string GetStringInput(const std::string& prompt) {
        std::string value;
        std::cout << "  " << prompt << ": ";
        std::getline(std::cin, value);
        return value;
    }

public:
    void ShowMenu() {
        while (true) {
            PrintHeader("ЛАБОРАТОРНАЯ РАБОТА №4");
            std::cout << "  Ленивые последовательности и потоки данных\n\n";
            
            PrintSeparator();
            PrintOption(1, "Демо: Факториалы (ленивая последовательность)");
            PrintOption(2, "Демо: Фибоначчи (бесконечная последовательность)");
            PrintOption(3, "Демо: Частичная материализация");
            PrintOption(4, "Демо: Сортировка потока (куча)");
            PrintOption(5, "Демо: Онлайн-статистика");
            PrintSeparator();
            PrintOption(6, "Ручной режим: Создать ленивую последовательность");
            PrintOption(7, "Ручной режим: Тестирование потоков");
            PrintOption(8, "Ручной режим: Тест с файлом CSV");
            PrintOption(9, "Ручной режим: Тест с файлом JSON");
            PrintSeparator();
            PrintOption(10, "Запустить все автоматические тесты");
            PrintOption(11, "Бенчмарк производительности");
            PrintSeparator();
            PrintOption(0, "Выход");
            PrintSeparator();
            
            std::cout << "\n  Ваш выбор: ";
            int choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            switch (choice) {
                case 1: DemoFactorials(); break;
                case 2: DemoFibonacci(); break;
                case 3: DemoPartialMaterialization(); break;
                case 4: DemoHeapSort(); break;
                case 5: DemoOnlineStats(); break;
                case 6: ManualLazySequence(); break;
                case 7: ManualStreams(); break;
                case 8: ManualCSVTest(); break;
                case 9: ManualJSONTest(); break;
                case 10: RunAllTests(); break;
                case 11: RunBenchmark(); break;
                case 0: return;
                default: 
                    std::cout << "  Неверный выбор! Попробуйте снова.\n";
                    WaitForEnter();
            }
        }
    }
    
void DemoFactorials() {
    PrintHeader("ФАКТОРИАЛЫ (ленивая последовательность)");
    
    auto* factorials = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            if (prev->GetLength() == 0) return 1;
            return prev->Get(prev->GetLength() - 1) * (int)prev->GetLength();
        },
        Cardinal(10)
    );
        
        std::cout << "  Первые 10 факториалов:\n  ";
        for (int i = 0; i < 10; i++) {
            std::cout << factorials->Get(i);
            if (i < 9) std::cout << ", ";
        }
        std::cout << "\n\n";
        
        std::cout << "  Материализовано: " << factorials->GetMaterializedCount() << " из 10\n";
        std::cout << "  Эффективность мемоизации: " << factorials->GetMemoizationRatio() << "%\n";
        
        delete factorials;
        WaitForEnter();
    }
    
    void DemoFibonacci() {
        PrintHeader("ФИБОНАЧЧИ (бесконечная ленивая последовательность)");
        
        auto* fib = new LazySequence<int>(
            [](Sequence<int>* prev) -> int {
                if (prev->GetLength() < 2) return 1;
                return prev->Get(prev->GetLength() - 1) + prev->Get(prev->GetLength() - 2);
            },
            Cardinal::Infinity()
        );
        
        std::cout << "  Первые 12 чисел Фибоначчи:\n  ";
        for (int i = 0; i < 12; i++) {
            std::cout << fib->Get(i);
            if (i < 11) std::cout << ", ";
        }
        std::cout << "\n\n";
        
        std::cout << "  Длина: ∞ (бесконечная)\n";
        std::cout << "  Материализовано: " << fib->GetMaterializedCount() << "\n";
        
        delete fib;
        WaitForEnter();
    }
    
    void DemoHeapSort() {
        PrintHeader("СОРТИРОВКА ПОТОКА (бинарная куча)");
        
        int data[] = {42, 7, 19, 3, 88, 15, 51};
        auto* seq = new MutableArraySequence<int>(data, 7);
        auto* stream = new SequenceReadStream<int>(seq);
        
        std::cout << "  Исходный поток: ";
        stream->Open();
        while (!stream->IsEndOfStream()) {
            std::cout << stream->Read() << " ";
        }
        stream->Close();
        
        StreamSorter<int> sorter([](int a, int b) -> bool { return a < b; });
        auto* sorted = sorter.SortWithHeap(new SequenceReadStream<int>(
            new MutableArraySequence<int>(data, 7)
        ));
        
        std::cout << "\n  Отсортированный: ";
        sorted->Open();
        while (!sorted->IsEndOfStream()) {
            std::cout << sorted->Read() << " ";
        }
        sorted->Close();
        std::cout << "\n";
        
        delete sorted;
        delete stream;
        delete seq;
        WaitForEnter();
    }
    
    void DemoOnlineStats() {
        PrintHeader("ОНЛАЙН-СТАТИСТИКА");
        
        OnlineStatistics<int> stats;
        int data[] = {10, 25, 3, 47, 19, 33, 8};
        
        std::cout << "  Данные: ";
        for (int val : data) {
            std::cout << val << " ";
            stats.Add(val);
        }
        std::cout << "\n\n";
        
        std::cout << "  Количество: " << stats.GetCount() << "\n";
        std::cout << "  Среднее:    " << stats.GetMean() << "\n";
        std::cout << "  Медиана:    " << stats.GetMedian() << "\n";
        std::cout << "  Минимум:    " << stats.GetMin() << "\n";
        std::cout << "  Максимум:   " << stats.GetMax() << "\n";
        
        WaitForEnter();
    }
    
    void ManualLazySequence() {
        PrintHeader("РУЧНОЙ РЕЖИМ: ЛЕНИВАЯ ПОСЛЕДОВАТЕЛЬНОСТЬ");
        
        int bufferSize = GetIntInput("Размер скользящего буфера генератора", 1, 1000);
        
        int length = GetIntInput("Длина последовательности", 1, 1000);
        
        std::cout << "\n  Выберите правило генерации:\n";
        std::cout << "  1. Натуральные числа (1, 2, 3, ...)\n";
        std::cout << "  2. Факториалы (1, 1, 2, 6, 24, ...)\n";
        std::cout << "  3. Фибоначчи (1, 1, 2, 3, 5, ...)\n";
        int rule = GetIntInput("Ваш выбор", 1, 3);
        
        LazySequence<int>* seq = nullptr;
        
        switch (rule) {
            case 1:
                seq = new LazySequence<int>(
                    [](Sequence<int>* prev) -> int {
                        return prev->GetLength() + 1;
                    },
                    Cardinal(length)
                );
                break;
            case 2:
                seq = new LazySequence<int>(
                    [](Sequence<int>* prev) -> int {
                        if (prev->GetLength() == 0) return 1;
                        return prev->Get(prev->GetLength() - 1) * (int)(prev->GetLength() + 1);
                    },
                    Cardinal(length)
                );
                break;
            case 3:
            default:
                seq = new LazySequence<int>(
                    [](Sequence<int>* prev) -> int {
                        if (prev->GetLength() < 2) return 1;
                        return prev->Get(prev->GetLength() - 1) + prev->Get(prev->GetLength() - 2);
                    },
                    Cardinal(length)
                );
                break;
        }
        
        std::cout << "\n  Сгенерированная последовательность:\n  ";
        int displayCount = (length < 20) ? length : 20;
        for (int i = 0; i < displayCount; i++) {
            std::cout << seq->Get(i) << " ";
        }
        if (length > 20) std::cout << "... (всего " << length << ")";
        std::cout << "\n\n";
        
        std::cout << "  Материализовано: " << seq->GetMaterializedCount() << "\n";
        std::cout << "  Размер буфера генератора: " << bufferSize << "\n";
        
        delete seq;
        WaitForEnter();
    }
    
    void ManualStreams() {
        PrintHeader("РУЧНОЙ РЕЖИМ: ПОТОКИ");
        
        int count = GetIntInput("Количество элементов", 1, 100);
        
        int* data = new int[count];
        std::cout << "  Введите " << count << " целых чисел:\n  ";
        for (int i = 0; i < count; i++) {
            std::cin >> data[i];
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        auto* seq = new MutableArraySequence<int>(data, count);
        auto* stream = new SequenceReadStream<int>(seq);
        
        stream->Open();
        std::cout << "\n  Чтение из потока:\n  ";
        while (!stream->IsEndOfStream()) {
            std::cout << stream->Read() << " ";
        }
        stream->Close();
        
        std::cout << "\n\n  Позиция в потоке: " << stream->GetPosition() << "\n";
        
        delete stream;
        delete seq;
        delete[] data;
        WaitForEnter();
    }
    
    void ManualCSVTest() {
    PrintHeader("РУЧНОЙ РЕЖИМ: ТЕСТ С ФАЙЛОМ CSV");
    
    std::string filename = GetStringInput("Имя файла CSV");
    
    if (filename.empty()) {
        filename = "test_data.csv";
        std::ofstream testFile(filename);
        testFile << "1\n2\n3\n4\n5\n10\n20\n30\n";
        testFile.close();
        std::cout << "  Создан тестовый файл: " << filename << "\n";
    }
    
    try {
        auto* fileStream = new FileStream(filename, 
            [](const std::string& line) -> std::string { return line; });
        
        auto* csvStream = new CsvReadStream<int>(fileStream,
            [](Sequence<std::string>* fields) -> int {
                if (fields->GetLength() == 0) return 0;
                return std::stoi(fields->Get(0));
            },
            ','
        );
        
        csvStream->Open();
        
        std::cout << "\n  Чтение из CSV:\n  ";
        int count = 0;
        while (!csvStream->IsEndOfStream() && count < 20) {
            std::cout << csvStream->Read() << " ";
            count++;
        }
        if (!csvStream->IsEndOfStream()) std::cout << "...";
        std::cout << "\n\n";
        
        csvStream->Close();
        delete csvStream;
        delete fileStream;
        
    } catch (const std::exception& e) {
        std::cout << "  Ошибка: " << e.what() << "\n";
    }
    
    WaitForEnter();
    }

    void ManualJSONTest() {
        PrintHeader("РУЧНОЙ РЕЖИМ: ТЕСТ С ФАЙЛОМ JSON");
        
        std::string filename = GetStringInput("Имя файла JSON");
        
        if (filename.empty()) {
            filename = "test_data.json";
            std::ofstream testFile(filename);
            testFile << "[10, 20, 30, 40, 50, 100, 200, 300]";
            testFile.close();
            std::cout << "  Создан тестовый файл: " << filename << "\n";
        }
        
        try {
            std::ifstream file(filename);
            if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            file.close();
            
            Sequence<int>* items = new MutableArraySequence<int>();
            std::string num;
            for (char c : content) {
                if (isdigit(c) || c == '-') {
                    num += c;
                } else if (!num.empty()) {
                    items->Append(std::stoi(num));
                    num.clear();
                }
            }
            
            auto* jsonStream = new JsonReadStream<int>(items);
            
            jsonStream->Open();
            
            std::cout << "\n  Чтение из JSON:\n  ";
            while (!jsonStream->IsEndOfStream()) {
                std::cout << jsonStream->Read() << " ";
            }
            std::cout << "\n\n";
            
            jsonStream->Close();
            delete jsonStream;
            delete items;
            
        } catch (const std::exception& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        }
        
        WaitForEnter();
    }
    
    void RunAllTests() {
        PrintHeader("ЗАПУСК ВСЕХ АВТОМАТИЧЕСКИХ ТЕСТОВ");
        
        TestCardinal();
        TestLazySequence();
        TestGenerator();
        TestStreams();
        TestAlgorithms();
        TestPerformance();
        
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                  ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ                      ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        
        WaitForEnter();
    }
    
    void RunBenchmark() {
    PrintHeader("БЕНЧМАРК ПРОИЗВОДИТЕЛЬНОСТИ");
    
    int bufferSize = GetIntInput("Размер скользящего буфера генератора", 1, 10000);
    
    const int SIZES[] = {1000, 10000, 100000};
    
    std::cout << "\n  Ленивая последовательность (доступ по индексу):\n";
    for (int size : SIZES) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto* lazy = new LazySequence<int>(
            [](Sequence<int>* prev) -> int {
                return prev->GetLength() + 1;
            },
            Cardinal(size)
        );
        
        volatile int dummy = lazy->Get(size - 1);
        (void)dummy;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    N=" << size << ": " << duration.count() << " мс"
                  << " (материализовано: " << lazy->GetMaterializedCount() << ")\n";
        
        delete lazy;
    }
    
        
        std::cout << "\n  Сортировка кучей:\n";
        for (int size : SIZES) {
            auto* data = new int[size];
            for (int i = 0; i < size; i++) data[i] = size - i;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            auto* seq = new MutableArraySequence<int>(data, size);
            auto* stream = new SequenceReadStream<int>(seq);
            StreamSorter<int> sorter([](int a, int b) -> bool { return a < b; });
            auto* sorted = sorter.SortWithHeap(stream);
            
            sorted->Open();
            while (!sorted->IsEndOfStream()) sorted->Read();
            sorted->Close();
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::cout << "    N=" << size << ": " << duration.count() << " мс\n";
            
            delete sorted;
            delete stream;
            delete seq;
            delete[] data;
        }
        
        std::cout << "\n  Размер буфера генератора: " << bufferSize << "\n";
        WaitForEnter();
    }
    void DemoPartialMaterialization() {
    PrintHeader("ЧАСТИЧНАЯ МАТЕРИАЛИЗАЦИЯ");
    
    std::cout << "  Создаём последовательность натуральных чисел (1, 2, 3, ...)\n";
    std::cout << "  Длина: 100 элементов\n\n";
    
    auto* seq = new LazySequence<int>(
        [](Sequence<int>* prev) -> int {
            return prev->GetLength() + 1;
        },
        Cardinal(100)
    );
    
    std::cout << "  ┌───────────────────────────────────────────────┐\n";
    std::cout << "  │  Шаг  │  Действие           │  Материализовано│\n";
    std::cout << "  ├───────────────────────────────────────────────┤\n";
    
    std::cout << "  │   1   │  Создание           │  ";
    std::cout << seq->GetMaterializedCount();
    std::cout << " из 100       │\n";
    
    volatile int val1 = seq->Get(0);
    (void)val1;
    std::cout << "  │   2   │  Get(0)             │  ";
    std::cout << seq->GetMaterializedCount();
    std::cout << " из 100       │\n";
    
    volatile int val2 = seq->Get(5);
    (void)val2;
    std::cout << "  │   3   │  Get(5)             │  ";
    std::cout << seq->GetMaterializedCount();
    std::cout << " из 100       │\n";
    
    volatile int val3 = seq->Get(2);
    (void)val3;
    std::cout << "  │   4   │  Get(2) (из кэша)   │  ";
    std::cout << seq->GetMaterializedCount();
    std::cout << " из 100       │\n";
    
    volatile int val4 = seq->Get(20);
    (void)val4;
    std::cout << "  │   5   │  Get(20)            │  ";
    std::cout << seq->GetMaterializedCount();
    std::cout << " из 100      │\n";
    
    volatile int val5 = seq->Get(50);
    (void)val5;
    std::cout << "  │   6   │  Get(50)            │  ";
    std::cout << seq->GetMaterializedCount();
    std::cout << " из 100      │\n";
    
    std::cout << "  └───────────────────────────────────────────────┘\n\n";
    
    std::cout << "  Вывод:\n";
    std::cout << "  • При Get(index) материализуются ВСЕ элементы от 0 до index\n";
    std::cout << "  • Повторный доступ к тем же элементам — из кэша (O(1))\n";
    std::cout << "  • Элементы после последнего запрошенного — НЕ материализованы\n";
    std::cout << "  • Эффективность мемоизации: " << seq->GetMemoizationRatio() << "%\n";
    
    delete seq;
    WaitForEnter();
}
    
private:
    void TestCardinal() {
        std::cout << "\n=== Тесты Cardinal ===\n";
        
        Cardinal finite(42);
        Cardinal infinite = Cardinal::Infinity();
        
        bool passed = finite.IsFinite() && 
                      infinite.IsInfinite() && 
                      (finite.GetValue() == 42) && 
                      (finite < infinite);
        
        std::cout << "  " << (passed ? "✓" : "✗") << " Базовые операции Cardinal\n";
    }
    
    void TestLazySequence() {
        std::cout << "\n=== Тесты LazySequence ===\n";
        
auto* factorials = new LazySequence<int>(
    [](Sequence<int>* prev) -> int {
        if (prev->GetLength() == 0) return 1;
        return prev->Get(prev->GetLength() - 1) * (int)prev->GetLength();
    },
    Cardinal(10)
);
        
        bool passed = (factorials->Get(0) == 1) && 
                      (factorials->Get(4) == 24) &&
                      (factorials->GetMaterializedCount() <= 5);
        
        std::cout << "  " << (passed ? "✓" : "✗") << " Факториалы\n";
        
        delete factorials;
    }
    
    void TestGenerator() {
        std::cout << "\n=== Тесты Generator ===\n";
        
        auto* seq = new LazySequence<int>(
            [](Sequence<int>* prev) -> int {
                return prev->GetLength() + 1;
            },
            Cardinal(100)
        );
        
        auto* gen = new RecursiveGenerator<int>(seq, 
            [](Sequence<int>* prev) -> int {
                return prev->GetLength() + 1;
            },
            10
        );
        
        bool passed = gen->HasNext() && (gen->GetNext() > 0);
        
        std::cout << "  " << (passed ? "✓" : "✗") << " Generator\n";
        
        delete gen;
        delete seq;
    }
    
    void TestStreams() {
        std::cout << "\n=== Тесты потоков ===\n";
        
        int data[] = {1, 2, 3, 4, 5};
        auto* seq = new MutableArraySequence<int>(data, 5);
        auto* stream = new SequenceReadStream<int>(seq);
        
        stream->Open();
        bool passed = !stream->IsEndOfStream() && 
                      (stream->Read() == 1) && 
                      (stream->Read() == 2);
        stream->Close();
        
        std::cout << "  " << (passed ? "✓" : "✗") << " SequenceReadStream\n";
        
        delete stream;
        delete seq;
    }
    
    void TestAlgorithms() {
        std::cout << "\n=== Тесты алгоритмов ===\n";
        
        OnlineStatistics<int> stats;
        stats.Add(10);
        stats.Add(20);
        stats.Add(30);
        
        bool passed = (stats.GetCount() == 3) && 
                      (stats.GetMean() == 20) &&
                      (stats.GetMin() == 10);
        
        std::cout << "  " << (passed ? "✓" : "✗") << " Онлайн-статистика\n";
    }
    
    void TestPerformance() {
        std::cout << "\n=== Тесты производительности ===\n";
        
        const int SIZE = 10000;
        
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
        
        bool passed = (lazy->GetMaterializedCount() == (size_t)SIZE) && 
                      (duration.count() < 5000);
        
        std::cout << "  " << (passed ? "✓" : "✗") << " Большие данные (" << SIZE 
                  << " элементов: " << duration.count() << " мс)\n";
        
        delete lazy;
    }
};