#include <iostream>
#include <string>
#include <future>
#include <queue>
#include <utility>
#include <experimental/coroutine>

class Worker {

public:

	static Worker& getInstance() {
		static Worker worker;
		return worker;
	}

	using coroHandle = std::experimental::coroutine_handle<>;

	void suspend(coroHandle handle, int inputData) {
		m_suspendedCoros.push({ handle, inputData });
	}

	int veryComplexDataGeneration(int inputData) { return std::rand() % 10 + inputData; }

	void run() {
		unsigned int i = 0;
		while (!m_suspendedCoros.empty()) {
			auto [coro, inputData] = m_suspendedCoros.front();
			m_suspendedCoros.pop();
			outputData = veryComplexDataGeneration(inputData);
			coro.resume();
		}
	}

	int getData() const { return outputData; }

private:

	std::queue<std::pair<coroHandle, int>> m_suspendedCoros;
	int outputData{ -1 };
};

class Awaiter {
public:

	Awaiter(int data) : data(data) {}

	bool await_ready()
	{
		return false;
	}

	void await_suspend(std::experimental::coroutine_handle<> handle)
	{
		Worker::getInstance().suspend(handle, data);
	}

	int await_resume()
	{
		return Worker::getInstance().getData();
	}

private:
	int data;
};

struct noisy {

	noisy(const std::string& name) : m_name{ name } { std::cout << "noisy for coroutine " << m_name << " created\n"; }
	~noisy() { { std::cout << "noisy for coroutine " << m_name << " destroyed\n"; } }

	std::string m_name;
};


struct task {

	struct promise_type;

	using handle = std::experimental::coroutine_handle<promise_type>;

	struct promise_type {

		auto get_return_object() { return task{ handle::from_promise(*this) }; }
		void return_value(int value) { m_value = value; }
		auto initial_suspend() { return std::experimental::suspend_never{}; }
		auto final_suspend() { return std::experimental::suspend_always{}; }

		int m_value{ 0 };

		//Part needed to think about lifetime, ignore
		static inline int s_instance_id = 0;
		noisy n{ "promise " + std::to_string(++s_instance_id) };
	};

	task(handle h) : coro(h) {}


	int get() {
		return coro.promise().m_value;
	}

	~task() {
		if (coro)
			coro.destroy();
	}

	task(task const&) = delete;
	task(task &&) = delete;

	task& operator=(task&&);
	task& operator=(task const&);

private:
	handle coro;
};

task coRoutine(std::string name) {

	int sum = 0;
	while (sum < 20) {
		int input_data = rand() % 5;
		const auto s = co_await Awaiter{ input_data };
		std::cout << name << " received " << s << '\n';
		sum += s;
	}
	co_return sum;
}


int main()
{
	{
		auto f1 = coRoutine("c1");
		auto f2 = coRoutine("c2");
		auto f3 = coRoutine("c3");

		Worker::getInstance().run();

		auto v1 = f1.get();
		std::cout << "Got result from 1\n";
		auto v2 = f2.get();
		std::cout << "Got result from 2\n";
		auto v3 = f3.get();
		std::cout << "Got result from 3\n";


		std::cout << "RESULTS " << v1 << " " << v2 << " " << v3 << '\n';
	}

	std::cout << "Coros should be destroyed now\n";
}
