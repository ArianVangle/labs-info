#pragma once
#include "array_sequence.h"
#include "deque_segmented.h"

template <class T>
DequeSegmented<T>::DequeSegmented(int segSize)
	: segmentSize(segSize), totalCount(0) {
	chunks = new MutableArraySequence<Sequence<T> *>();
}

template <class T>
DequeSegmented<T>::DequeSegmented(const DequeSegmented<T> &other)
	: segmentSize(other.segmentSize), totalCount(0) {
	chunks = new MutableArraySequence<Sequence<T> *>();
	IEnumerator<T> *en = other.GetEnumerator();
	while (en->MoveNext())
		this->PushBack(en->Current());
	delete en;
}

template <class T> DequeSegmented<T>::~DequeSegmented() {
	IEnumerator<Sequence<T> *> *en = chunks->GetEnumerator();
	while (en->MoveNext())
		delete en->Current();
	delete en;
	delete chunks;
}

template <class T> void DequeSegmented<T>::PushBack(const T &item) {
	int lastIdx = chunks->GetLength() - 1;
	if (lastIdx < 0 || chunks->Get(lastIdx)->GetLength() >= segmentSize) {
		MutableArraySequence<T> *newSeg = new MutableArraySequence<T>();
		newSeg->Append(item);
		chunks->Append(newSeg);
	} else {
		chunks->Get(lastIdx)->Append(item);
	}
	totalCount++;
}

template <class T> void DequeSegmented<T>::PushFront(const T &item) {
	int len0 = (chunks->GetLength() > 0) ? chunks->Get(0)->GetLength() : 0;

	if (len0 >= segmentSize) {
		MutableArraySequence<T> *newSeg = new MutableArraySequence<T>();
		newSeg->Append(item);
		chunks->Prepend(newSeg);
	} else if (chunks->GetLength() == 0) {
		MutableArraySequence<T> *newSeg = new MutableArraySequence<T>();
		newSeg->Append(item);
		chunks->Append(newSeg);
	} else {
		Sequence<T> *oldFirst = chunks->Get(0);
		MutableArraySequence<T> *newFirst = new MutableArraySequence<T>();
		newFirst->Append(item);
		IEnumerator<T> *en = oldFirst->GetEnumerator();
		while (en->MoveNext())
			newFirst->Append(en->Current());
		delete en;
		delete chunks->Get(0);
		chunks->Set(0, newFirst);
	}
	totalCount++;
}

template <class T> T DequeSegmented<T>::PopBack() {
	if (totalCount == 0)
		throw InvalidOperationException("Deque is empty");
	int lastIdx = chunks->GetLength() - 1;
	Sequence<T> *last = chunks->Get(lastIdx);
	T value = last->Get(last->GetLength() - 1);

	if (last->GetLength() == 1) {
		delete last;
		if (chunks->GetLength() > 1) {
			Sequence<Sequence<T> *> *newChunks =
				chunks->GetSubsequence(0, chunks->GetLength() - 2);
			delete chunks;
			chunks = newChunks;
		} else {
			delete chunks;
			chunks = new MutableArraySequence<Sequence<T> *>();
		}
	} else {
		MutableArraySequence<T> *newLast = new MutableArraySequence<T>();
		IEnumerator<T> *en = last->GetEnumerator();
		int count = 0;
		while (en->MoveNext()) {
			if (count < last->GetLength() - 1)
				newLast->Append(en->Current());
			count++;
		}
		delete en;
		delete last;
		chunks->Set(lastIdx, newLast);
	}
	totalCount--;
	return value;
}

template <class T> T DequeSegmented<T>::PopFront() {
	if (totalCount == 0)
		throw InvalidOperationException("Deque is empty");
	Sequence<T> *first = chunks->Get(0);
	T value = first->Get(0);

	if (first->GetLength() == 1) {
		delete first;
		if (chunks->GetLength() > 1) {
			Sequence<Sequence<T> *> *newChunks =
				chunks->GetSubsequence(1, chunks->GetLength() - 1);
			delete chunks;
			chunks = newChunks;
		} else {
			delete chunks;
			chunks = new MutableArraySequence<Sequence<T> *>();
		}
	} else {
		MutableArraySequence<T> *newFirst = new MutableArraySequence<T>();
		IEnumerator<T> *en = first->GetEnumerator();
		en->MoveNext();
		while (en->MoveNext())
			newFirst->Append(en->Current());
		delete en;
		delete first;
		chunks->Set(0, newFirst);
	}
	totalCount--;
	return value;
}

template <class T> T DequeSegmented<T>::GetFront() const {
	if (totalCount == 0)
		throw InvalidOperationException("Deque is empty");
	return chunks->Get(0)->Get(0);
}

template <class T> T DequeSegmented<T>::GetBack() const {
	if (totalCount == 0)
		throw InvalidOperationException("Deque is empty");
	int lastIdx = chunks->GetLength() - 1;
	return chunks->Get(lastIdx)->Get(chunks->Get(lastIdx)->GetLength() - 1);
}

template <class T> void DequeSegmented<T>::Clear() {
	IEnumerator<Sequence<T> *> *en = chunks->GetEnumerator();
	while (en->MoveNext())
		delete en->Current();
	delete en;
	delete chunks;
	chunks = new MutableArraySequence<Sequence<T> *>();
	totalCount = 0;
}

template <class T> T DequeSegmented<T>::Get(size_t index) const {
	if (index >= (size_t)totalCount)
		throw IndexOutOfRangeException("Index out of range");
	int remaining = (int)index;
	for (int i = 0; i < chunks->GetLength(); i++) {
		int segLen = chunks->Get(i)->GetLength();
		if (remaining < segLen)
			return chunks->Get(i)->Get(remaining);
		remaining -= segLen;
	}
	throw IndexOutOfRangeException("Logic error in Get");
}

template <class T> size_t DequeSegmented<T>::GetCount() const {
	return totalCount;
}
template <class T> int DequeSegmented<T>::GetLength() const {
	return totalCount;
}
template <class T> T DequeSegmented<T>::GetFirst() const { return GetFront(); }
template <class T> T DequeSegmented<T>::GetLast() const { return GetBack(); }

template <class T> Sequence<T> *DequeSegmented<T>::Clone() const {
	DequeSegmented<T> *res = new DequeSegmented<T>(segmentSize);
	IEnumerator<T> *en = this->GetEnumerator();
	while (en->MoveNext())
		res->PushBack(en->Current());
	delete en;
	return res;
}

template <class T> Sequence<T> *DequeSegmented<T>::Append(const T &item) {
	this->PushBack(item);
	return this;
}
template <class T> Sequence<T> *DequeSegmented<T>::Prepend(const T &item) {
	this->PushFront(item);
	return this;
}

template <class T>
Sequence<T> *DequeSegmented<T>::InsertAt(const T &item, int index) {
	if (index < 0 || index > totalCount)
		throw IndexOutOfRangeException("Index out of range");
	DequeSegmented<T> *newDeque = new DequeSegmented<T>(segmentSize);

	IEnumerator<T> *en = this->GetEnumerator();
	int i = 0;
	while (en->MoveNext()) {
		if (i == index)
			newDeque->PushBack(item);
		newDeque->PushBack(en->Current());
		i++;
	}
	if (index == totalCount)
		newDeque->PushBack(item);
	delete en;
	return newDeque;
}

template <class T> void DequeSegmented<T>::Set(size_t index, const T &value) {
	if (index >= (size_t)totalCount)
		throw IndexOutOfRangeException("Index out of range");
	int remaining = (int)index;
	for (int i = 0; i < chunks->GetLength(); i++) {
		int segLen = chunks->Get(i)->GetLength();
		if (remaining < segLen) {
			chunks->Get(i)->Set(remaining, value);
			return;
		}
		remaining -= segLen;
	}
}

template <class T>
Sequence<T> *DequeSegmented<T>::GetSubsequence(int start, int end) const {
	if (start < 0 || end >= totalCount || start > end)
		throw IndexOutOfRangeException("Invalid indices");
	auto *res = new DequeSegmented<T>(segmentSize);
	for (int i = start; i <= end; i++)
		res->PushBack(this->Get(i));
	return res;
}

template <class T>
Sequence<T> *DequeSegmented<T>::Concat(const Sequence<T> &list) {
	auto *res = new DequeSegmented<T>(segmentSize);
	IEnumerator<T> *en1 = this->GetEnumerator();
	while (en1->MoveNext()) {
		res->PushBack(en1->Current());
	}
	delete en1;
	IEnumerator<T> *en2 = list.GetEnumerator();
	while (en2->MoveNext()) {
		res->PushBack(en2->Current());
	}
	delete en2;
	return res;
}

template <class T> T DequeSegmented<T>::operator[](int index) const {
	return Get(index);
}

template <class T>
void DequeSegmented<T>::Sort(std::function<bool(T, T)> comparator) {
	if (totalCount <= 1)
		return;
	int count = totalCount;
	T *arr = new T[count];

	int idx = 0;
	IEnumerator<T> *en = this->GetEnumerator();
	while (en->MoveNext())
		arr[idx++] = en->Current();
	delete en;

	std::sort(arr, arr + count, comparator);
	Clear();
	for (int i = 0; i < count; i++)
		PushBack(arr[i]);
	delete[] arr;
}
template <class T>
int DequeSegmented<T>::FindSubsequence(const Sequence<T> &pat) const {
	if (pat.GetLength() == 0)
		return 0;
	if (pat.GetLength() > totalCount)
		return -1;
	for (int i = 0; i <= totalCount - pat.GetLength(); i++) {
		bool match = true;
		for (int j = 0; j < pat.GetLength(); j++) {
			if (Get(i + j) != pat.Get(j)) {
				match = false;
				break;
			}
		}
		if (match)
			return i;
	}
	return -1;
}

template <class T>
Sequence<T> *DequeSegmented<T>::Merge(const Sequence<T> &other,
									  std::function<bool(T, T)> comp) {
	auto *res = new DequeSegmented<T>(segmentSize);
	IEnumerator<T> *en1 = this->GetEnumerator();
	IEnumerator<T> *en2 = other.GetEnumerator();
	bool h1 = en1->MoveNext(), h2 = en2->MoveNext();
	while (h1 && h2) {
		if (comp(en1->Current(), en2->Current())) {
			res->PushBack(en1->Current());
			h1 = en1->MoveNext();
		} else {
			res->PushBack(en2->Current());
			h2 = en2->MoveNext();
		}
	}
	while (h1) {
		res->PushBack(en1->Current());
		h1 = en1->MoveNext();
	}
	while (h2) {
		res->PushBack(en2->Current());
		h2 = en2->MoveNext();
	}
	delete en1;
	delete en2;
	return res;
}

template <class T>
template <class R>
Sequence<R> *DequeSegmented<T>::Map(std::function<R(T)> func) const {
	auto *res = new DequeSegmented<R>(segmentSize);
	IEnumerator<T> *en = this->GetEnumerator();
	while (en->MoveNext())
		res->PushBack(func(en->Current()));
	delete en;
	return res;
}

template <class T>
Sequence<T> *DequeSegmented<T>::Where(std::function<bool(T)> func) const {
	auto *res = new DequeSegmented<T>(segmentSize);
	IEnumerator<T> *en = this->GetEnumerator();
	while (en->MoveNext())
		if (func(en->Current()))
			res->PushBack(en->Current());
	delete en;
	return res;
}

template <class T>
T DequeSegmented<T>::Reduce(std::function<T(T, T)> func, T start) const {
	T res = start;
	IEnumerator<T> *en = this->GetEnumerator();
	while (en->MoveNext())
		res = func(res, en->Current());
	delete en;
	return res;
}

template <class T> Option<T> DequeSegmented<T>::TryGet(int index) const {
	try {
		return Option<T>::Some(Get(index));
	} catch (...) {
		return Option<T>::None();
	}
}

template <class T> IEnumerator<T> *DequeSegmented<T>::GetEnumerator() const {
	return new DequeEnumerator<T>(this);
}

template <class T> int DequeSegmented<T>::GetSegmentCount() const {
	return chunks->GetLength();
}
template <class T> int DequeSegmented<T>::GetSegmentSize() const {
	return segmentSize;
}
template <class T> double DequeSegmented<T>::GetMemoryEfficiency() const {
	int totalCap = chunks->GetLength() * segmentSize;
	return totalCap == 0 ? 0.0 : (double)totalCount / totalCap * 100.0;
}