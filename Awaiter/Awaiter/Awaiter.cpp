#include <thread>
#include <future>
#include <sstream>
#include <iostream>
#include <experimental/coroutine>
#include <experimental/generator>


struct Noisy {

	Noisy() { std::cout << "Noisy created - the coroutine frame has been created\n"; }

	~Noisy() { std::cout << "Noisy destroyed - the coroutine frame has been destroyed\n"; }
};

class Awaiter {
public:
	//Needed by Awaiter concept
	bool await_ready();
	void await_suspend(std::experimental::coroutine_handle<>);
	std::string await_resume();

	//my member function to resume
	void SetReady(std::string messageFromWorker);
private:
	std::experimental::coroutine_handle<> m_handle;
	std::string message;
};

void Awaiter::SetReady(std::string messageFromWorker)
{
	std::cout << "SET READY!\n";
	message = std::move(messageFromWorker);
	if (m_handle)
		m_handle.resume();
}

bool Awaiter::await_ready()
{
	return false;
}

void Awaiter::await_suspend(std::experimental::coroutine_handle<> handle)
{
	m_handle = std::move(handle);
}

std::string Awaiter::await_resume()
{
	std::cout << "I am DONE Waiting!\n";

	return  "DONE!!!!, message = " + message;
}

Awaiter awaiter;

std::future<std::string> coRoutine() {

	Noisy n{};
	std::cout << "coRoutine()\n";

	std::cout << "about to await!\n";

	std::string s = co_await awaiter;

	std::cout << "co await passed!\n";

	co_return s;
}

int main()
{
	std::thread t
	{
		[]()
		{
			std::cout << "About to sleep\n";
			std::this_thread::sleep_for(std::chrono::duration<int>(3));
			std::cout << "About to set ready\n";
			awaiter.SetReady("Message from Worker");
		}
	};


	std::cout << "About to call coRoutine()\n";
	std::future<std::string> a = coRoutine();
	std::cout << "Main do something longish\n";
	std::this_thread::sleep_for(std::chrono::duration<int>(1));
	std::cout << "Main done doing something longish, waiting for co-routine to be done\n";
	std::string s = a.get();
	t.join();
	std::cout << "after join, result = "<< s << "!\n";

}