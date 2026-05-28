#pragma once
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "../lab3_base/array_sequence.h"
#include "../lab3_base/utils.h"
#include "../lab4_new/algorithms.hpp"
#include "../lab4_new/cardinal.hpp"
#include "../lab4_new/lazy_sequence.hpp"
#include "../lab4_new/read_only_stream.hpp"
#include "../lab4_new/stream.hpp"
#include "../lab4_new/stream_adapters.hpp"
#include "../lab4_new/write_only_stream.hpp"

static int ReadKey() {
#ifdef _WIN32
	int ch = _getch();
	if (ch == 0 || ch == 224) {
		int dir = _getch();
		if (dir == 72)
			return 38;
		if (dir == 80)
			return 40;
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
		if (ch == 'A')
			ch = 38;
		else if (ch == 'B')
			ch = 40;
	}
	if (ch == '\n' || ch == '\r')
		ch = 13;
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
#endif
}

class ConsoleUI {
  private:
	static const int WIDTH = 70;
	void PrintHeader(const std::string &text) {
		std::cout << "\n╔";
		for (int i = 0; i < WIDTH - 4; ++i)
			std::cout << "═";
		std::cout << "╗\n";
		std::cout << "  ║";
		int pad = (WIDTH - 4 - (int)text.length()) / 2;
		for (int i = 0; i < pad; ++i)
			std::cout << " ";
		std::cout << text;
		for (int i = 0; i < WIDTH - 4 - (int)text.length() - pad; ++i)
			std::cout << " ";
		std::cout << "║\n";
		std::cout << "  ╚";
		for (int i = 0; i < WIDTH - 4; ++i)
			std::cout << "═";
		std::cout << "╝\n";
	}
	void PrintSeparator() {
		std::cout << "  ";
		for (int i = 0; i < WIDTH - 6; ++i)
			std::cout << "─";
		std::cout << "\n";
	}
	void ClearScreen() {
#ifdef _WIN32
		system("cls");
#else
		std::cout << "\033[2J\033[H";
#endif
	}
	void WaitForEnter() {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "\n[Нажмите Enter для возврата в меню...] ";
		char c;
		while (std::cin.get(c) && c != '\n') {
		}
	}
	template <typename T>
	T GetInput(const std::string &prompt,
			   T minVal = std::numeric_limits<T>::lowest(),
			   T maxVal = std::numeric_limits<T>::max()) {
		T value;
		while (true) {
			std::cout << "  " << prompt;
			if (!(std::cin >> value)) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
								'\n');
				std::cout << "\n❌ Ошибка: введите корректное число.\n";
				continue;
			}
			if (value < minVal || value > maxVal) {
				std::cout << "\n❌ Значение должно быть в диапазоне [" << minVal
						  << "; " << maxVal << "]\n";
				continue;
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return value;
		}
	}
	int SelectWithArrows(const std::string &title, const char **options,
						 int count) {
		int selected = 0;
		while (true) {
			ClearScreen();
			PrintHeader(title);
			PrintSeparator();
			for (int i = 0; i < count; ++i) {
				if (i == selected)
					std::cout << "  \033[97m▶ " << options[i] << "\033[0m\n";
				else
					std::cout << "    " << options[i] << "\n";
			}
			PrintSeparator();
			std::cout << "  ↑↓ навигация, Enter - выбор\n";
			int key = ReadKey();
			if (key == 38)
				selected = (selected > 0) ? selected - 1 : count - 1;
			else if (key == 40)
				selected = (selected < count - 1) ? selected + 1 : 0;
			else if (key == 13)
				return selected;
		}
	}

  public:
	void ShowMenu() {
		while (true) {
			const char *opts[] = {"1. 📊 Онлайн-статистика (терминал)",
								  "2. 🧩 Конструктор LazySequence + операции",
								  "3. 🌊 Потоки ввода/вывода (Seq/CSV/JSON)",
								  "4. ♾️ Кардиналы и Ординалы",
								  "5. ⛓️ Цепочки Map → Where → Reduce",
								  "6. ⚡ Бенчмарк производительности",
								  "7. 🧪 Запустить автоматические тесты",
								  "0. 🚪 Выход"};
			int choice = SelectWithArrows("ЛАБОРАТОРНАЯ РАБОТА №4", opts, 8);
			switch (choice) {
			case 0:
				InteractiveOnlineStats();
				break;
			case 1:
				InteractiveLazySequence();
				break;
			case 2:
				InteractiveStreams();
				break;
			case 3:
				DemoCardinalOrdinal();
				break;
			case 4:
				DemoChains();
				break;
			case 5:
				RunBenchmark();
				break;
			case 6:
				RunAllTests();
				break;
			case 7:
				return;
			}
		}
	}

	void InteractiveOnlineStats() {
		ClearScreen();
		PrintHeader("📊 ОНЛАЙН-СТАТИСТИКА (режим терминала)");
		std::cout
			<< "  Вводите числа по одному. Статистика обновляется мгновенно.\n";
		std::cout << "  Введите 0 или пустую строку для завершения.\n";
		PrintSeparator();
		OnlineStatistics<double> stats;
		std::string line;
		while (true) {
			std::cout << "  ➤ Новое значение: ";
			std::getline(std::cin, line);
			size_t start = line.find_first_not_of(" \t\r\n");
			if (start == std::string::npos)
				break;
			line = line.substr(start);
			if (line == "0")
				break;
			try {
				double val = std::stod(line);
				stats.Add(val);
				std::cout << "  ✅ Принято. Элементов: " << std::setw(24)
						  << stats.GetCount() << "\n";
				std::cout << "  │ Минимум:   " << std::setw(24)
						  << stats.GetMin() << "\n";
				std::cout << "  │ Максимум:  " << std::setw(24)
						  << stats.GetMax() << "\n";
				std::cout << "  │ Среднее:   " << std::setw(24)
						  << stats.GetMean() << "\n";
				std::cout << "  │ Медиана:   " << std::setw(24)
						  << stats.GetMedian() << "\n";
				std::cout << "  └─────────────────────────────────────┘\n";
			} catch (...) {
				std::cout << "  ❌ Ошибка: введите корректное число.\n";
			}
		}
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	void InteractiveLazySequence() {
		const char *rules[] = {"1. Натуральные (1,2,3...)", "2. Факториалы",
							   "3. Фибоначчи", "0. Отмена"};
		int rule = SelectWithArrows("🧩 КОНСТРУКТОР ЛЕНИВОЙ ПОСЛЕДОВАТЕЛЬНОСТИ",
									rules, 4);
		if (rule == 3)
			return;

		int len = GetInput<int>("Длина (-1 для бесконечности): ", -1, 100000);
		LazySequence<int> *seq = nullptr;
		Cardinal card = (len == -1) ? Cardinal::Infinity() : Cardinal(len);
		if (rule == 0)
			seq = new LazySequence<int>(
				[](Sequence<int> *p) { return (int)p->GetLength() + 1; }, card);
		else if (rule == 1)
			seq = new LazySequence<int>(
				[](Sequence<int> *p) {
					int l = (int)p->GetLength();
					return (l == 0) ? 1 : (int)p->Get(l - 1) * l;
				},
				card);
		else
			seq = new LazySequence<int>(
				[](Sequence<int> *p) {
					int l = (int)p->GetLength();
					return (l < 2) ? 1
								   : (int)p->Get(l - 1) + (int)p->Get(l - 2);
				},
				card);

		while (true) {
			const char *ops[] = {"1. Get(idx)",	   "2. Map",	"3. Where",
								 "4. Reduce",	   "5. Concat", "6. InsertAt",
								 "7. Subsequence", "0. В меню"};
			int op = SelectWithArrows("🔧 ОПЕРАЦИИ", ops, 8);
			if (op == 7) {
				delete seq;
				return;
			}
			ClearScreen();
			try {
				if (op == 0) {
					int idx = GetInput<int>("Индекс: ", 0, 1000);
					std::cout << "\n✅ seq[" << idx << "] = " << seq->Get(idx)
							  << "\n";
				} else if (op == 1) {
					auto *mapped = seq->Map<int>([](int x) { return x * x; });
					std::cout << "\n✅ Map(x->x²) первые 5 элементов: ";
					for (int i = 0; i < 5 && (i < mapped->GetLength() ||
											  mapped->GetLength() == -1);
						 ++i)
						std::cout << mapped->Get(i) << " ";
					std::cout << "\n";
					auto *lazyMapped =
						dynamic_cast<LazySequence<int> *>(mapped);
					std::cout
						<< "Материализовано: "
						<< (lazyMapped ? lazyMapped->GetMaterializedCount() : 0)
						<< "\n";
					delete mapped;
				} else if (op == 2) {
					auto *filtered =
						seq->Where([](int x) { return x % 2 == 0; });
					std::cout << "\n✅ Where(чётные) первые 2: ";
					for (int i = 0; i < 2; ++i)
						std::cout << filtered->Get(i) << " ";
					std::cout << "\n";
					delete filtered;
				} else if (op == 3) {
					size_t limit = seq->GetCardinalLength().IsInfinite()
									   ? GetInput<size_t>(
											 "⚠️ Последовательность бесконечна. "
											 "Введите N элементов для Reduce: ",
											 1, 100000)
									   : (size_t)seq->GetLength();

					int sum = 0;
					for (size_t i = 0; i < limit; ++i)
						sum += seq->Get(i);
					std::cout << "\n✅ Reduce(сумма первых " << limit
							  << ") = " << sum << "\n";
				} else if (op == 4) {
					int addLen = GetInput<int>(
						"Длина добавляемой последовательности: ", 1, 10);
					auto *other = new LazySequence<int>(
						[](Sequence<int> *p) { return p->GetLength() + 100; },
						Cardinal(addLen));
					auto *concated = seq->Concat(*other);
					std::cout << "\n✅ Concat первые 3 элемента второй части: ";
					int baseLen = seq->GetLength();
					if (baseLen != -1) {
						for (int i = 0; i < 3; ++i)
							std::cout << concated->Get(baseLen + i) << " ";
					} else {
						std::cout << "(бесконечная последовательность)\n";
					}
					int len = concated->GetLength();
					std::cout << "\nИтоговая длина: "
							  << (len == -1 ? "∞" : std::to_string(len))
							  << "\n";
					delete concated;
					delete other;
				} else if (op == 5) {
					int idx = GetInput<int>("Позиция вставки: ", 0, 100);
					int val = GetInput<int>("Значение: ");
					auto *inserted = seq->InsertAt(val, idx);
					std::cout << "\n✅ Вставлено " << val << " в " << idx
							  << ". Элемент: " << inserted->Get(idx) << "\n";
					delete inserted;
				} else if (op == 6) {
					int s = GetInput<int>("Начало: ", 0, 50);
					int e = GetInput<int>("Конец: ", s, 50);
					auto *sub = seq->GetSubsequence(s, e);
					std::cout << "\n✅ Sub[" << s << ".." << e << "]: ";
					for (int i = 0; i < sub->GetLength(); ++i)
						std::cout << sub->Get(i) << " ";
					std::cout << "\n";
					delete sub;
				}
			} catch (const std::exception &ex) {
				std::cout << "\n❌ Ошибка: " << ex.what() << "\n";
			}
			WaitForEnter();
		}
	}

	void InteractiveStreams() {
		const char *src[] = {"1. Sequence Read", "2. Write to Seq",
							 "3. CSV Reader", "0. Отмена"};
		int c = SelectWithArrows("🌊 ПОТОКИ", src, 4);
		if (c == 3)
			return;
		try {
			if (c == 0) {
				int data[] = {42, 7, 19, 3, 88};
				auto *seq = new MutableArraySequence<int>(data, 5);
				auto *stream = new SequenceReadStream<int>(seq);
				stream->Open();
				std::cout << "\n📥 Поток: ";
				while (!stream->IsEndOfStream())
					std::cout << stream->Read() << " ";
				std::cout << "\n";
				stream->Close();
				delete stream;
				delete seq;
			} else if (c == 1) {
				auto *seq = new MutableArraySequence<int>();
				auto *w = new SequenceWriteStream<int>(seq);
				w->Open();
				for (int i = 1; i <= 3; ++i)
					w->Write(i * 10);
				w->Close();
				std::cout << "\n📤 Записано. Проверка: ";
				for (int i = 0; i < seq->GetLength(); ++i)
					std::cout << seq->Get(i) << " ";
				std::cout << "\n";
				delete w;
				delete seq;
			} else if (c == 2) {
				std::cout << "\n📂 CSV Reader требует путь к файлу (реализуйте "
							 "по аналогии с Sequence)\n";
			}
		} catch (const std::exception &ex) {
			std::cout << "\n❌ " << ex.what() << "\n";
		}
		WaitForEnter();
	}

	void DemoCardinalOrdinal() {
		ClearScreen();
		PrintHeader("♾️ КАРДИНАЛЫ И ОРДИНАЛЫ");
		int n = GetInput<int>("n: ", 0, 1000000);
		Cardinal c1(n), c2 = Cardinal::Infinity();
		std::cout << "\n"
				  << c1.ToString() << " + ∞ = " << (c1 + c2).ToString() << "\n";
		std::cout << "  " << c1.ToString() << " < ∞ : " << (c1 < c2) << "\n";
		OrdinalIndex o1(0, n), o2(1, 0);
		std::cout << "  ω·0 + " << n << " < ω·1 + 0 : " << (o1 < o2) << "\n";
		WaitForEnter();
	}
	void DemoChains() {
		ClearScreen();
		PrintHeader("⛓️ ЦЕПОЧКИ");
		auto *fib = new LazySequence<int>(
			[](Sequence<int> *p) {
				return (int)p->GetLength() < 2
						   ? 1
						   : (int)p->Get((int)p->GetLength() - 1) +
								 (int)p->Get((int)p->GetLength() - 2);
			},
			Cardinal::Infinity());
		auto *mapped = fib->Map<int>([](int x) { return x * 2; });
		auto *filtered = mapped->Where([](int x) { return x % 3 == 0; });
		std::cout << "  Фибоначчи -> x2 -> фильтр(÷3)\n";
		for (int i = 0; i < 4; ++i)
			std::cout << "  [" << i << "] = " << filtered->Get(i) << "\n";
		delete filtered;
		delete mapped;
		delete fib;
		WaitForEnter();
	}
	void RunBenchmark() {
		ClearScreen();
		PrintHeader("⚡ БЕНЧМАРК");
		int size = GetInput<int>("N: ", 1000, 500000);
		auto start = std::chrono::high_resolution_clock::now();
		auto *lazy = new LazySequence<int>(
			[](Sequence<int> *p) { return (int)p->GetLength() + 1; },
			Cardinal(size));
		volatile int dummy = lazy->Get(size - 1);
		(void)dummy;
		auto end = std::chrono::high_resolution_clock::now();
		auto ms =
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
				.count();
		std::cout << "  ⏱️  Lazy Get(" << size - 1 << "): " << ms << " мс\n";
		delete lazy;
		WaitForEnter();
	}
	void RunAllTests() {
		ClearScreen();
		PrintHeader("ТЕСТЫ");
		std::cout << "  📜 make clean && make test && ./test\n";
		WaitForEnter();
	}
};