#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
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
	void ClearScreen() {
#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif
	}
	void SetColor(const std::string &c) { std::cout << c; }
	void ResetColor() { std::cout << Color::RESET; }
	void PrintChars(char c, int count) {
		for (int i = 0; i < count; i++)
			std::cout << c;
	}
	void PrintRow(const std::string &text, const std::string &color) {
		SetColor(color);
		std::cout << "| ";
		std::cout << text;
		int padding = WIDTH - 3 - VisibleLength(text);
		PrintChars(' ', padding);
		std::cout << "|" << std::endl;
		ResetColor();
	}
	void PrintRowIndented(const std::string &text, const std::string &color) {
		SetColor(color);
		std::cout << "|   ";
		std::cout << text;
		int padding = WIDTH - 5 - VisibleLength(text);
		PrintChars(' ', padding);
		std::cout << "|" << std::endl;
		ResetColor();
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
	void PrintLine(char c) {
		SetColor(Color::BRIGHT_BLACK);
		PrintChars(c, WIDTH);
		std::cout << std::endl;
		ResetColor();
	}
	void PrintHeader(const std::string &text) {
		std::cout << std::endl;
		PrintLine('=');
		int visibleLen = VisibleLength(text);
		int padding = (WIDTH - 2 - visibleLen) / 2;
		SetColor(Color::BRIGHT_CYAN);
		std::cout << "|";
		PrintChars(' ', padding);
		std::cout << text;
		PrintChars(' ', WIDTH - 2 - visibleLen - padding);
		std::cout << "|" << std::endl;
		PrintLine('=');
		ResetColor();
	}
	void PrintSection(const std::string &text) {
		std::cout << std::endl;
		SetColor(Color::BRIGHT_YELLOW);
		PrintLine('-');
		std::cout << "| ";
		std::cout << text;
		int visibleLen = VisibleLength(text);
		int padding = WIDTH - 3 - visibleLen;
		PrintChars(' ', padding);
		std::cout << "|" << std::endl;
		PrintLine('-');
		ResetColor();
	}
	void Sleep(int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	std::string GetSeqState(Sequence<int> *s) const {
		if (!s)
			return "[null]";
		std::string r = "[";
		for (int i = 0; i < s->GetLength(); ++i) {
			if (i > 0)
				r += ", ";
			r += std::to_string(s->Get(i));
		}
		return r + "]";
	}
	std::string GetDAState(const DynamicArray<int> &da) const {
		std::string r = "[";
		for (int i = 0; i < da.GetSize(); ++i) {
			if (i > 0)
				r += ", ";
			r += std::to_string(da.Get(i));
		}
		return r + "]";
	}
	std::string GetLLState(const LinkedList<int> &ll) const {
		std::string r = "[";
		for (int i = 0; i < ll.GetLength(); ++i) {
			if (i > 0)
				r += ", ";
			r += std::to_string(ll.Get(i));
		}
		return r + "]";
	}
	void WaitForEnter() {
		std::cout << "\n  Нажмите Enter для возврата в меню...";
		std::cout.flush();
		while (ReadKey() != 13)
			;
	}

  public:
	ConsoleUI() {}

	void ShowWelcome() {
		ClearScreen();
		std::cout << std::endl;
		PrintLine('#');
		PrintEmpty();
		std::string title = "ЛАБОРАТОРНАЯ РАБОТА №2";
		int visibleTitle = VisibleLength(title);
		int pad1 = (WIDTH - 2 - visibleTitle) / 2;
		SetColor(Color::BRIGHT_CYAN);
		std::cout << "|";
		PrintChars(' ', pad1);
		SetColor(Color::BRIGHT_WHITE);
		std::cout << title;
		SetColor(Color::BRIGHT_CYAN);
		PrintChars(' ', WIDTH - 2 - visibleTitle - pad1);
		std::cout << "|" << std::endl;
		std::string sub = "Линейные структуры данных";
		int visibleSub = VisibleLength(sub);
		int pad2 = (WIDTH - 2 - visibleSub) / 2;
		SetColor(Color::BRIGHT_CYAN);
		std::cout << "|";
		PrintChars(' ', pad2);
		SetColor(Color::BRIGHT_BLACK);
		std::cout << sub;
		SetColor(Color::BRIGHT_CYAN);
		PrintChars(' ', WIDTH - 2 - visibleSub - pad2);
		std::cout << "|" << std::endl;
		PrintEmpty();
		PrintLine('#');
		ResetColor();
		std::cout << std::endl;
		SetColor(Color::BRIGHT_BLACK);
		std::cout << "  Нажмите Enter для продолжения...";
		ResetColor();
		while (ReadKey() != 13)
			;
	}

	void ShowMenu() {
		struct MenuItem {
			int key;
			std::string text;
			std::string color;
		};
		MenuItem items[] = {
			{1, "1. Запустить все тесты", Color::BRIGHT_GREEN},
			{2, "2. Тестировать DynamicArray", Color::BRIGHT_BLUE},
			{3, "3. Тестировать LinkedList", Color::BRIGHT_BLUE},
			{4, "4. Тестировать Sequence (Mutable)", Color::BRIGHT_MAGENTA},
			{5, "5. Тестировать Sequence (Immutable)", Color::BRIGHT_MAGENTA},
			{6, "6. Тестировать Map/Reduce/Where", Color::BRIGHT_YELLOW},
			{7, "7. Тестировать From/Concat/Zip/Split", Color::BRIGHT_CYAN},
			{8, "8. Тестировать Итераторы", Color::BRIGHT_CYAN},
			{9, "9. Тестировать Option<T>", Color::BRIGHT_CYAN},
			{0, "0. Выход", Color::BRIGHT_RED}};
		size_t selected = 0;
		int count = sizeof(items) / sizeof(items[0]);

		while (true) {
			ClearScreen();
			PrintHeader("ГЛАВНОЕ МЕНЮ");
			OpenBox();
			PrintEmpty();
			for (size_t i = 0; i < (size_t)count; i++) {
				std::string prefix = (i == selected) ? "▶ " : "  ";
				std::string clr =
					(i == selected) ? Color::BRIGHT_WHITE : items[i].color;
				PrintRowIndented(prefix + items[i].text, clr);
			}
			PrintEmpty();
			CloseBox();
			std::cout << "\n  Используйте ↑↓ для навигации, Enter для выбора\n";
			int key = ReadKey();
			if (key == 38)
				selected = (selected + count - 1) % count;
			else if (key == 40)
				selected = (selected + 1) % count;
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
				case 2:
					PrintHeader("ТЕСТИРОВАНИЕ DYNAMICARRAY");
					TestDynamicArray();
					break;
				case 3:
					PrintHeader("ТЕСТИРОВАНИЕ LINKEDLIST");
					TestLinkedList();
					break;
				case 4:
					PrintHeader("ТЕСТИРОВАНИЕ SEQUENCE (MUTABLE)");
					TestMutableSequence();
					break;
				case 5:
					PrintHeader("ТЕСТИРОВАНИЕ SEQUENCE (IMMUTABLE)");
					TestImmutableSequence();
					break;
				case 6:
					PrintHeader("ТЕСТИРОВАНИЕ MAP/REDUCE/WHERE");
					TestMapReduce();
					break;
				case 7:
					PrintHeader("FROM/CONCAT/ZIP/SPLIT");
					TestFromConcat();
					TestZipUnzip();
					TestSplit();
					TestSlice();
					break;
				case 8:
					PrintHeader("ТЕСТИРОВАНИЕ ИТЕРАТОРОВ");
					TestIterators();
					break;
				case 9:
					PrintHeader("ТЕСТИРОВАНИЕ OPTION<T>");
					TestOption();
					break;
				case 0:
					ClearScreen();
					PrintHeader("ЗАВЕРШЕНИЕ РАБОТЫ");
					SetColor(Color::BRIGHT_GREEN);
					ResetColor();
					Sleep(1500);
					return;
				}
				WaitForEnter();
			}
		}
	}

	void TestSplit() {
		OpenBox();
		PrintSection("SPLIT");
		PrintEmpty();
		int data[] = {1, 2, 0, 3, 4, 0, 5};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 7);
		PrintRowIndented("-> Исходный", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("-> Split(разделитель: 0)", Color::BRIGHT_MAGENTA);

		auto *splitted = Split(*seq, [](int x) { return x == 0; });
		PrintRowIndented("   Фрагментов: " +
							 std::to_string(splitted->GetLength()),
						 Color::BRIGHT_CYAN);
		for (int i = 0; i < splitted->GetLength(); ++i) {
			PrintRowIndented("   Фрагмент [" + std::to_string(i) +
								 "]: " + GetSeqState(splitted->Get(i)),
							 Color::BRIGHT_GREEN);
		}

		for (int i = 0; i < splitted->GetLength(); ++i)
			delete splitted->Get(i);
		delete splitted;
		delete seq;
		CloseBox();
	}

	void TestZipUnzip() {
		OpenBox();
		PrintSection("ZIP/UNZIP");
		PrintEmpty();
		int data1[] = {1, 2, 3};
		int data2[] = {10, 20, 30};
		Sequence<int> *seq1 = new MutableArraySequence<int>(data1, 3);
		Sequence<int> *seq2 = new MutableArraySequence<int>(data2, 3);
		PrintRowIndented("-> Seq1", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq1),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("-> Seq2", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq2),
						 Color::BRIGHT_GREEN);

		auto *zipped = Zip(*seq1, *seq2);
		PrintRowIndented("-> Zip Result", Color::BRIGHT_MAGENTA);
		for (int i = 0; i < zipped->GetLength(); ++i) {
			auto pair = zipped->Get(i);
			PrintRowIndented("   Пара [" + std::to_string(i) + "]: (" +
								 std::to_string(pair.item1) + ", " +
								 std::to_string(pair.item2) + ")",
							 Color::BRIGHT_GREEN);
		}
		delete zipped;
		delete seq1;
		delete seq2;
		CloseBox();
	}

	void TestSlice() {
		OpenBox();
		PrintSection("SLICE");
		PrintEmpty();
		int data[] = {1, 2, 3, 4, 5};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 5);
		int insertData[] = {9, 10};
		Sequence<int> *insertSeq = new MutableArraySequence<int>(insertData, 2);
		PrintRowIndented("-> Исходный", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("-> Slice(1, 2, [9, 10])", Color::BRIGHT_MAGENTA);

		Sequence<int> *sliced = Slice(*seq, 1, 2, insertSeq);
		PrintRowIndented("   Результат: " + GetSeqState(sliced),
						 Color::BRIGHT_GREEN);
		delete sliced;
		delete seq;
		delete insertSeq;
		CloseBox();
	}

	void TestDynamicArray() {
		int data[] = {10, 20, 30, 40, 50};
		DynamicArray<int> da(data, 5);
		OpenBox();
		PrintSection("DYNAMIC ARRAY");
		PrintEmpty();
		PrintRowIndented("-> Инициализация", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetDAState(da),
						 Color::BRIGHT_GREEN);

		da.Set(2, 999);
		PrintRowIndented("-> Set(2, 999)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetDAState(da),
						 Color::BRIGHT_GREEN);

		da.Resize(7);
		PrintRowIndented("-> Resize(7)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Размер: " + std::to_string(da.GetSize()),
						 Color::BRIGHT_CYAN);
		CloseBox();
	}

	void TestLinkedList() {
		LinkedList<int> ll;
		OpenBox();
		PrintSection("LINKED LIST");
		PrintEmpty();
		PrintRowIndented("-> Инициализация", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetLLState(ll),
						 Color::BRIGHT_GREEN);

		ll.Append(100);
		ll.Append(200);
		ll.Append(300);
		PrintRowIndented("-> Append(100, 200, 300)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetLLState(ll),
						 Color::BRIGHT_GREEN);

		ll.Prepend(50);
		PrintRowIndented("-> Prepend(50)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetLLState(ll),
						 Color::BRIGHT_GREEN);

		ll.InsertAt(150, 2);
		PrintRowIndented("-> InsertAt(150, 2)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetLLState(ll),
						 Color::BRIGHT_GREEN);
		CloseBox();
	}

	void TestMutableSequence() {
		int data[] = {1, 2, 3};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 3);
		OpenBox();
		PrintSection("MUTABLE SEQUENCE");
		PrintEmpty();
		PrintRowIndented("-> Инициализация", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);

		seq->Append(4);
		PrintRowIndented("-> Append(4)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);
		seq->Prepend(0);
		PrintRowIndented("-> Prepend(0)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);
		seq->Set(1, 999);
		PrintRowIndented("-> Set(1, 999)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);
		CloseBox();
		delete seq;
	}

	void TestImmutableSequence() {
		int data[] = {1, 2, 3};
		Sequence<int> *seq = new ImmutableArraySequence<int>(data, 3);
		OpenBox();
		PrintSection("IMMUTABLE SEQUENCE");
		PrintEmpty();
		PrintRowIndented("-> Оригинал", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);

		Sequence<int> *newSeq = seq->Append(4);
		PrintRowIndented("-> Append(4)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Оригинал (неизменен): " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("   Новый объект: " + GetSeqState(newSeq),
						 Color::BRIGHT_CYAN);
		CloseBox();
		delete seq;
		delete newSeq;
	}

	void TestMapReduce() {
		int data[] = {1, 2, 3, 4, 5};
		MutableArraySequence<int> *seq = new MutableArraySequence<int>(data, 5);
		OpenBox();
		PrintSection("MAP-REDUCE");
		PrintEmpty();
		PrintRowIndented("-> Исходный", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);

		Sequence<int> *mapped = seq->Map<int>([](int x) { return x * 2; });
		PrintRowIndented("-> Map(x*2)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Результат: " + GetSeqState(mapped),
						 Color::BRIGHT_GREEN);
		delete mapped;

		Sequence<int> *filtered = seq->Where([](int x) { return x > 2; });
		PrintRowIndented("-> Where(x>2)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Результат: " + GetSeqState(filtered),
						 Color::BRIGHT_GREEN);
		delete filtered;

		int sum = seq->Reduce([](int acc, int x) { return acc + x; }, 0);
		PrintRowIndented("-> Reduce(сумма): " + std::to_string(sum),
						 Color::BRIGHT_CYAN);
		CloseBox();
		delete seq;
	}

	void TestIterators() {
		int data[] = {10, 20, 30, 40, 50};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 5);
		OpenBox();
		PrintSection("ИТЕРАТОРЫ");
		PrintEmpty();
		PrintRowIndented("-> Исходный", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);

		IEnumerator<int> *en = seq->GetEnumerator();
		std::string elements = "";
		bool first = true;
		while (en->MoveNext()) {
			if (!first)
				elements += ", ";
			elements += std::to_string(en->Current());
			first = false;
		}
		PrintRowIndented("-> Обход через IEnumerator", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Пройдено: [" + elements + "]",
						 Color::BRIGHT_GREEN);
		delete en;
		delete seq;
		CloseBox();
	}

	void TestOption() {
		int data[] = {1, 2, 3};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 3);
		OpenBox();
		PrintSection("OPTION<T>");
		PrintEmpty();
		PrintRowIndented("-> Исходный", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq),
						 Color::BRIGHT_GREEN);

		Option<int> opt1 = seq->TryGet(0);
		PrintRowIndented("-> TryGet(0)", Color::BRIGHT_MAGENTA);
		if (opt1.IsSome())
			PrintRowIndented("   IsSome=true, Value=" +
								 std::to_string(opt1.GetValue()),
							 Color::BRIGHT_GREEN);

		Option<int> opt2 = seq->TryGet(100);
		PrintRowIndented("-> TryGet(100)", Color::BRIGHT_MAGENTA);
		if (opt2.IsNone())
			PrintRowIndented("   IsNone=true (без исключения)",
							 Color::BRIGHT_GREEN);
		CloseBox();
		delete seq;
	}

	void TestFromConcat() {
		OpenBox();
		PrintSection("FROM / CONCAT");
		PrintEmpty();
		int data1[] = {1, 2, 3};
		Sequence<int> *seq1 = From(data1, 3);
		PrintRowIndented("-> From([1,2,3])", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq1),
						 Color::BRIGHT_GREEN);

		int data2[] = {4, 5, 6};
		Sequence<int> *seq2 = From(data2, 3);
		PrintRowIndented("-> From([4,5,6])", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(seq2),
						 Color::BRIGHT_GREEN);

		Sequence<int> *concatenated = Concat(*seq1, *seq2);
		PrintRowIndented("-> Concat", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Результат: " + GetSeqState(concatenated),
						 Color::BRIGHT_GREEN);
		delete seq1;
		delete seq2;
		delete concatenated;
		CloseBox();
	}
};