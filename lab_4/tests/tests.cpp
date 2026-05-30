#include "../include/lab3_base/array_sequence.h"
#include "../include/lab4_new/algorithms.hpp"
#include "../include/lab4_new/cardinal.hpp"
#include "../include/lab4_new/generator.hpp"
#include "../include/lab4_new/lazy_sequence.hpp"
#include "../include/lab4_new/read_only_stream.hpp"
#include "../include/lab4_new/stream.hpp"
#include "../include/lab4_new/stream_adapters.hpp"
#include "../include/lab4_new/write_only_stream.hpp"
#include <chrono>
#include <iostream>

void PrintHeader(const std::string &title) {
	std::cout
		<< "\n╔═══════════════════════════════════════════════════════════╗\n";
	std::cout << "║  " << title;
	int padding = 58 - title.length();
	for (int i = 0; i < padding - 1; i++)
		std::cout << " ";
	std::cout << "║\n";
	std::cout
		<< "╚═══════════════════════════════════════════════════════════╝\n";
}

void Assert(bool condition, const std::string &testName) {
	if (condition) {
		std::cout << "  ✓ [PASS] " << testName << "\n";
	} else {
		std::cout << "  ✗ [FAIL] " << testName << "\n";
	}
}

void TestCardinal() {
	PrintHeader("CARDINAL TESTS");
	Cardinal finite(42);
	Cardinal infinite = Cardinal::Infinity();
	Assert(finite.IsFinite(), "IsFinite");
	Assert(infinite.IsInfinite(), "IsInfinite");
	Assert(finite.GetValue() == 42, "GetValue");
	Assert(finite < infinite, "Comparison: finite < infinite");
	Assert(!(infinite < finite), "Comparison: !(infinite < finite)");
	Cardinal sum = finite + infinite;
	Assert(sum.IsInfinite(), "Addition with Infinity");
	Cardinal sum2 = finite + Cardinal(8);
	Assert(sum2.GetValue() == 50, "Addition: 42 + 8 = 50");
	std::cout << "\n";
}

void TestLazySequence() {
	PrintHeader("LAZY SEQUENCE TESTS");
	auto *factorials = new LazySequence<int>(
		[](Sequence<int> *prev) -> int {
			if (prev->GetLength() == 0)
				return 1;
			return prev->Get(prev->GetLength() - 1) * (int)prev->GetLength();
		},
		Cardinal(10));
	Assert(factorials->Get(0) == 1, "Factorial[0] = 1");
	Assert(factorials->Get(4) == 24, "Factorial[4] = 24");
	Assert(factorials->GetMaterializedCount() == 5, "Materialized count = 5");
	delete factorials;

	auto *fib = new LazySequence<int>(
		[](Sequence<int> *prev) -> int {
			if (prev->GetLength() < 2)
				return 1;
			return prev->Get(prev->GetLength() - 1) +
				   prev->Get(prev->GetLength() - 2);
		},
		Cardinal::Infinity());
	Assert(fib->Get(6) == 13, "Fibonacci[6]");
	Assert(fib->GetCardinalLength().IsInfinite(), "Fibonacci is infinite");

	auto *doubled = fib->Map<int>([](int x) -> int { return x * 2; });
	Assert(doubled->Get(3) == 6, "Map: double");
	delete doubled;
	delete fib;
	std::cout << "\n";
}

void TestGenerator() {
	PrintHeader("GENERATOR TESTS");
	auto *seq = new LazySequence<int>(
		[](Sequence<int> *prev) -> int { return prev->GetLength() + 1; },
		Cardinal(100));
	auto *gen = new RecursiveGenerator<int>(
		seq, [](Sequence<int> *prev) -> int { return prev->GetLength() + 1; },
		10);
	Assert(gen->HasNext(), "HasNext");
	Assert(gen->GetNext() > 0, "GetNext");
	delete gen;
	delete seq;
	std::cout << "\n";
}

void TestStreams() {
	PrintHeader("STREAM TESTS");
	int data[] = {1, 2, 3, 4, 5};
	auto *seq = new MutableArraySequence<int>(data, 5);
	auto *stream = new SequenceReadStream<int>(seq);
	stream->Open();
	Assert(!stream->IsEndOfStream(), "Not at end");
	Assert(stream->Read() == 1, "Read[0]");
	stream->Close();
	delete stream;
	delete seq;
	std::cout << "\n";
}

void TestAlgorithms() {
	PrintHeader("ALGORITHM TESTS");
	OnlineStatistics<int> stats;
	stats.Add(10);
	stats.Add(20);
	stats.Add(30);
	Assert(stats.GetCount() == 3, "Stats Count");
	Assert(stats.GetMean() == 20, "Stats Mean");
	std::cout << "\n";
}

void TestConcatScenarios() {
	PrintHeader("CONCATENATION TESTS");

	auto *seqA = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; }, Cardinal(5));
	auto *seqB = new LazySequence<int>(
		[](Sequence<int> *p) { return 100 + p->GetLength(); }, Cardinal(3));
	auto *resAB = seqA->Concat(*seqB);

	auto *lazyResAB = dynamic_cast<LazySequence<int> *>(resAB);
	Assert(lazyResAB && lazyResAB->GetCardinalLength().IsFinite(),
		   "Concat: Finite + Finite is Finite");
	Assert(resAB->GetCount() == 8, "Concat: Length 5 + 3 = 8");
	Assert(resAB->Get(0) == 1, "Concat: A[0] = 1");
	Assert(resAB->Get(7) == 102, "Concat: B[2] = 102");
	delete seqA;
	delete seqB;
	delete resAB;

	auto *inf = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; },
		Cardinal::Infinity());
	auto *finite = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; }, Cardinal(10));
	auto *resInfFin = inf->Concat(*finite);
	auto *lazyResInfFin = dynamic_cast<LazySequence<int> *>(resInfFin);
	Assert(lazyResInfFin && lazyResInfFin->GetCardinalLength().IsInfinite(),
		   "Concat: Infinite + Finite is Infinite");
	Assert(resInfFin->Get(1000) == 1001,
		   "Concat: Access to first infinite block works");
	delete inf;
	delete finite;
	delete resInfFin;

	auto *fin = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; }, Cardinal(3));
	auto *inf2 = new LazySequence<int>(
		[](Sequence<int> *p) { return 1000 + p->GetLength(); },
		Cardinal::Infinity());
	auto *resFinInf = fin->Concat(*inf2);
	auto *lazyResFinInf = dynamic_cast<LazySequence<int> *>(resFinInf);
	Assert(lazyResFinInf && lazyResFinInf->GetCardinalLength().IsInfinite(),
		   "Concat: Finite + Infinite is Infinite");
	Assert(resFinInf->Get(3) == 1000, "Concat: First of infinite part");
	delete fin;
	delete inf2;
	delete resFinInf;
	std::cout << "\n";
}

void TestInsertOperations() {
	PrintHeader("INSERT OPERATIONS TESTS");

	auto *seq = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; }, Cardinal(5));
	auto *inserted = seq->InsertAt(999, 2);
	auto *lazyIns = dynamic_cast<LazySequence<int> *>(inserted);
	Assert(lazyIns && lazyIns->GetCardinalLength().IsFinite(),
		   "Insert: Finite remains Finite");
	Assert(inserted->GetCount() == 6, "Insert: Length increased by 1");
	Assert(inserted->Get(2) == 999, "Insert: Element at index 2");
	delete seq;
	delete inserted;

	auto *inf = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; },
		Cardinal::Infinity());
	auto *insInf = inf->InsertAt(0, 500);
	auto *lazyInsInf = dynamic_cast<LazySequence<int> *>(insInf);
	Assert(lazyInsInf && lazyInsInf->GetCardinalLength().IsInfinite(),
		   "Insert: Infinite remains Infinite");
	Assert(insInf->Get(500) == 0, "Insert: Inserted element at index 500");
	delete inf;
	delete insInf;
	std::cout << "\n";
}

void TestLazyMapWhereReduce() {
	PrintHeader("LAZY MAP / WHERE / REDUCE TESTS");
	auto *fib = new LazySequence<int>(
		[](Sequence<int> *p) {
			return p->GetLength() < 2 ? 1
									  : p->Get(p->GetLength() - 1) +
											p->Get(p->GetLength() - 2);
		},
		Cardinal::Infinity());
	auto *doubled = fib->Map<int>([](int x) { return x * 2; });
	auto *lazyDoubled = dynamic_cast<LazySequence<int> *>(doubled);
	Assert(lazyDoubled && lazyDoubled->GetMaterializedCount() <= 1,
		   "Map: Lazy evaluation (low materialization)");
	Assert(doubled->Get(5) == 16, "Map: double");
	delete doubled;
	fib->Get(20);

	auto *filtered = fib->Where([](int x) { return x % 2 == 0; });
	Assert(filtered->Get(0) > 0 && filtered->GetCount() > 0,
		   "Where: filter even");
	delete filtered;
	delete fib;
	std::cout << "\n";
}

void TestOrdinalIndexAccess() {
	PrintHeader("ORDINAL INDEX ACCESS TESTS");

	auto *seqA = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; },
		Cardinal::Infinity());
	auto *seqB = new LazySequence<int>(
		[](Sequence<int> *p) { return 1000 + p->GetLength(); },
		Cardinal::Infinity());
	auto *concat = seqA->Concat(*seqB);

	Assert(concat->Get(5) == 6, "Get(5) from first infinity");

	auto *lazyConcat = dynamic_cast<LazySequence<int> *>(concat);
	if (lazyConcat) {
		Assert(lazyConcat->Get(OrdinalIndex(0, 5)) == 6,
			   "Ordinal: Block 0, offset 5");
		Assert(lazyConcat->Get(OrdinalIndex(1, 0)) == 1000,
			   "Ordinal: Block 1, offset 0");
		Assert(lazyConcat->Get(OrdinalIndex(1, 15)) == 1015,
			   "Ordinal: Block 1, offset 15");
	} else {
		Assert(false, "Ordinal: Cast failed");
	}

	delete seqA;
	delete seqB;
	delete concat;
	std::cout << "\n";
}

void TestPerformance() {
	PrintHeader("PERFORMANCE TESTS (Large Data)");
	const int SIZE = 50000;
	std::cout << "  Testing with " << SIZE << " elements...\n";
	auto start = std::chrono::high_resolution_clock::now();
	auto *lazy = new LazySequence<int>(
		[](Sequence<int> *prev) -> int { return prev->GetLength() + 1; },
		Cardinal(SIZE));
	volatile int dummy = lazy->Get(SIZE - 1);
	(void)dummy;
	auto end = std::chrono::high_resolution_clock::now();
	auto duration =
		std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "  Time: " << duration.count() << " ms\n";
	Assert(lazy->GetMaterializedCount() == (size_t)SIZE,
		   "Full materialization");
	Assert(duration.count() < 10000, "Performance < 10 sec");
	delete lazy;
}
void TestCircularBuffer() {
	PrintHeader("CIRCULAR BUFFER TESTS");
	CircularBuffer<int> buf(3);
	buf.Append(10);
	buf.Append(20);
	buf.Append(30);
	Assert(buf.GetLength() == 3, "Length after 3 appends");
	Assert(buf.Get(0) == 10, "Get[0]");
	Assert(buf.Get(2) == 30, "Get[2]");

	buf.Append(40);
	Assert(buf.Get(0) == 20, "Rollover: Get[0] == 20");
	Assert(buf.Get(2) == 40, "Rollover: Get[2] == 40");

	buf.Clear();
	Assert(buf.IsEmpty(), "Clear -> IsEmpty");
	Assert(buf.GetLength() == 0, "Clear -> Length 0");
	std::cout << "\n";
}

void TestLazySequenceExtended() {
	PrintHeader("LAZY SEQUENCE EXTENDED TESTS");
	auto *finite = new LazySequence<int>(
		[](Sequence<int> *p) { return p->GetLength() + 1; }, Cardinal(5));
	Assert(finite->GetFirst() == 1, "GetFirst");
	Assert(finite->GetLast() == 5, "GetLast");

	try {
		finite->Get(-1);
		Assert(false, "Get(-1) exception");
	} catch (...) {
		Assert(true, "Get(-1) throws");
	}

	Assert(finite->TryGet(10).IsNone(), "TryGet out of bounds -> None");
	Assert(finite->TryGet(2).IsSome() && finite->TryGet(2).GetValue() == 3,
		   "TryGet valid -> Some");

	auto *cloned = finite->Clone();
	Assert(cloned->Get(2) == 3, "Clone preserves data");

	auto *sub = finite->GetSubsequence(1, 3);
	Assert(sub->Get(0) == 2 && sub->Get(2) == 4, "GetSubsequence");
	Assert(finite->operator[](4) == 5, "operator[]");

	auto *appended = finite->Append(99);
	Assert(appended->Get(5) == 99, "Append");

	auto reduced =
		finite->Reduce([](int acc, int val) { return acc + val; }, 0);
	Assert(reduced == 15, "Reduce sum 1..5");

	delete finite;
	delete cloned;
	delete sub;
	delete appended;
	std::cout << "\n";
}

void TestAlgorithmsExtended() {
	PrintHeader("ALGORITHMS EXTENDED TESTS");
	OnlineStatistics<int> stats;
	stats.Add(5);
	stats.Add(1);
	stats.Add(9);
	Assert(stats.GetMin() == 1, "Min");
	Assert(stats.GetMax() == 9, "Max");
	Assert(stats.GetMedian() == 5, "Median");

	stats.Reset();
	Assert(stats.GetCount() == 0, "Reset count");

	try {
		stats.GetMean();
		Assert(false, "Mean on empty");
	} catch (...) {
		Assert(true, "Mean on empty throws");
	}

	int arr[] = {8, 3, 5, 1, 9};
	auto *seq = new MutableArraySequence<int>(arr, 5);
	auto *stream = new SequenceReadStream<int>(seq);
	auto *sorted = StreamSorter<int>([](int a, int b) {
					   return a < b;
				   }).SortWithHeap(stream);
	sorted->Open();
	bool sortedCorrect = true;
	int last = 0;
	bool isFirst = true;

	while (!sorted->IsEndOfStream()) {
		int cur = sorted->Read();
		if (!isFirst && cur < last) {
			sortedCorrect = false;
			break;
		}
		last = cur;
		isFirst = false;
	}
	sorted->Close();

	Assert(sortedCorrect, "StreamSorter heap sort");

	delete sorted;
	delete stream;
	delete seq;
	std::cout << "\n";
}

void TestStreamsExtended() {
	PrintHeader("STREAMS EXTENDED TESTS");
	int data[] = {1, 2, 3};
	auto *seq = new MutableArraySequence<int>(data, 3);
	auto *writeStream = new SequenceWriteStream<int>(seq);
	writeStream->Open();
	writeStream->Write(4);
	writeStream->Write(5);
	writeStream->Close();
	Assert(seq->GetLength() == 5 && seq->Get(3) == 4, "WriteOnlyStream append");

	auto *readStream = new SequenceReadStream<int>(seq);
	readStream->Open();
	readStream->Read();
	readStream->Read();
	Assert(readStream->GetPosition() == 2, "GetPosition");
	readStream->Seek(0);
	Assert(readStream->GetPosition() == 0, "Seek");
	readStream->Close();

	delete writeStream;
	delete readStream;
	delete seq;
	std::cout << "\n";
}

void TestStringStream() {
	PrintHeader("STRING READ STREAM TESTS");
	std::string data = "10 20 30 40 50";
	auto deserializer = [](const std::string &s) -> int {
		return std::stoi(s);
	};
	auto *stream = new StringReadStream<int>(data, deserializer);

	stream->Open();
	Assert(!stream->IsEndOfStream(), "StringStream: Not at end initially");
	Assert(stream->Read() == 10, "StringStream: Read[0] == 10");
	Assert(stream->Read() == 20, "StringStream: Read[1] == 20");

	stream->Read();
	stream->Read();
	stream->Read();

	Assert(stream->GetPosition() == 5,
		   "StringStream: Position == 5 after full read");

	try {
		stream->Read();
		Assert(false, "StringStream: Read past end should throw");
	} catch (const InvalidOperationException &e) {
		Assert(true, "StringStream: Exception on EOF caught correctly");
	} catch (...) {
		Assert(false, "StringStream: Unknown exception type");
	}

	stream->Close();
	delete stream;
	std::cout << "\n";
}

void TestStreamPipeline() {
	PrintHeader("STREAM PIPELINE TESTS");

	auto *srcSeq = new MutableArraySequence<int>();
	for (int i = 1; i <= 5; ++i)
		srcSeq->Append(i * 10);

	auto *readStream = new SequenceReadStream<int>(srcSeq);
	auto *targetSeq = new MutableArraySequence<int>();
	auto *writeStream = new SequenceWriteStream<int>(targetSeq);

	readStream->Open();
	writeStream->Open();
	size_t copied1 = CopyStream(readStream, writeStream);
	readStream->Close();
	writeStream->Close();

	Assert(copied1 == 5, "Pipeline: Seq->Seq count matches");
	Assert(targetSeq->GetLength() == 5, "Pipeline: Seq->Seq length matches");
	Assert(targetSeq->Get(2) == 30, "Pipeline: Seq->Seq data integrity");
	delete readStream;
	delete writeStream;
	delete srcSeq;
	delete targetSeq;

	std::string testFile = "lab4_pipeline_test.dat";
	auto *seqForFile = new MutableArraySequence<int>();
	seqForFile->Append(100);
	seqForFile->Append(200);
	seqForFile->Append(300);

	auto *rs = new SequenceReadStream<int>(seqForFile);
	auto *ws = new FileWriteStream<int>(
		testFile, [](int x) { return std::to_string(x); });
	rs->Open();
	ws->Open();
	size_t written = CopyStream(rs, ws);
	rs->Close();
	ws->Close();
	delete rs;
	delete ws;
	delete seqForFile;

	Assert(written == 3, "Pipeline: Seq->File write count");

	auto *fileSeq = new MutableArraySequence<int>();

	ws = new FileWriteStream<int>(testFile,
								  [](int x) { return std::to_string(x); });
	ws->Open();
	ws->Write(100);
	ws->Write(200);
	ws->Write(300);
	ws->Close();
	delete ws;

	auto *fs = new FileStream(testFile, [](const std::string &s) { return s; });
	fs->Open();
	while (!fs->IsEndOfStream()) {
		std::string line = fs->Read();
		if (!line.empty()) {
			fileSeq->Append(std::stoi(line));
		}
	}
	fs->Close();
	delete fs;

	Assert(fileSeq->GetLength() == 3, "Pipeline: File->Seq read count");
	Assert(fileSeq->Get(0) == 100, "Pipeline: File->Seq value 1");
	Assert(fileSeq->Get(2) == 300, "Pipeline: File->Seq value 3");

	delete fileSeq;
	std::remove(testFile.c_str());
}
int RunAllTests() {
	std::cout
		<< "\n╔═══════════════════════════════════════════════════════════╗\n";
	std::cout
		<< "║              LAB 4 — AUTOMATED TESTS                      ║\n";
	std::cout
		<< "╚═══════════════════════════════════════════════════════════╝\n";

	TestCardinal();
	TestLazySequence();
	TestGenerator();
	TestStreams();
	TestStringStream();
	TestAlgorithms();
	TestConcatScenarios();
	TestInsertOperations();
	TestLazyMapWhereReduce();
	TestOrdinalIndexAccess();
	TestPerformance();
	TestCircularBuffer();
	TestLazySequenceExtended();
	TestAlgorithmsExtended();
	TestStreamsExtended();
	TestStreamPipeline();

	std::cout
		<< "\n╔═══════════════════════════════════════════════════════════╗\n";
	std::cout
		<< "║                  ALL TESTS COMPLETED                      ║\n";
	std::cout
		<< "╚═══════════════════════════════════════════════════════════╝\n";
	return 0;
}

int main() { return RunAllTests(); }