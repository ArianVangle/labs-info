#pragma once
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#endif

#include "../lab3_base/array_sequence.h"
#include "../lab3_base/utils.h"
#include "algorithms.hpp"
#include "cardinal.hpp"
#include "lazy_sequence.hpp"
#include "read_only_stream.hpp"
#include "stream.hpp"
#include "stream_adapters.hpp"
#include "write_only_stream.hpp"

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
} // namespace Color

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
	static const int WIDTH = 100;

	void ClearScreen() {
#ifdef _WIN32
		system("cls");
#else
		std::cout << "\033[2J\033[H";
#endif
	}
	void SetColor(const std::string &c) { std::cout << c; }
	void ResetColor() { std::cout << Color::RESET; }
	void PrintChars(char c, int count) {
		for (int i = 0; i < count; i++)
			std::cout << c;
	}

	void PrintRowIndented(const std::string &text, const std::string &color) {
		SetColor(color);
		std::cout << "|   ";

		int maxContentWidth = WIDTH - 6;
		std::string displayText = text;
		int vLen = VisibleLength(text);

		if (vLen > maxContentWidth) {
			displayText = text.substr(0, maxContentWidth - 3) + "...";
			vLen = VisibleLength(displayText);
		}

		std::cout << displayText;
		int padding = std::max(0, WIDTH - 5 - vLen);
		PrintChars(' ', padding);
		std::cout << "|" << std::endl;
		ResetColor();
	}
	void PrintConcatVisualization(LazySequence<int> *s) {
		PrintRowIndented("--- СТРУКТУРА CONCAT ---", Color::BRIGHT_YELLOW);
		try {
			std::string b0 = "Block 0: ";
			int shown0 = 0;
			for (int i = 0; i < 10; i++) {
				try {
					b0 += std::to_string(s->Get(OrdinalIndex(0, i))) + ", ";
					shown0++;
				} catch (...) {
					break;
				}
			}
			if (shown0 == 10) {
				b0 += "...";
			}
			PrintRowIndented(b0, Color::BRIGHT_CYAN);

			PrintRowIndented("       ↓", Color::BRIGHT_BLACK);

			std::string b1 = "Block 1: ";
			int shown1 = 0;
			for (int i = 0; i < 10; i++) {
				try {
					b1 += std::to_string(s->Get(OrdinalIndex(1, i))) + ", ";
					shown1++;
				} catch (...) {
					break;
				}
			}
			if (shown1 == 10) {
				b1 += "...";
			}
			PrintRowIndented(b1, Color::BRIGHT_GREEN);
		} catch (...) {
			PrintRowIndented("Ошибка визуализации структуры",
							 Color::BRIGHT_RED);
		}
	}

	void PrintEmpty() {
		SetColor(Color::BRIGHT_BLACK);
		std::cout << "|";
		PrintChars(' ', WIDTH - 2);
		std::cout << "|" << std::endl;
		ResetColor();
	}
	void OpenBox() {
		SetColor(Color::BRIGHT_BLACK);
		std::cout << "+";
		PrintChars('-', WIDTH - 2);
		std::cout << "+" << std::endl;
		ResetColor();
	}
	void CloseBox() { OpenBox(); }

	void PrintHeader(const std::string &text) {
		std::cout << std::endl;
		SetColor(Color::BRIGHT_BLACK);
		PrintChars('=', WIDTH);
		std::cout << std::endl;
		ResetColor();

		int vLen = VisibleLength(text);
		int padding = std::max(0, (WIDTH - 2 - vLen) / 2);
		SetColor(Color::BRIGHT_CYAN);
		std::cout << "|";
		PrintChars(' ', padding);
		std::cout << text;
		PrintChars(' ', std::max(0, WIDTH - 2 - vLen - padding));
		std::cout << "|" << std::endl;

		SetColor(Color::BRIGHT_BLACK);
		PrintChars('=', WIDTH);
		std::cout << std::endl;
		ResetColor();
	}

	void PrintSection(const std::string &text) {
		std::cout << std::endl;
		SetColor(Color::BRIGHT_YELLOW);
		PrintChars('-', WIDTH);
		std::cout << std::endl;
		ResetColor();

		std::cout << "| ";
		int vLen = VisibleLength(text);
		int padding = std::max(0, WIDTH - 3 - vLen);
		SetColor(Color::BRIGHT_YELLOW);
		std::cout << text;
		PrintChars(' ', padding);
		std::cout << "|" << std::endl;

		SetColor(Color::BRIGHT_YELLOW);
		PrintChars('-', WIDTH);
		std::cout << std::endl;
		ResetColor();
	}

	void Sleep(int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
	void WaitForEnter() {
		std::cout << "\n  Нажмите Enter для возврата в меню...";
		std::cout.flush();
		while (ReadKey() != 13)
			;
	}
	void FlushInput() {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	int SelectFromMenu(const std::string &title,
					   Sequence<std::string> *options) {
		int selected = 0;
		int count = options->GetLength();
		while (true) {
			ClearScreen();
			PrintHeader(title);
			OpenBox();
			PrintEmpty();
			for (int i = 0; i < count; ++i) {
				std::string prefix = (i == selected) ? "▶ " : "  ";
				std::string text = prefix + options->Get(i);
				PrintRowIndented(text, (i == selected) ? Color::BRIGHT_WHITE
													   : Color::BRIGHT_CYAN);
			}
			PrintEmpty();
			CloseBox();
			std::cout << "\n  ↑↓ навигация, Enter - выбор" << std::endl;
			std::cout.flush();

			int key = ReadKey();
			if (key == 38)
				selected = (selected + count - 1) % count;
			else if (key == 40)
				selected = (selected + 1) % count;
			else if (key == 13)
				return selected;
		}
	}

	std::string GetLazyState(LazySequence<int> *s) const {
		if (!s)
			return "[null]";
		std::string r = "[";
		int len = s->GetLength();
		const int limit = 7;

		int show = (len == -1 || len > limit) ? limit : len;

		for (int i = 0; i < show; ++i) {
			if (i > 0)
				r += ", ";
			r += std::to_string(s->Get(i));
		}

		if (len == -1 || len > limit) {
			r += ", ...";
		}

		r += "] (len=" + std::to_string(len) +
			 ", mat=" + std::to_string(s->GetMaterializedCount()) + ")";
		return r;
	}

  public:
	ConsoleUI() {}

	void ShowMenu() {
		struct Item {
			int key;
			const char *text;
			const char *color;
		};
		Item items[] = {
			{1, "1. Онлайн-статистика (терминал)", Color::BRIGHT_GREEN.c_str()},
			{2, "2. Конструктор LazySequence + операции",
			 Color::BRIGHT_BLUE.c_str()},
			{3, "3. Потоки ввода/вывода (Seq/CSV/JSON)",
			 Color::BRIGHT_CYAN.c_str()},
			{4, "4. Кардиналы и Ординалы", Color::BRIGHT_YELLOW.c_str()},
			{5, "5. Цепочки Map -> Where -> Reduce",
			 Color::BRIGHT_MAGENTA.c_str()},
			{6, "6. Бенчмарк производительности", Color::BRIGHT_CYAN.c_str()},
			{7, "7. Запустить автоматические тесты",
			 Color::BRIGHT_WHITE.c_str()},
			{0, "0. Выход", Color::BRIGHT_RED.c_str()}};
		int count = sizeof(items) / sizeof(items[0]);
		int selected = 0;

		while (true) {
			ClearScreen();
			PrintHeader("ЛАБОРАТОРНАЯ РАБОТА №4");
			OpenBox();
			PrintEmpty();
			for (int i = 0; i < count; ++i) {
				std::string prefix = (i == selected) ? "▶ " : "  ";
				std::string line = prefix + items[i].text;
				PrintRowIndented(line, (i == selected) ? Color::BRIGHT_WHITE
													   : items[i].color);
			}
			PrintEmpty();
			CloseBox();
			std::cout << "\n  ↑↓ навигация, Enter - выбор" << std::endl;

			int key = ReadKey();
			if (key == 38)
				selected = (selected > 0) ? selected - 1 : count - 1;
			else if (key == 40)
				selected = (selected < count - 1) ? selected + 1 : 0;
			else if (key == 13) {
				int choice = items[selected].key;
				ClearScreen();
				switch (choice) {
				case 1:
					InteractiveOnlineStats();
					break;
				case 2:
					InteractiveLazySequence();
					break;
				case 3:
					InteractiveStreams();
					break;
				case 4:
					DemoCardinalOrdinal();
					break;
				case 5:
					DemoChains();
					break;
				case 6:
					RunBenchmark();
					break;
				case 7:
					RunAllTests();
					break;
				case 0:
					ClearScreen();
					PrintHeader("ЗАВЕРШЕНИЕ РАБОТЫ");
					PrintEmpty();
					PrintRowIndented("Спасибо за использование!",
									 Color::BRIGHT_GREEN);
					PrintEmpty();
					CloseBox();
					Sleep(1000);
					return;
				}
				WaitForEnter();
			}
		}
	}

	void InteractiveOnlineStats() {
		OpenBox();
		PrintSection("ОНЛАЙН-СТАТИСТИКА");
		PrintEmpty();
		PrintRowIndented("Вводите числа по одному. Для выхода: 0 или Enter",
						 Color::BRIGHT_YELLOW);
		PrintEmpty();
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
				PrintRowIndented("Принято. Элементов: " +
									 std::to_string((int)stats.GetCount()),
								 Color::BRIGHT_GREEN);
				PrintRowIndented("Min: " + std::to_string(stats.GetMin()),
								 Color::BRIGHT_CYAN);
				PrintRowIndented("Max: " + std::to_string(stats.GetMax()),
								 Color::BRIGHT_CYAN);
				std::ostringstream meanStr;
				meanStr << std::fixed << std::setprecision(2)
						<< stats.GetMean();
				PrintRowIndented("Mean: " + meanStr.str(), Color::BRIGHT_CYAN);
				std::ostringstream medStr;
				medStr << std::fixed << std::setprecision(2)
					   << stats.GetMedian();
				PrintRowIndented("Median: " + medStr.str(), Color::BRIGHT_CYAN);
				PrintEmpty();
			} catch (...) {
				PrintRowIndented("Ошибка: введите корректное число.",
								 Color::BRIGHT_RED);
			}
		}
		FlushInput();
		CloseBox();
	}

	void InteractiveLazySequence() {
		Sequence<std::string> *rules = new MutableArraySequence<std::string>();
		rules->Append("1. Натуральные (1,2,3...)");
		rules->Append("2. Факториалы");
		rules->Append("3. Фибоначчи");
		rules->Append("0. Отмена");

		int ruleIdx = SelectFromMenu("ВЫБОР ПОРОЖДАЮЩЕГО ПРАВИЛА", rules);
		delete rules;
		if (ruleIdx == 3)
			return;

		OpenBox();
		PrintSection("ПАРАМЕТРЫ ПОСЛЕДОВАТЕЛЬНОСТИ");
		PrintEmpty();
		int len = -1;
		std::cout << "  Введите длину (-1 для бесконечности): ";
		std::cin >> len;
		FlushInput();
		CloseBox();

		LazySequence<int> *seq = nullptr;
		Cardinal card = (len == -1) ? Cardinal::Infinity() : Cardinal(len);
		if (ruleIdx == 0)
			seq = new LazySequence<int>(
				[](Sequence<int> *p) { return (int)p->GetLength() + 1; }, card);
		else if (ruleIdx == 1)
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
			Sequence<std::string> *ops =
				new MutableArraySequence<std::string>();
			ops->Append("1. Get(idx)");
			ops->Append("2. Map (x^2)");
			ops->Append("3. Where (чётные)");
			ops->Append("4. Reduce (сумма)");
			ops->Append("5. Concat (с визуализацией)");
			ops->Append("6. InsertAt");
			ops->Append("7. Subsequence");
			ops->Append("0. В меню");

			int opIdx =
				SelectFromMenu("ОПЕРАЦИИ НАД ЛЕНИВОЙ ПОСЛЕДОВАТЕЛЬНОСТЬЮ", ops);
			int menuSize = ops->GetLength();
			delete ops;
			if (opIdx == menuSize - 1) {
				delete seq;
				return;
			}

			OpenBox();
			PrintSection("РЕЗУЛЬТАТ ОПЕРАЦИИ");
			PrintEmpty();
			PrintRowIndented("Исходное состояние: " + GetLazyState(seq),
							 Color::BRIGHT_BLUE);
			PrintEmpty();

			try {
				if (opIdx == 0) {
					int idx = 0;
					std::cout << "  Индекс: ";
					std::cin >> idx;
					FlushInput();
					PrintRowIndented("seq[" + std::to_string(idx) +
										 "] = " + std::to_string(seq->Get(idx)),
									 Color::BRIGHT_GREEN);
				} else if (opIdx == 1) {
					auto *mapped = seq->Map<int>([](int x) { return x * x; });
					try {
						std::string nums = "";
						for (int i = 0; i < 5 && (i < mapped->GetLength() ||
												  mapped->GetLength() == -1);
							 ++i) {
							if (i > 0)
								nums += " ";
							nums += std::to_string(mapped->Get(i));
						}
						PrintRowIndented("Map(x→x²) первые 5: " + nums,
										 Color::BRIGHT_MAGENTA);
					} catch (const std::exception &e) {
						PrintRowIndented("Ошибка доступа: " +
											 std::string(e.what()),
										 Color::BRIGHT_RED);
					}
					delete mapped;
				} else if (opIdx == 2) {
					auto *filtered =
						seq->Where([](int x) { return x % 2 == 0; });
					try {
						std::string nums = "";
						for (int i = 0; i < 3; ++i) {
							if (i > 0)
								nums += " ";
							nums += std::to_string(filtered->Get(i));
						}
						PrintRowIndented("Where(чётные) первые 3: " + nums,
										 Color::BRIGHT_MAGENTA);
					} catch (const std::exception &e) {
						PrintRowIndented("Ошибка доступа: " +
											 std::string(e.what()),
										 Color::BRIGHT_RED);
					}
					delete filtered;
				} else if (opIdx == 3) {
					size_t limit = seq->GetCardinalLength().IsInfinite()
									   ? 100000
									   : (size_t)seq->GetLength();
					if (seq->GetCardinalLength().IsInfinite()) {
						std::cout << "  Последовательность бесконечна. Введите "
									 "N для Reduce: ";
						std::cin >> limit;
						FlushInput();
					}
					int sum = 0;
					for (size_t i = 0; i < limit; ++i)
						sum += seq->Get(i);
					PrintRowIndented("Reduce(сумма первых " +
										 std::to_string(limit) +
										 ") = " + std::to_string(sum),
									 Color::BRIGHT_GREEN);
				}

				else if (opIdx == 4) {
					int addLen = 5;
					std::cout << "  Длина добавляемой: ";
					std::cin >> addLen;
					FlushInput();
					auto *other = new LazySequence<int>(
						[](Sequence<int> *p) { return p->GetLength() + 100; },
						Cardinal(addLen));
					auto *concated = seq->Concat(*other);
					try {
						PrintRowIndented(
							"Concat состояние: " +
								GetLazyState(dynamic_cast<LazySequence<int> *>(
									concated)),
							Color::BRIGHT_CYAN);
						PrintEmpty();
						PrintRowIndented("Визуализация блоков:",
										 Color::BRIGHT_YELLOW);
						PrintConcatVisualization(
							dynamic_cast<LazySequence<int> *>(concated));
					} catch (const std::exception &e) {
						PrintRowIndented("Ошибка доступа: " +
											 std::string(e.what()),
										 Color::BRIGHT_RED);
					}
					delete concated;
					delete other;
				} else if (opIdx == 5) {
					int idx = 0, val = 0;
					std::cout << "  Позиция и значение: ";
					std::cin >> idx >> val;
					FlushInput();
					auto *inserted = seq->InsertAt(val, idx);
					try {
						PrintRowIndented(
							"InsertAt состояние: " +
								GetLazyState(dynamic_cast<LazySequence<int> *>(
									inserted)),
							Color::BRIGHT_CYAN);
					} catch (const std::exception &e) {
						PrintRowIndented("Ошибка доступа: " +
											 std::string(e.what()),
										 Color::BRIGHT_RED);
					}
					delete inserted;
				} else if (opIdx == 6) {
					int s = 0, e = 5;
					std::cout << "  Начало и конец: ";
					std::cin >> s >> e;
					FlushInput();
					auto *sub = seq->GetSubsequence(s, e);
					try {
						PrintRowIndented("Sub[" + std::to_string(s) + ".." +
											 std::to_string(e) +
											 "]: " + GetLazyState(sub),
										 Color::BRIGHT_CYAN);
					} catch (const std::exception &e) {
						PrintRowIndented("Ошибка доступа: " +
											 std::string(e.what()),
										 Color::BRIGHT_RED);
					}
					delete sub;
				}
			} catch (const std::exception &ex) {
				PrintRowIndented("Критическая ошибка: " +
									 std::string(ex.what()),
								 Color::BRIGHT_RED);
			}
			CloseBox();
			WaitForEnter();
		}
	}

	void InteractiveStreams() {
		Sequence<std::string> *src = new MutableArraySequence<std::string>();
		src->Append("1. Sequence Read");
		src->Append("2. CSV Reader");
		src->Append("3. Write to Seq");
		src->Append("0. Отмена");
		int c = SelectFromMenu("ВЫБОР ТИПА ПОТОКА", src);
		delete src;
		if (c == 3)
			return;

		OpenBox();
		PrintSection("ОБРАБОТКА ПОТОКА");
		PrintEmpty();
		try {
			if (c == 0) {
				int data[] = {42, 7, 19, 3, 88};
				auto *seq = new MutableArraySequence<int>(data, 5);
				auto *stream = new SequenceReadStream<int>(seq);
				stream->Open();
				PrintRowIndented("Чтение Sequence потока:", Color::BRIGHT_BLUE);
				std::string line = "[";
				while (!stream->IsEndOfStream())
					line += std::to_string(stream->Read()) + ", ";
				if (line.size() > 1) {
					line.pop_back();
					line.pop_back();
				}
				line += "]";
				PrintRowIndented(line, Color::BRIGHT_GREEN);
				stream->Close();
				delete stream;
				delete seq;
			} else if (c == 1) {
				std::string csvFile = "lab4_demo.csv";
				{
					std::ofstream out(csvFile);
					out << "10,apple,3.14\n20,banana,2.71\n30,cherry,1.41\n";
					out.close();
				}
				PrintRowIndented("Создан демо-файл: " + csvFile,
								 Color::BRIGHT_YELLOW);
				auto parser = [](Sequence<std::string> *fields) -> int {
					if (fields->GetLength() > 0)
						try {
							return std::stoi(fields->Get(0));
						} catch (...) {
							return 0;
						}
					return 0;
				};
				auto *fileStream = new FileStream(
					csvFile, [](const std::string &s) { return s; });
				auto *csvStream =
					new CsvReadStream<int>(fileStream, parser, ',');
				csvStream->Open();
				PrintRowIndented("Чтение CSV (колонка 1 -> int):",
								 Color::BRIGHT_BLUE);
				std::string res = "[";
				while (!csvStream->IsEndOfStream())
					res += std::to_string(csvStream->Read()) + ", ";
				if (res.size() > 1) {
					res.pop_back();
					res.pop_back();
				}
				res += "]";
				PrintRowIndented(res, Color::BRIGHT_GREEN);
				csvStream->Close();
				delete csvStream;
				delete fileStream;
				std::remove(csvFile.c_str());
				PrintRowIndented("Временный файл удалён.", Color::BRIGHT_BLACK);
			} else if (c == 2) {
				auto *seq = new MutableArraySequence<int>();
				auto *w = new SequenceWriteStream<int>(seq);
				w->Open();
				for (int i = 1; i <= 3; ++i)
					w->Write(i * 10);
				w->Close();
				PrintRowIndented("Записано в Sequence. Проверка:",
								 Color::BRIGHT_BLUE);
				std::string r = "[";
				for (int i = 0; i < seq->GetLength(); ++i)
					r += std::to_string(seq->Get(i)) + ", ";
				if (r.size() > 1) {
					r.pop_back();
					r.pop_back();
				}
				r += "]";
				PrintRowIndented(r, Color::BRIGHT_GREEN);
				delete w;
				delete seq;
			}
		} catch (const std::exception &ex) {
			PrintRowIndented("Ошибка: " + std::string(ex.what()),
							 Color::BRIGHT_RED);
		}
		CloseBox();
	}

	void DemoCardinalOrdinal() {
		OpenBox();
		PrintSection("КАРДИНАЛЫ И ОРДИНАЛЫ");
		PrintEmpty();
		int n = 0;
		std::cout << "  Введите n: ";
		std::cin >> n;
		FlushInput();
		Cardinal c1(n), c2 = Cardinal::Infinity();
		PrintRowIndented(c1.ToString() + " + ∞ = " + (c1 + c2).ToString(),
						 Color::BRIGHT_CYAN);
		PrintRowIndented(c1.ToString() + " < ∞ : " + std::to_string(c1 < c2),
						 Color::BRIGHT_CYAN);
		OrdinalIndex o1(0, n), o2(1, 0);
		PrintRowIndented("ω·0 + " + std::to_string(n) +
							 " < ω·1 + 0 : " + std::to_string(o1 < o2),
						 Color::BRIGHT_YELLOW);
		CloseBox();
	}

	void DemoChains() {
		OpenBox();
		PrintSection("ЦЕПОЧКИ MAP -> WHERE -> REDUCE");
		PrintEmpty();
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
		PrintRowIndented("Фибоначчи -> x2 -> фильтр(÷3)", Color::BRIGHT_BLUE);
		for (int i = 0; i < 4; ++i)
			PrintRowIndented("  [" + std::to_string(i) +
								 "] = " + std::to_string(filtered->Get(i)),
							 Color::BRIGHT_GREEN);
		delete filtered;
		delete mapped;
		delete fib;
		CloseBox();
	}

	void RunBenchmark() {
		OpenBox();
		PrintSection("БЕНЧМАРК ПРОИЗВОДИТЕЛЬНОСТИ");
		PrintEmpty();
		int size = 0;
		std::cout << "  Введите N: ";
		std::cin >> size;
		FlushInput();
		PrintRowIndented("Тест: LazySequence (Фибоначчи)", Color::BRIGHT_BLUE);
		auto *fibLazy = new LazySequence<int>(
			[](Sequence<int> *p) {
				int l = (int)p->GetLength();
				return (l < 2) ? 1 : (int)p->Get(l - 1) + (int)p->Get(l - 2);
			},
			Cardinal(size));
		auto t0 = std::chrono::high_resolution_clock::now();
		auto t1 = std::chrono::high_resolution_clock::now();
		auto createUs =
			std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0)
				.count();

		t0 = std::chrono::high_resolution_clock::now();
		volatile int val1 = fibLazy->Get(size - 1);
		(void)val1;
		t1 = std::chrono::high_resolution_clock::now();
		auto firstUs =
			std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0)
				.count();

		t0 = std::chrono::high_resolution_clock::now();
		volatile int val2 = fibLazy->Get(size - 1);
		(void)val2;
		t1 = std::chrono::high_resolution_clock::now();
		auto memoUs =
			std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0)
				.count();

		PrintRowIndented("Создание: " + std::to_string(createUs) + " мкс",
						 Color::BRIGHT_GREEN);
		PrintRowIndented("Первый Get(" + std::to_string(size - 1) +
							 "): " + std::to_string(firstUs) + " мкс",
						 Color::BRIGHT_CYAN);
		PrintRowIndented("Мемоизированный Get: " + std::to_string(memoUs) +
							 " мкс",
						 Color::BRIGHT_YELLOW);
		PrintRowIndented("Материализовано: " +
							 std::to_string(fibLazy->GetMaterializedCount()) +
							 " / " + std::to_string(size),
						 Color::BRIGHT_BLACK);
		delete fibLazy;
		CloseBox();
	}

	void RunAllTests() {
		OpenBox();
		PrintSection("АВТОМАТИЧЕСКИЕ ТЕСТЫ");
		PrintEmpty();
		PrintRowIndented("Для запуска тестов выполните в терминале:",
						 Color::BRIGHT_YELLOW);
		PrintRowIndented("  make clean && make test && ./test",
						 Color::BRIGHT_GREEN);
		CloseBox();
	}
};