#pragma once

#include <iostream>
#include <limits>

#include "array_sequence.h"
#include "deque_segmented.h"
#include "ring.h"
#include "utils.h"

class HanoiTower {
   private:
    DequeSegmented<Ring>* towers[3];
    int diskCount;
    int moveCount;
    Sequence<std::string>* moves;
    bool interactive;

   public:
    HanoiTower(int disks = 3, bool interact = false) : diskCount(disks), moveCount(0), interactive(interact) {
        for (int i = 0; i < 3; i++) {
            towers[i] = new DequeSegmented<Ring>(disks > 4 ? disks : 4);
        }

        moves = new MutableArraySequence<std::string>();

        std::string colors[] = {
            "\033[91m", "\033[93m", "\033[92m", "\033[96m", "\033[94m", "\033[95m", "\033[97m",
        };

        for (int i = diskCount; i >= 1; i--) {
            std::string color = colors[(diskCount - i) % 7];
            Ring ring(i, color);
            towers[0]->PushBack(ring);
        }
    }

    HanoiTower(int disks, int startTower, bool interact = false)
        : diskCount(disks), moveCount(0), interactive(interact) {
        for (int i = 0; i < 3; i++) {
            towers[i] = new DequeSegmented<Ring>(disks > 4 ? disks : 4);
        }

        moves = new MutableArraySequence<std::string>();

        std::string colors[] = {
            "\033[91m", "\033[93m", "\033[92m", "\033[96m", "\033[94m", "\033[95m", "\033[97m",
        };

        int startIdx = startTower - 1;
        for (int i = diskCount; i >= 1; i--) {
            std::string color = colors[(diskCount - i) % 7];
            Ring ring(i, color);
            towers[startIdx]->PushBack(ring);
        }
    }

    ~HanoiTower() {
        for (int i = 0; i < 3; i++) {
            if (towers[i] != nullptr) {
                delete towers[i];
                towers[i] = nullptr;
            }
        }
        if (moves != nullptr) {
            delete moves;
            moves = nullptr;
        }
    }

    void Print() const {
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    ХАНОЙСКАЯ БАШНЯ                        ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

        int maxHeight = diskCount;
        int maxRingWidth = diskCount * 2 + 1;

        for (int level = maxHeight - 1; level >= 0; level--) {
            std::cout << "  ";

            for (int tower = 0; tower < 3; tower++) {
                if (towers[tower] != nullptr && level < towers[tower]->GetLength()) {
                    Ring ring = towers[tower]->Get(level);
                    int ringWidth = ring.GetSize() * 2 + 1;
                    int padding = (maxRingWidth - ringWidth) / 2;

                    for (int i = 0; i < padding; i++) std::cout << " ";
                    ring.Print();
                    for (int i = 0; i < padding; i++) std::cout << " ";
                } else {
                    int padding = (maxRingWidth - 1) / 2;
                    for (int i = 0; i < padding; i++) std::cout << " ";
                    std::cout << "│";
                    for (int i = 0; i < padding; i++) std::cout << " ";
                }

                if (tower < 2) 
                    std::cout << "   ";
            }

            std::cout << "  \n";
        }

        std::cout << "\n  ";
        for (int tower = 0; tower < 3; tower++) {
            for (int i = 0; i < maxRingWidth; i++) 
                std::cout << "═";
            if (tower < 2) 
                std::cout << "   ";
        }
        std::cout << "  \n";

        std::cout << "  ";
        for (int tower = 0; tower < 3; tower++) {
            int numPadding = (maxRingWidth - 1) / 2;

            for (int i = 0; i < numPadding; i++) 
                std::cout << " ";
                
            std::cout << (tower + 1);
            for (int i = 0; i < numPadding; i++) 
                std::cout << " ";
            if (tower < 2) 
                std::cout << "   ";
        }
        std::cout << "  \n\n";
    }

    void PrintStats() const {
        const int BOX_WIDTH = 50;

        std::cout << "\n┌";
        for (int i = 0; i < BOX_WIDTH - 2; i++) std::cout << "─";
        std::cout << "┐\n";

        std::string title = " СТАТИСТИКА ";
        int titlePad = (BOX_WIDTH - 2 - VisibleLength(title)) / 2;
        std::cout << "│";
        for (int i = 0; i < titlePad; i++) std::cout << " ";
        std::cout << title;
        for (int i = 0; i < BOX_WIDTH - 2 - VisibleLength(title) - titlePad; i++) std::cout << " ";
        std::cout << "│\n";

        std::cout << "├";
        for (int i = 0; i < BOX_WIDTH - 2; i++) std::cout << "─";
        std::cout << "┤\n";

        std::string diskText = " Колец: " + std::to_string(diskCount);
        int diskPad = BOX_WIDTH - 2 - VisibleLength(diskText);
        std::cout << "│" << diskText;
        for (int i = 0; i < diskPad; i++) std::cout << " ";
        std::cout << "│\n";

        std::string moveText = " Ходов: " + std::to_string(moveCount);
        int movePad = BOX_WIDTH - 2 - VisibleLength(moveText);
        std::cout << "│" << moveText;
        for (int i = 0; i < movePad; i++) std::cout << " ";
        std::cout << "│\n";

        int minMoves = (1 << diskCount) - 1;
        std::string minText = " Минимум: " + std::to_string(minMoves);
        int minPad = BOX_WIDTH - 2 - VisibleLength(minText);
        std::cout << "│" << minText;
        for (int i = 0; i < minPad; i++) std::cout << " ";
        std::cout << "│\n";

        std::cout << "└";
        for (int i = 0; i < BOX_WIDTH - 2; i++) std::cout << "─";
        std::cout << "┘\n";
    }

    void Move(int from, int to) {
        if (from < 0 || from > 2 || to < 0 || to > 2) {
            throw IndexOutOfRangeException("Invalid tower index in Move");
        }

        if (towers[from] == nullptr || towers[to] == nullptr) {
            throw InvalidOperationException("Tower is null");
        }

        if (towers[from]->GetLength() == 0) {
            throw InvalidOperationException("Source tower is empty");
        }

        Ring ring = towers[from]->PopBack();

        if (towers[to]->GetLength() > 0) {
            Ring topRing = towers[to]->GetBack();
            if (ring.GetSize() > topRing.GetSize()) {
                towers[from]->PushBack(ring);
                throw InvalidOperationException("Cannot place larger ring on smaller");
            }
        }

        towers[to]->PushBack(ring);
        moveCount++;

        std::string move = "Ход " + std::to_string(moveCount) + ": Кольцо " + std::to_string(ring.GetSize()) +
                           " | Стержень " + std::to_string(from + 1) + " → Стержень " + std::to_string(to + 1);

        moves->Append(move);

        if (interactive) {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif

            std::cout << "\n" << move << "\n";
            Print();

            std::cout << "  Нажмите Enter для следующего хода...";
            std::cout.flush();

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    void Solve(int from, int to, int aux, int n) {
        if (n == 1) {
            Move(from, to);
            return;
        }

        Solve(from, aux, to, n - 1);
        Move(from, to);
        Solve(aux, to, from, n - 1);
    }

    void Solve() {
        Solve(0, 2, 1, diskCount);
    }

    void SolveInteractive() {
        interactive = true;
        Solve(0, 2, 1, diskCount);
        interactive = false;
    }

    void SolveCustom(int startTower, int endTower) {
        int auxTower = 3 - startTower - endTower;
        interactive = true;
        Solve(startTower - 1, endTower - 1, auxTower - 1, diskCount);
        interactive = false;
    }

    bool IsComplete() const {
        return towers[0] != nullptr && towers[0]->GetLength() == 0 && towers[1] != nullptr &&
               towers[1]->GetLength() == 0 && towers[2] != nullptr && towers[2]->GetLength() == diskCount;
    }

    DequeSegmented<Ring>* GetTower(int index) const {
        if (index < 0 || index > 2) {
            throw IndexOutOfRangeException("Invalid tower index");
        }
        return towers[index];
    }

    int GetMoveCount() const {
        return moveCount;
    }
    int GetDiskCount() const {
        return diskCount;
    }

    void PrintMoveHistory() const {
        const int BOX_WIDTH = 70;

        std::cout << "\n┌";
        for (int i = 0; i < BOX_WIDTH - 2; i++) std::cout << "─";
        std::cout << "┐\n";

        std::string title = " ИСТОРИЯ ХОДОВ (" + std::to_string(moveCount) + " ходов) ";
        int titlePad = (BOX_WIDTH - 2 - VisibleLength(title)) / 2;
        std::cout << "│";
        for (int i = 0; i < titlePad; i++) std::cout << " ";
        std::cout << title;
        for (int i = 0; i < BOX_WIDTH - 2 - VisibleLength(title) - titlePad; i++) std::cout << " ";
        std::cout << "│\n";

        std::cout << "├";
        for (int i = 0; i < BOX_WIDTH - 2; i++) std::cout << "─";
        std::cout << "┤\n";

        int start = moveCount > 10 ? moveCount - 10 : 0;
        for (int i = start; i < moveCount; i++) {
            std::string line = " " + moves->Get(i);
            int linePad = BOX_WIDTH - 2 - VisibleLength(line);
            std::cout << "│" << line;
            for (int j = 0; j < linePad; j++) std::cout << " ";
            std::cout << "│\n";
        }

        std::cout << "└";
        for (int i = 0; i < BOX_WIDTH - 2; i++) std::cout << "─";
        std::cout << "┘\n";
    }
};