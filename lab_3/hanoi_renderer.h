#pragma once
#include "hanoi.h"
#include "utils.h"
#include <iostream>
#include <string>
#include "ring_renderer.h"

class HanoiRenderer {
  private:
	const HanoiTower &hanoi;

	void printRowPadded(const std::string &text, int width) const {
		std::cout << "│";
		int pad1 = (width - 2 - VisibleLength(text)) / 2;
		int pad2 = width - 2 - VisibleLength(text) - pad1;
		for (int i = 0; i < pad1; ++i)
			std::cout << " ";
		std::cout << text;
		for (int i = 0; i < pad2; ++i)
			std::cout << " ";
		std::cout << "│\n";
	}

  public:
	HanoiRenderer(const HanoiTower &h) : hanoi(h) {}

	void PrintBoard() const {
		std::cout << "\n╔══════════════════════════════════════════════════════"
					 "═════╗\n";
		std::cout << "║                    ХАНОЙСКАЯ БАШНЯ                     "
					 "   ║\n";
		std::cout << "╚════════════════════════════════════════════════════════"
					 "═══╝\n\n";

		int maxH = hanoi.GetDiskCount();
		int maxW = maxH * 2 + 1;

		for (int level = maxH - 1; level >= 0; --level) {
			std::cout << "  ";
			for (int t = 0; t < 3; ++t) {
				const auto *tower = hanoi.GetTower(t);
				if (level < tower->GetLength()) {
					Ring r = tower->Get(level);
					int rw = r.GetSize() * 2 + 1;
					int pad = (maxW - rw) / 2;
					for (int i = 0; i < pad; ++i)
						std::cout << " ";
					RingRenderer::Render(r);

					for (int i = 0; i < pad; ++i)
						std::cout << " ";
				} else {
					int pad = (maxW - 1) / 2;
					for (int i = 0; i < pad; ++i)
						std::cout << " ";
					std::cout << "│";
					for (int i = 0; i < pad; ++i)
						std::cout << " ";
				}
				if (t < 2)
					std::cout << "   ";
			}
			std::cout << "\n";
		}
		std::cout << "\n  ";
		for (int t = 0; t < 3; ++t) {
			for (int i = 0; i < maxW; ++i)
				std::cout << "═";
			if (t < 2)
				std::cout << "   ";
		}
		std::cout << "\n  ";
		for (int t = 0; t < 3; ++t) {
			int pad = (maxW - 1) / 2;
			for (int i = 0; i < pad; ++i)
				std::cout << " ";
			std::cout << (t + 1);
			for (int i = 0; i < pad; ++i)
				std::cout << " ";
			if (t < 2)
				std::cout << "   ";
		}
		std::cout << "\n\n";
	}

	void PrintStats() const {
		const int BW = 50;
		std::cout << "\n┌";
		for (int i = 0; i < BW - 2; ++i)
			std::cout << "─";
		std::cout << "┐\n";
		printRowPadded(" СТАТИСТИКА ", BW);
		std::cout << "├";
		for (int i = 0; i < BW - 2; ++i)
			std::cout << "─";
		std::cout << "┤\n";
		printRowPadded(" Колец: " + std::to_string(hanoi.GetDiskCount()), BW);
		printRowPadded(" Ходов: " + std::to_string(hanoi.GetMoveCount()), BW);
		printRowPadded(
			" Минимум: " + std::to_string((1 << hanoi.GetDiskCount()) - 1), BW);
		std::cout << "└";
		for (int i = 0; i < BW - 2; ++i)
			std::cout << "─";
		std::cout << "┘\n";
	}

	void PrintHistory() const {
		const int BW = 70;
		const auto *hist = hanoi.GetMoveHistory();
		std::cout << "\n┌";
		for (int i = 0; i < BW - 2; ++i)
			std::cout << "─";
		std::cout << "┐\n";

		std::string title =
			" ИСТОРИЯ ХОДОВ (" + std::to_string(hanoi.GetMoveCount()) + ") ";
		std::cout << "│";
		int p1 = (BW - 2 - VisibleLength(title)) / 2;
		int p2 = BW - 2 - VisibleLength(title) - p1;
		for (int i = 0; i < p1; ++i)
			std::cout << " ";
		std::cout << title;
		for (int i = 0; i < p2; ++i)
			std::cout << " ";
		std::cout << "│\n";

		std::cout << "├";
		for (int i = 0; i < BW - 2; ++i)
			std::cout << "─";
		std::cout << "┤\n";
		int start = hanoi.GetMoveCount() > 10 ? hanoi.GetMoveCount() - 10 : 0;
		for (int i = start; i < hanoi.GetMoveCount(); ++i) {
			std::string line = " " + hist->Get(i);
			std::cout << "│" << line;
			int pad = BW - 2 - VisibleLength(line);
			for (int j = 0; j < pad; ++j)
				std::cout << " ";
			std::cout << "│\n";
		}
		std::cout << "└";
		for (int i = 0; i < BW - 2; ++i)
			std::cout << "─";
		std::cout << "┘\n";
	}
};