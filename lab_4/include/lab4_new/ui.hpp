#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../lab4_new/cardinal.hpp"
#include "../lab4_new/lazy_sequence.hpp"
#include "../lab4_new/stream.hpp"
#include "../lab4_new/stream_adapters.hpp"
#include "../lab4_new/read_only_stream.hpp"
#include "../lab4_new/write_only_stream.hpp"
#include "../lab4_new/algorithms.hpp"
#include "../lab3_base/array_sequence.h"
#include "../lab3_base/utils.h"

class ConsoleUI {
private:
    static const int WIDTH = 70;

    void PrintHeader(const std::string& text) {
        std::cout << "\n╔"; for(int i=0; i<WIDTH-2; ++i) std::cout << "═"; std::cout << "╗\n";
        std::cout << "║";
        int pad = (WIDTH - 2 - VisibleLength(text)) / 2;
        for(int i=0; i<pad; ++i) std::cout << " ";
        std::cout << text;
        for(int i=0; i<WIDTH-2-VisibleLength(text)-pad; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "╚"; for(int i=0; i<WIDTH-2; ++i) std::cout << "═"; std::cout << "╝\n";
    }

    void PrintOption(int num, const std::string& text) {
        std::cout << "  " << std::setw(2) << num << ". " << text << "\n";
    }

    void PrintSeparator() {
        std::cout << "  "; for(int i=0; i<WIDTH-4; ++i) std::cout << "─"; std::cout << "\n";
    }

    void WaitForEnter() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n  [Нажмите Enter для продолжения...] ";
        std::cin.get();
    }

    template<typename T>
    T GetInput(const std::string& prompt, T minVal = std::numeric_limits<T>::lowest(), T maxVal = std::numeric_limits<T>::max()) {
        T value;
        while (true) {
            std::cout << "  " << prompt;
            if (!(std::cin >> value)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "\n  ❌ Ошибка: введите корректное число.\n";
                continue;
            }
            if (value < minVal || value > maxVal) {
                std::cout << "\n  ❌ Значение должно быть в диапазоне [" << minVal << "; " << maxVal << "]\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }

    std::string GetStringInput(const std::string& prompt) {
        std::string value;
        std::cout << "  " << prompt << ": ";
        std::getline(std::cin, value);
        return value;
    }

    void InteractiveOnlineStats() {
        ClearScreen();
        PrintHeader("📊 ОНЛАЙН-СТАТИСТИКА (режим терминала)");
        std::cout << "  Вводите числа по одному. Статистика обновляется мгновенно.\n";
        std::cout << "  Введите 0 или пустую строку для завершения.\n";
        PrintSeparator();

        OnlineStatistics<double> stats;
        double val;
        while (true) {
            std::cout << "  ➤ Новое значение: ";
            if (!(std::cin >> val)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            if (val == 0 && std::cin.peek() == '\n') { std::cin.ignore(); break; }
            
            stats.Add(val);
            std::cout << "  ✅ Принято. Текущие метрики:\n";
            std::cout << "  ┌─────────────────────────────────────┐\n";
            std::cout << "  │ Элементов: " << std::setw(24) << stats.GetCount() << " │\n";
            std::cout << "  │ Минимум:   " << std::setw(24) << stats.GetMin() << " │\n";
            std::cout << "  │ Максимум:  " << std::setw(24) << stats.GetMax() << " │\n";
            std::cout << "  │ Среднее:   " << std::setw(24) << stats.GetMean() << " │\n";
            std::cout << "  │ Медиана:   " << std::setw(24) << stats.GetMedian() << " │\n";
            std::cout << "  └─────────────────────────────────────┘\n\n";
        }
        WaitForEnter();
    }

    void InteractiveLazySequence() {
        ClearScreen();
        PrintHeader("🧩 КОНСТРУКТОР ЛЕНИВОЙ ПОСЛЕДОВАТЕЛЬНОСТИ");
        PrintOption(1, "Натуральные числа (1, 2, 3...)");
        PrintOption(2, "Факториалы (1, 1, 2, 6, 24...)");
        PrintOption(3, "Фибоначчи (1, 1, 2, 3, 5...)");
        PrintOption(0, "Отмена");
        PrintSeparator();
        
        int rule = GetInput<int>("Выберите правило: ", 0, 3);
        if (rule == 0) return;
        int len = GetInput<int>("Длина последовательности (или -1 для бесконечности): ", -1, 100000);
        
        LazySequence<int>* seq = nullptr;
        switch (rule) {
            case 1:
                seq = new LazySequence<int>(
                    [](Sequence<int>* p) -> int { return p->GetLength() + 1; },
                    (len == -1) ? Cardinal::Infinity() : Cardinal(len)
                );
                break;
            case 2:
                seq = new LazySequence<int>(
                    [](Sequence<int>* p) -> int {
                        int l = p->GetLength();
                        return (l == 0) ? 1 : p->Get(l-1) * l;
                    },
                    (len == -1) ? Cardinal::Infinity() : Cardinal(len)
                );
                break;
            case 3:
            default:
                seq = new LazySequence<int>(
                    [](Sequence<int>* p) -> int {
                        int l = p->GetLength();
                        return (l < 2) ? 1 : p->Get(l-1) + p->Get(l-2);
                    },
                    (len == -1) ? Cardinal::Infinity() : Cardinal(len)
                );
                break;
        }

        while (true) {
            ClearScreen();
            PrintHeader("🔧 ОПЕРАЦИИ НАД ПОСЛЕДОВАТЕЛЬНОСТЬЮ");
            std::cout << "  Текущая длина: " << (seq->GetLength() == -1 ? "∞" : std::to_string(seq->GetLength())) << "\n";
            std::cout << "  Материализовано: " << seq->GetMaterializedCount() << "\n";
            PrintSeparator();
            PrintOption(1, "Получить элемент по индексу");
            PrintOption(2, "Map (преобразование)");
            PrintOption(3, "Where (фильтрация чётных)");
            PrintOption(4, "Reduce (сумма)");
            PrintOption(5, "Concat (сцепить с другой)");
            PrintOption(6, "InsertAt (вставка элемента)");
            PrintOption(7, "GetSubsequence");
            PrintOption(0, "Выйти в главное меню");
            PrintSeparator();
            
            int choice = GetInput<int>("Ваш выбор: ", 0, 7);
            if (choice == 0) { delete seq; return; }
            
            ClearScreen();
            try {
                if (choice == 1) {
                    int idx = GetInput<int>("Индекс: ", 0, (seq->GetLength() == -1) ? 1000 : seq->GetLength()-1);
                    std::cout << "\n  ✅ seq[" << idx << "] = " << seq->Get(idx) << "\n";
                } else if (choice == 2) {
                    auto* mapped = seq->Map<int>([](int x){ return x * x; });
                    std::cout << "\n  ✅ Map(x->x²) первые 5 элементов: ";
                    for(int i=0; i<5; ++i) 
                        std::cout << mapped->Get(i) << " ";
                    std::cout << "\n  Материализовано в новой последовательности: " << 
                        dynamic_cast<LazySequence<int>*>(mapped)->GetMaterializedCount() << "\n";
                    delete mapped;
                } else if (choice == 3) {
                    auto* filtered = seq->Where([](int x){ return x % 2 == 0; });
                    std::cout << "\n  ✅ Where(чётные) первые 3 элемента: ";
                    for(int i=0; i<3; ++i) 
                        std::cout << filtered->Get(i) << " ";
                    std::cout << "\n";
                    delete filtered;
                } else if (choice == 4) {
                    if (seq->GetLength() > 500) { std::cout << "\n  ⚠️ Reduce доступна только для последовательностей ≤ 500 элементов.\n"; }
                    else {
                        int sum = seq->Reduce([](int acc, int v){ return acc + v; }, 0);
                        std::cout << "\n  ✅ Reduce(сумма) = " << sum << "\n";
                    }
                } else if (choice == 5) {
                    int addLen = GetInput<int>("Длина добавляемой последовательности: ", 1, 10);
                    auto* other = new LazySequence<int>([](Sequence<int>* p){ return p->GetLength() + 100; }, Cardinal(addLen));
                    auto* concated = seq->Concat(*other);
                    std::cout << "\n  ✅ Concat первые 3 элемента второй части: ";
                    for(int i=0; i<3; ++i) std::cout << concated->Get(seq->GetLength() + i) << " ";
                    std::cout << "\n  Итоговая длина: " << (concated->GetLength()==-1 ? "∞" : std::to_string(concated->GetLength())) << "\n";
                    delete concated; delete other;
                } else if (choice == 6) {
                    int idx = GetInput<int>("Позиция вставки: ", 0, (seq->GetLength()==-1)?100:seq->GetLength());
                    int val = GetInput<int>("Значение: ");
                    auto* inserted = seq->InsertAt(val, idx);
                    std::cout << "\n  ✅ Вставлено " << val << " в позицию " << idx << ". Новый элемент: " << inserted->Get(idx) << "\n";
                    delete inserted;
                } else if (choice == 7) {
                    int s = GetInput<int>("Начало: ", 0, (seq->GetLength()==-1)?100:seq->GetLength()-1);
                    int e = GetInput<int>("Конец: ", s, (seq->GetLength()==-1)?100:seq->GetLength()-1);
                    auto* sub = seq->GetSubsequence(s, e);
                    std::cout << "\n  ✅ Subsequence[" << s << ".." << e << "]: ";
                    for(int i=0; i<sub->GetLength(); ++i) std::cout << sub->Get(i) << " ";
                    std::cout << "\n";
                    delete sub;
                }
            } catch (const std::exception& ex) {
                std::cout << "\n  ❌ Ошибка: " << ex.what() << "\n";
            }
            WaitForEnter();
        }
    }

    void InteractiveStreams() {
        ClearScreen();
        PrintHeader("🌊 ТЕСТИРОВАНИЕ ПОТОКОВ");
        PrintOption(1, "Чтение из Sequence");
        PrintOption(2, "Чтение из CSV-файла");
        PrintOption(3, "Чтение из JSON-файла");
        PrintOption(4, "Запись в Sequence");
        PrintOption(0, "Отмена");
        PrintSeparator();
        
        int choice = GetInput<int>("Источник потока: ", 0, 4);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                int data[] = {42, 7, 19, 3, 88, 15, 51};
                auto* seq = new MutableArraySequence<int>(data, 7);
                auto* stream = new SequenceReadStream<int>(seq);
                stream->Open();
                std::cout << "\n  📥 Поток: ";
                while (!stream->IsEndOfStream()) std::cout << stream->Read() << " ";
                std::cout << "\n  Позиция в конце: " << stream->GetPosition() << "\n";
                stream->Close();
                delete stream; delete seq;
            } else if (choice == 2) {
                std::string fname = "test_stream.csv";
                std::ofstream f(fname); f << "10,20\n30,40\n50,60\n"; f.close();
                
                auto* fileStr = new FileStream(fname, [](const std::string& l){ return l; });
                auto* csv = new CsvReadStream<int>(fileStr, [](Sequence<std::string>* fields){
                    return fields->GetLength() > 0 ? std::stoi(fields->Get(0)) : 0;
                }, ',');
                csv->Open();
                std::cout << "\n  📥 CSV поток (первый столбец): ";
                while (!csv->IsEndOfStream()) std::cout << csv->Read() << " ";
                std::cout << "\n";
                csv->Close(); delete csv; delete fileStr;
                std::remove(fname.c_str());
            } else if (choice == 3) {
                std::string fname = "test_stream.json";
                std::ofstream f(fname); f << "[100, 200, 300]"; f.close();
                
                Sequence<int>* items = new MutableArraySequence<int>();
                std::ifstream fin(fname); std::string content((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
                std::string num;
                for(char c : content) {
                    if (std::isdigit(c) || c=='-') num += c;
                    else if (!num.empty()) { items->Append(std::stoi(num)); num.clear(); }
                }
                auto* json = new JsonReadStream<int>(items);
                json->Open();
                std::cout << "\n  📥 JSON поток: ";
                while (!json->IsEndOfStream()) std::cout << json->Read() << " ";
                std::cout << "\n";
                json->Close(); delete json; delete items;
                std::remove(fname.c_str());
            } else if (choice == 4) {
                auto* seq = new MutableArraySequence<int>();
                auto* wStream = new SequenceWriteStream<int>(seq);
                wStream->Open();
                for(int i=1; i<=5; ++i) wStream->Write(i*10);
                wStream->Close();
                std::cout << "\n  📤 Записано 5 элементов. Проверка чтения: ";
                for(int i=0; i<seq->GetLength(); ++i) std::cout << seq->Get(i) << " ";
                std::cout << "\n";
                delete wStream; delete seq;
            }
        } catch (const std::exception& ex) {
            std::cout << "\n  ❌ Ошибка потока: " << ex.what() << "\n";
        }
        WaitForEnter();
    }

    void DemoCardinalOrdinal() {
        ClearScreen();
        PrintHeader("♾️ АЛГЕБРА КАРДИНАЛОВ И ОРДИНАЛОВ");
        int n = GetInput<int>("Введите конечное n: ", 0, 1000000);
        Cardinal c1(n), c2 = Cardinal::Infinity();
        
        std::cout << "\n  Кардинал " << c1.ToString() << " + ∞ = " << (c1 + c2).ToString() << "\n";
        std::cout << "  Кардинал " << c1.ToString() << " < ∞ : " << (c1 < c2) << "\n";
        
        OrdinalIndex o1(0, n), o2(1, 0), o3(2, 5);
        std::cout << "\n  Ординалы:\n";
        std::cout << "    ω·0 + " << n << "  <  ω·1 + 0   : " << (o1 < o2) << "\n";
        std::cout << "    ω·1 + 0  <  ω·2 + 5   : " << (o2 < o3) << "\n";
        std::cout << "    Визуализация: " << o1.ToString() << " -> " << o2.ToString() << " -> " << o3.ToString() << "\n";
        
        WaitForEnter();
    }

    void DemoChains() {
        ClearScreen();
        PrintHeader("⛓️ ЦЕПОЧКИ ЛЕНИВЫХ ОПЕРАЦИЙ");
        auto* fib = new LazySequence<int>(
            [](Sequence<int>* p){ return p->GetLength()<2 ? 1 : p->Get(p->GetLength()-1)+p->Get(p->GetLength()-2); },
            Cardinal::Infinity()
        );
        
        auto* mapped = fib->Map<int>([](int x){ return x * 2; });
        auto* filtered = mapped->Where([](int x){ return x % 3 == 0; });
        
        std::cout << "  Фибоначчи -> x2 -> фильтр(делится на 3)\n";
        std::cout << "  Первые 4 элемента цепочки:\n";
        for(int i=0; i<4; ++i) {
            std::cout << "  [" << i << "] = " << filtered->Get(i) << "\n";
        }
        auto* lazyFiltered = dynamic_cast<LazySequence<int>*>(filtered);
        std::cout << "\n  Материализовано в фильтре: " << (lazyFiltered ? lazyFiltered->GetMaterializedCount() : 0) << " (ленивая оценка!)\n";
        
        delete filtered; delete mapped; delete fib;
        WaitForEnter();
    }

    void RunBenchmark() {
        ClearScreen();
        PrintHeader("⚡ БЕНЧМАРК ПРОИЗВОДИТЕЛЬНОСТИ");
        int size = GetInput<int>("Размер теста N: ", 1000, 500000);
        PrintSeparator();

        auto start = std::chrono::high_resolution_clock::now();
        auto* lazy = new LazySequence<int>([](Sequence<int>* p){ return p->GetLength() + 1; }, Cardinal(size));
        volatile int dummy = lazy->Get(size - 1);
        (void)dummy;
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "  ⏱️  LazySequence Get(" << size-1 << "): " << ms << " мс\n";
        delete lazy;

        start = std::chrono::high_resolution_clock::now();
        int* data = new int[size];
        for(int i=0; i<size; ++i) data[i] = size - i;
        auto* seq = new MutableArraySequence<int>(data, size);
        auto* stream = new SequenceReadStream<int>(seq);
        StreamSorter<int> sorter([](int a, int b){ return a < b; });
        auto* sorted = sorter.SortWithHeap(stream);
        sorted->Open();
        while(!sorted->IsEndOfStream()) sorted->Read();
        sorted->Close();
        end = std::chrono::high_resolution_clock::now();
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "  ⏱️  HeapSort(" << size << "): " << ms << " мс\n";
        
        delete sorted; delete stream; delete seq; delete[] data;
        WaitForEnter();
    }

    void RunAllTests() {
        ClearScreen();
        PrintHeader("ЗАПУСК АВТОМАТИЧЕСКИХ ТЕСТОВ");
        std::cout << "  📜 Выполните в терминале: make clean && make test && ./test\n";
        std::cout << "  (Тесты вынесены в отдельный бинарник для изоляции окружения)\n";
        WaitForEnter();
    }

public:
    void ShowMenu() {
        while (true) {
            ClearScreen();
            PrintHeader("ЛАБОРАТОРНАЯ РАБОТА №4");
            std::cout << "  Ленивые последовательности, потоки и статистика\n";
            PrintSeparator();
            PrintOption(1, "📊 Онлайн-статистика (терминал)");
            PrintOption(2, "🧩 Конструктор LazySequence + операции");
            PrintOption(3, "🌊 Потоки ввода/вывода (Seq/CSV/JSON)");
            PrintOption(4, "♾️ Кардиналы и Ординалы");
            PrintOption(5, "⛓️ Цепочки Map → Where → Reduce");
            PrintOption(6, "⚡ Бенчмарк производительности");
            PrintSeparator();
            PrintOption(7, "🧪 Запустить автоматические тесты");
            PrintOption(0, "🚪 Выход");
            PrintSeparator();
            
            int choice = GetInput<int>("Ваш выбор: ", 0, 7);
            switch (choice) {
                case 1: InteractiveOnlineStats(); break;
                case 2: InteractiveLazySequence(); break;
                case 3: InteractiveStreams(); break;
                case 4: DemoCardinalOrdinal(); break;
                case 5: DemoChains(); break;
                case 6: RunBenchmark(); break;
                case 7: RunAllTests(); break;
                case 0: return;
                default:
                    std::cout << "\n  ❌ Неверный выбор! Попробуйте снова.\n";
                    WaitForEnter();
            }
        }
    }

    void ClearScreen() {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[H";
#endif
    }
};