//============================================================================
// Name        : async.cpp
// Author      : Ravi Teja Gudapati
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cinttypes>
#include <functional>
#include <iostream>
#include <chrono>
#include "core.hpp"

using namespace std;

typedef void (*VoidCallback)(void);

constexpr uint32_t microStackSize = 4096;

/// Structure to store a MicroTask's context
struct MicroTaskContext {
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbx;

	uint64_t rsp;
	uint64_t rbp;
	uint64_t ip;

	std::function<void()> initial;

	// TODO XMM6:XMM15

	// TODO
};

struct MicroTask;

void makeContext(MicroTask& task, std::function<void()> callback,
		VoidCallback wrapper);

extern "C" {
extern void changeContext(MicroTaskContext* oldTask, MicroTaskContext* newTask);
}

void wrapper(void);

struct MicroTask {
private:
	/// Stack of this MicroTask
	uint8_t stack[microStackSize];
	/// Saved context of this MicroTask
	MicroTaskContext ctx;

	bool complete = false;

	static MicroTask schedulerTask;

	static Vector<MicroTask*, 10> tasks;

	static Iterator<MicroTask*> it;

	friend void makeContext(MicroTask& task, std::function<void()> callback,
			VoidCallback wrapper);

	friend void wrapper(void);

	MicroTask() {
	}

public:
	/// Schedules a new microtask
	static void schedule(std::function<void()> callback) {
		MicroTask& x = *new MicroTask();
		makeContext(x, callback, wrapper);
		tasks.add(&x);
	}

	/// Suspend the current task
	static void suspend(void) {
		changeContext(&it.current()->ctx, &schedulerTask.ctx);
	}

	static void scheduler() {
		it = tasks.iterator();
		while (tasks.isNotEmpty()) {
			if (it.current()->complete) {
				tasks.remove(it.current());
				if (tasks.isEmpty()) {
					break;
				}
				if (!it.moveNext())
					it = tasks.iterator();
			} else {
				it.moveNext();
			}
			changeContext(&schedulerTask.ctx, &it.current()->ctx);
		}

		exit(0);
	}
};

MicroTask MicroTask::schedulerTask { };

Vector<MicroTask*, 10> MicroTask::tasks { };

Iterator<MicroTask*> MicroTask::it { };

void wrapper(void) {
	MicroTask::it.current()->ctx.initial();
	MicroTask::it.current()->complete = true;
	MicroTask::suspend();
}

void makeContext(MicroTask& task, std::function<void()> callback,
		VoidCallback wrapper) {
	task.ctx.rbp = task.ctx.rsp = (uint64_t) &task.stack;
	task.ctx.ip = (uint64_t) wrapper;
	task.ctx.initial = callback;
}

/* TODO
 void changeContext(MicroTask& oldTask, MicroTask& newTask) {
 asm volatile (
 "movq %%rsp, %0 \n\t"
 "movq %%rbp, %1 \n\t"
 "movq %%r12, %2 \n\t"
 "movq %%r13, %3 \n\t"
 "movq %%r14, %4 \n\t"
 "movq %%r15, %5 \n\t"
 "movq %%rbx, %8 \n\t"
 // TODO "movl $xane_async_recover, %%ebx \n\t"
 : "=r" (oldTask.ctx.rsp),
 "=r" (oldTask.ctx.rbp),
 "=r" (oldTask.ctx.r12),
 "=r" (oldTask.ctx.r13),
 "=r" (oldTask.ctx.r14),
 "=r" (oldTask.ctx.r15),
 "=r" (oldTask.ctx.rdi),
 "=r" (oldTask.ctx.rsi),
 "=r" (oldTask.ctx.rbx),
 "=r" (newTask.ctx.ip)
 : // No Inputs
 : "rsi", "rdi"
 );

 // TODO XMM

 // TODO restore XMM

 asm volatile (
 "movq %2, %%r12 \n\t"
 "movq %3, %%r13 \n\t"
 "movq %4, %%r14 \n\t"
 "movq %5, %%r15 \n\t"
 "movq %8, %%rbx \n\t"
 "movq %9, %%r10 \n\t"
 "movq %0, %%rsp \n\t"
 "movq %1, %%rbp \n\t"
 "jmp *%%r10 \n\t"
 "xane_async_recover:"
 :	// No output parameters
 : "r" (newTask.ctx.rsp),
 "r" (newTask.ctx.rbp),
 "r" (newTask.ctx.r12),
 "r" (newTask.ctx.r13),
 "r" (newTask.ctx.r14),
 "r" (newTask.ctx.r15),
 "r" (newTask.ctx.rdi),
 "r" (newTask.ctx.rsi),
 "r" (newTask.ctx.rbx),
 "r" (newTask.ctx.ip)
 : "rsi", "rdi"
 );
 }
 */

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
	Duration(int64_t value) :
			value(value) {
	}

	Duration(const Duration& other) :
			value(other.value) {
	}

	Duration(Duration&& other) :
			value(other.value) {
	}

	Duration& operator=(const Duration& other) {
		value = other.value;
	}

	Duration& operator=(const Duration&& other) {
		value = other.value;
	}

	Duration operator+(Duration other) {
		return Duration(value + other.value);
	}

	Duration operator-(Duration other) {
		return Duration(value - other.value);
	}

	bool operator>(Duration other) {
		return value > other.value;
	}

	bool operator>=(Duration other) {
		return value >= other.value;
	}

	bool operator<=(Duration other) {
		return value <= other.value;
	}

	bool operator<(Duration other) {
		return value < other.value;
	}

	static Duration secs(uint64_t seconds) {
		return Duration(seconds * 1000 * 1000);
	}

	static Duration mins(uint64_t minutes) {
		return Duration(minutes * 60 * 1000 * 1000);
	}
};

class Timer {
public:

private:
	Duration start;

	VoidCallback callback;

	bool hasTriggered = false;

public:
	const Duration duration;

	Timer(Duration duration, VoidCallback callback) :
			start(
					std::chrono::steady_clock::now().time_since_epoch().count()
							/ 1000), callback(callback), duration(duration) {
		Duration start = this->start;
		MicroTask::schedule(
				[callback, duration, start]() {
					auto end = Duration(
							std::chrono::steady_clock::now().time_since_epoch().count()
							/ 1000);
					while((end - start) < duration) {
						MicroTask::suspend();
						end = Duration(
								std::chrono::steady_clock::now().time_since_epoch().count()
								/ 1000);
					}
					callback();
				});
	}

	bool isFinished() {
		auto end = Duration(
				std::chrono::steady_clock::now().time_since_epoch().count()
						/ 1000);
		return (end - start) > duration;
	}

	void run(void) {
		if (hasTriggered)
			return;
		if (!isFinished())
			return;
		hasTriggered = true;
		if (callback != nullptr)
			callback();
	}
};

Future<int> add(int a, int b) {
	Promise<int> promise;
	return promise.future;
}

void printTimeout(void) {
	cout << "Timer finished" << endl;
}

void xane_main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	Timer timer(Duration::secs(10), printTimeout);
}

int main() {
	MicroTask::schedule(xane_main);
	MicroTask::scheduler();
	return 0;
}
