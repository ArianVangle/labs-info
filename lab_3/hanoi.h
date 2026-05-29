#pragma once
#include "array_sequence.h"
#include "deque_segmented.h"
#include "ring.h"
#include <string>

class HanoiTower {
  private:
	DequeSegmented<Ring> *towers[3];
	int diskCount;
	int moveCount;
	Sequence<std::string> *moves;

	void recordMove(int from, int to, int ringSize) {
		moves->Append("Ход " + std::to_string(++moveCount) + ": Кольцо " +
					  std::to_string(ringSize) + " | Стержень " +
					  std::to_string(from + 1) + " → Стержень " +
					  std::to_string(to + 1));
	}

  public:
	HanoiTower(int disks = 3, int startTower = 1)
		: diskCount(disks), moveCount(0) {
		for (int i = 0; i < 3; ++i)
			towers[i] = new DequeSegmented<Ring>(disks > 4 ? disks : 4);
		moves = new MutableArraySequence<std::string>();

		std::string colors[] = {"\033[91m", "\033[93m", "\033[92m", "\033[96m",
								"\033[94m", "\033[95m", "\033[97m"};
		int startIdx = startTower - 1;
		for (int i = diskCount; i >= 1; --i) {
			towers[startIdx]->PushBack(Ring(i, colors[(diskCount - i) % 7]));
		}
	}

	~HanoiTower() {
		for (int i = 0; i < 3; ++i)
			delete towers[i];
		delete moves;
	}

	void Move(int from, int to) {
		if (from < 0 || from > 2 || to < 0 || to > 2)
			throw IndexOutOfRangeException("Invalid tower index");
		if (towers[from]->GetLength() == 0)
			throw InvalidOperationException("Source tower is empty");

		Ring ring = towers[from]->PopBack();
		if (towers[to]->GetLength() > 0 &&
			ring.GetSize() > towers[to]->GetBack().GetSize()) {
			towers[from]->PushBack(ring);
			throw InvalidOperationException(
				"Cannot place larger ring on smaller");
		}
		towers[to]->PushBack(ring);
		recordMove(from, to, ring.GetSize());
	}

	void SolveRecursive(int from, int to, int aux, int n) {
		if (n == 1) {
			Move(from, to);
			return;
		}
		SolveRecursive(from, aux, to, n - 1);
		Move(from, to);
		SolveRecursive(aux, to, from, n - 1);
	}

	void Solve() { SolveRecursive(0, 2, 1, diskCount); }
	void SolveCustom(int startTower, int endTower) {
		int from = startTower - 1;
		int to = endTower - 1;
		int aux = 3 - from - to;
		SolveRecursive(from, to, aux, diskCount);
	}

	int GetDiskCount() const { return diskCount; }
	int GetMoveCount() const { return moveCount; }
	const DequeSegmented<Ring> *GetTower(int index) const {
		if (index < 0 || index > 2)
			throw IndexOutOfRangeException("Invalid tower index");
		return towers[index];
	}
	const Sequence<std::string> *GetMoveHistory() const { return moves; }
	bool IsComplete() const {
    return (towers[0]->GetLength() == diskCount) ||
           (towers[1]->GetLength() == diskCount) ||
           (towers[2]->GetLength() == diskCount);
	}
};