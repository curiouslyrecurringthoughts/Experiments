#ifndef WORKERQUEUE_H
#define WORKERQUEUE_H

#include <queue>
#include <mutex>
#include <optional>
#include <experimental/coroutine>

template<typename T>
class WorkerQueue
{
public:

	void push(T el) {
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_queue.push(std::move(el));
		coroHandle thisCoro{nullptr};
		if (m_coro){
			std::swap(thisCoro, m_coro);
		}

		lock.unlock();

		if(thisCoro)
			thisCoro.resume();
	}

	bool empty() const {
		std::lock_guard<std::mutex> guard{ m_mutex };
		return m_queue.empty();
	}

	T pop() {
		std::lock_guard<std::mutex> guard{ m_mutex };
		auto el = m_queue.front();
		m_queue.pop();
		return el;
	}

	using coroHandle = std::experimental::coroutine_handle<>;

	void wakeMeUp(coroHandle coro) {
		std::lock_guard<std::mutex> guard{ m_mutex };
		m_coro = coro;
	}
	

private:
	std::queue<T> m_queue;
	mutable std::mutex m_mutex;
	coroHandle m_coro{nullptr};
};

#endif