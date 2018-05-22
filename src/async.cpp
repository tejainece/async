//============================================================================
// Name        : async.cpp
// Author      : Ravi Teja Gudapati
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cinttypes>
#include <iostream>
#include <chrono>

using namespace std;

constexpr uint32_t microStackSize = 4096;

struct {
	uint8_t stack[microStackSize];
};

template<typename ValueType>
class Future;

template<typename ValueType>
class Promise {
	ValueType value;

public:
	Future<ValueType> future;

	void complete(ValueType value) {
		this->value = value;
	}
};

template<typename ValueType>
class Future {

};

class Duration {
	int64_t value;

public:
	Duration(uint64_t value): value(value) {}


};

class Timer {
public:
	typedef void TimerCallback(void);

private:
	std::chrono::microseconds start;

	TimerCallback callback;

public:
	const Duration duration;

	Timer(Duration duration, TimerCallback callback): duration(duration) {
		start = std::chrono::steady_clock::now();
	}

	bool isFinished() {

	}
};

Future<int> add(int a, int b) {
	Promise<int> promise;
	return promise.future;
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
