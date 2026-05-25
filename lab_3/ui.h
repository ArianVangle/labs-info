#pragma once
#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#endif

#include "array_sequence.h"
#include "deque_segmented.h"
#include "hanoi.h"
#include "hanoi_renderer.h"
#include "ring_renderer.h"
#include "list_sequence.h"
#include "map_reduce.h"
#include "ring.h"
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
				if (i == selected)
					PrintRowIndented(text, Color::BRIGHT_WHITE);
				else
					PrintRowIndented(text, Color::BRIGHT_CYAN);
			}
			PrintEmpty();
			CloseBox();
			std::cout << "\n  ↑↓ навигация, Enter - выбор\n";
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

	void WaitForEnter() {
		std::cout << "\n  Нажмите Enter для возврата в меню...";
		std::cout.flush();
		while (ReadKey() != 13)
			;
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
	std::string GetDequeState(const DequeSegmented<int> &d) const {
		std::string r = "[";
		for (int i = 0; i < d.GetLength(); ++i) {
			if (i > 0)
				r += ", ";
			r += std::to_string(d.Get(i));
		}
		return r + "]";
	}

  public:
	ConsoleUI() {}

	void ShowWelcome() {
		ClearScreen();
		std::cout << std::endl;
		PrintLine('#');
		PrintEmpty();
		std::string title = "ЛАБОРАТОРНАЯ РАБОТА №3";
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
		struct Item {
			int key;
			const char *text;
			const char *color;
		};
		Item items[] = {
			{1, "1. Запустить все тесты", Color::BRIGHT_GREEN.c_str()},
			{2, "2. Тестировать DynamicArray", Color::BRIGHT_BLUE.c_str()},
			{3, "3. Тестировать LinkedList", Color::BRIGHT_BLUE.c_str()},
			{4, "4. Тестировать Sequence (Mutable)",
			 Color::BRIGHT_MAGENTA.c_str()},
			{5, "5. Тестировать Sequence (Immutable)",
			 Color::BRIGHT_MAGENTA.c_str()},
			{6, "6. Тестировать Map/Reduce/Where",
			 Color::BRIGHT_YELLOW.c_str()},
			{7, "7. Тестировать From/Concat/Zip/Split",
			 Color::BRIGHT_CYAN.c_str()},
			{8, "8. Тестировать Итераторы", Color::BRIGHT_CYAN.c_str()},
			{9, "9. Тестировать Option<T>", Color::BRIGHT_CYAN.c_str()},
			{10, "10. Тестировать DequeSegmented",
			 Color::BRIGHT_MAGENTA.c_str()},
			{11, "11. Тестировать Ханойскую башню",
			 Color::BRIGHT_MAGENTA.c_str()},
			{0, "0. Выход", Color::BRIGHT_RED.c_str()}};
		int count = sizeof(items) / sizeof(items[0]);
		int selected = 0;

		while (true) {
			ClearScreen();
			PrintHeader("ГЛАВНОЕ МЕНЮ");
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
			std::cout << "\n  ↑↓ навигация, Enter - выбор\n";

			int key = ReadKey();
			if (key == 38)
				selected = (selected > 0) ? selected - 1 : count - 1;
			else if (key == 40)
				selected = (selected < count - 1) ? selected + 1 : 0;
			else if (key == 13) {
				int choice = items[selected].key;
				ClearScreen();
				switch (choice) {
				case 1: {
					PrintHeader("ЗАПУСК ВСЕХ ТЕСТОВ");
					Lab3TestRunner runner(this);
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
				case 10:
					PrintHeader("ТЕСТИРОВАНИЕ DEQUE SEGMENTED");
					TestDequeSegmented();
					break;
				case 11:
					PrintHeader("ТЕСТИРОВАНИЕ ХАНОЙСКОЙ БАШНИ");
					TestHanoiTower();
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

		auto *splitted = Split(*seq, [](int x) { return x == 0; });
		PrintRowIndented("-> Split(разделитель: 0)", Color::BRIGHT_MAGENTA);
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
		Sequence<int> *s1 = new MutableArraySequence<int>(data1, 3);
		Sequence<int> *s2 = new MutableArraySequence<int>(data2, 3);
		PrintRowIndented("-> Seq1", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(s1),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("-> Seq2", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(s2),
						 Color::BRIGHT_GREEN);

		auto *zipped = Zip(*s1, *s2);
		PrintRowIndented("-> Zip Result", Color::BRIGHT_MAGENTA);
		for (int i = 0; i < zipped->GetLength(); ++i) {
			auto pair = zipped->Get(i);
			PrintRowIndented("   Пара [" + std::to_string(i) + "]: (" +
								 std::to_string(pair.item1) + ", " +
								 std::to_string(pair.item2) + ")",
							 Color::BRIGHT_GREEN);
		}
		delete zipped;
		delete s1;
		delete s2;
		CloseBox();
	}

	void TestSlice() {
		OpenBox();
		PrintSection("SLICE");
		PrintEmpty();
		int data[] = {1, 2, 3, 4, 5};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 5);
		int ins[] = {9, 10};
		Sequence<int> *insertSeq = new MutableArraySequence<int>(ins, 2);
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
		OpenBox();
		PrintSection("DYNAMIC ARRAY");
		PrintEmpty();
		int data[] = {10, 20, 30, 40, 50};
		DynamicArray<int> da(data, 5);
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
		OpenBox();
		PrintSection("LINKED LIST");
		PrintEmpty();
		LinkedList<int> ll;
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
		OpenBox();
		PrintSection("MUTABLE SEQUENCE");
		PrintEmpty();
		int data[] = {1, 2, 3};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 3);
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
		OpenBox();
		PrintSection("IMMUTABLE SEQUENCE");
		PrintEmpty();
		int data[] = {1, 2, 3};
		Sequence<int> *seq = new ImmutableArraySequence<int>(data, 3);
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
		OpenBox();
		PrintSection("MAP-REDUCE");
		PrintEmpty();
		int data[] = {1, 2, 3, 4, 5};
		MutableArraySequence<int> *seq = new MutableArraySequence<int>(data, 5);
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
		OpenBox();
		PrintSection("ИТЕРАТОРЫ");
		PrintEmpty();
		int data[] = {10, 20, 30, 40, 50};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 5);
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
		OpenBox();
		PrintSection("OPTION<T>");
		PrintEmpty();
		int data[] = {1, 2, 3};
		Sequence<int> *seq = new MutableArraySequence<int>(data, 3);
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
		Sequence<int> *s1 = From(data1, 3);
		PrintRowIndented("-> From([1,2,3])", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(s1),
						 Color::BRIGHT_GREEN);
		int data2[] = {4, 5, 6};
		Sequence<int> *s2 = From(data2, 3);
		PrintRowIndented("-> From([4,5,6])", Color::BRIGHT_BLUE);
		PrintRowIndented("   Состояние: " + GetSeqState(s2),
						 Color::BRIGHT_GREEN);
		Sequence<int> *concatenated = Concat(*s1, *s2);
		PrintRowIndented("-> Concat", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Результат: " + GetSeqState(concatenated),
						 Color::BRIGHT_GREEN);
		delete s1;
		delete s2;
		delete concatenated;
		CloseBox();
	}

	void TestDequeSegmented() {
		OpenBox();
		PrintSection("ДЕК С СЕГМЕНТИРОВАННЫМ БУФЕРОМ");
		PrintEmpty();
		DequeSegmented<int> deque(3);
		PrintRowIndented("-> Инициализация (segmentSize=3)",
						 Color::BRIGHT_CYAN);
		PrintRowIndented("   Состояние: " + GetDequeState(deque),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("   Сегментов: " +
							 std::to_string(deque.GetSegmentCount()),
						 Color::BRIGHT_BLACK);
		PrintEmpty();

		deque.PushBack(1);
		deque.PushBack(2);
		deque.PushBack(3);
		PrintRowIndented("-> PushBack(1, 2, 3)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetDequeState(deque),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("   Сегментов: " +
							 std::to_string(deque.GetSegmentCount()),
						 Color::BRIGHT_BLACK);
		PrintEmpty();

		deque.PushBack(4);
		deque.PushBack(5);
		deque.PushBack(6);
		PrintRowIndented("-> PushBack(4, 5, 6)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetDequeState(deque),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("   Сегментов: " +
							 std::to_string(deque.GetSegmentCount()),
						 Color::BRIGHT_BLACK);
		PrintEmpty();

		deque.PushFront(0);
		PrintRowIndented("-> PushFront(0)", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetDequeState(deque),
						 Color::BRIGHT_GREEN);
		PrintRowIndented("   Сегментов: " +
							 std::to_string(deque.GetSegmentCount()),
						 Color::BRIGHT_BLACK);
		PrintEmpty();

		int f = deque.PopFront();
		int b = deque.PopBack();
		PrintRowIndented("-> PopFront() & PopBack()", Color::BRIGHT_YELLOW);
		PrintRowIndented("   Удалено: front=" + std::to_string(f) +
							 ", back=" + std::to_string(b),
						 Color::BRIGHT_CYAN);
		PrintRowIndented("   Осталось: " + GetDequeState(deque),
						 Color::BRIGHT_GREEN);
		PrintEmpty();

		DequeSegmented<int> sortDeque(3);
		sortDeque.PushBack(5);
		sortDeque.PushBack(2);
		sortDeque.PushBack(8);
		sortDeque.PushBack(1);
		sortDeque.PushBack(9);
		PrintRowIndented("-> До сортировки", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetDequeState(sortDeque),
						 Color::BRIGHT_GREEN);
		sortDeque.Sort();
		PrintRowIndented("-> После Sort()", Color::BRIGHT_MAGENTA);
		PrintRowIndented("   Состояние: " + GetDequeState(sortDeque),
						 Color::BRIGHT_GREEN);
		CloseBox();
	}

	void TestHanoiTower() {
		Sequence<std::string> *diskOpts =
			new MutableArraySequence<std::string>();
		for (int i = 1; i <= 10; ++i)
			diskOpts->Append(std::to_string(i));
		int diskIdx =
			SelectFromMenu("ВЫБЕРИТЕ КОЛИЧЕСТВО КОЛЕЦ (1-10)", diskOpts);
		int diskCount = diskIdx + 1;
		delete diskOpts;

		Sequence<std::string> *cfgOpts =
			new MutableArraySequence<std::string>();
		cfgOpts->Append("Классический (1 → 3)");
		cfgOpts->Append("Произвольный");
		bool isCustom = (SelectFromMenu("РЕЖИМ КОНФИГУРАЦИИ", cfgOpts) == 1);
		delete cfgOpts;

		int start = 1, end = 3;
		if (isCustom) {
			Sequence<std::string> *towOpts =
				new MutableArraySequence<std::string>();
			towOpts->Append("Стержень 1");
			towOpts->Append("Стержень 2");
			towOpts->Append("Стержень 3");
			start = SelectFromMenu("ВЫБОР СТАРТОВОГО СТЕРЖНЯ", towOpts) + 1;
			delete towOpts;

			Sequence<std::string> *endOpts =
				new MutableArraySequence<std::string>();
			for (int i = 1; i <= 3; ++i)
				if (i != start)
					endOpts->Append("Стержень " + std::to_string(i));
			int endIdx = SelectFromMenu("ВЫБОР ЦЕЛЕВОГО СТЕРЖНЯ", endOpts);
			int c = 0;
			for (int i = 1; i <= 3; ++i)
				if (i != start) {
					if (c == endIdx) {
						end = i;
						break;
					}
					c++;
				}
			delete endOpts;
		}
		Sequence<std::string> *modeOpts =
			new MutableArraySequence<std::string>();
		modeOpts->Append("Автоматический");
		modeOpts->Append("Пошаговый");

		int modeIdx = SelectFromMenu("РЕЖИМ РЕШЕНИЯ", modeOpts);
		bool interactive = (modeIdx == 1);

		delete modeOpts;

		HanoiTower hanoi(diskCount, start);
		HanoiRenderer renderer(hanoi);

		if (interactive) {
			std::function<void(int, int, int)> solve = [&](int f, int t,
														   int n) {
				if (n == 1) {
					hanoi.Move(f, t);
					ClearScreen();
					renderer.PrintBoard();
					std::cout << "  Ход выполнен. Нажмите Enter...";
					std::cout.flush();
					while (ReadKey() != 13)
						;
					return;
				}
				solve(f, 3 - f - t, n - 1);
				hanoi.Move(f, t);
				ClearScreen();
				renderer.PrintBoard();
				std::cout << "  Ход выполнен. Нажмите Enter...";
				std::cout.flush();
				while (ReadKey() != 13)
					;
				solve(3 - f - t, t, n - 1);
			};

			ClearScreen();
			renderer.PrintBoard();
			std::cout << "  Нажмите Enter для старта...";
			std::cout.flush();
			while (ReadKey() != 13)
				;
			solve(start - 1, end - 1, diskCount);
		} else {
			ClearScreen();
			renderer.PrintBoard();
			std::cout << "  Решение в автоматическом режиме...\n";
			std::cout.flush();
			Sleep(500);
			hanoi.SolveCustom(start, end);
		}

		ClearScreen();
		renderer.PrintStats();
		renderer.PrintHistory();
		std::cout << "\n  Нажмите Enter для возврата...";
		std::cout.flush();
		while (ReadKey() != 13)
			;
	}
};