#ifndef TSAFEQUEUE_H
#define TSAFEQUEUE_H

#include <queue>
#include <mutex>
#include <stdexcept>
#include <condition_variable>

template<typename T>
class TSafeQueue
{
public:

	void push(T el) {
		{
			std::lock_guard<std::mutex> guard{ m_mutex };
			if (m_done) throw std::runtime_error{ "Insertion completed, no new element can be inserted" };
			m_queue.push(std::move(el));
		}
		m_dataChanged.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lock{ m_mutex };
		m_dataChanged.wait(lock, [this] {return !m_queue.empty() || m_done;  });
		if(m_queue.empty())
			throw std::runtime_error{ "Insertion completed, no new element available" };
		auto el = m_queue.front();
		m_queue.pop();
		return el;
	}

	bool isDone() const{
		std::lock_guard<std::mutex> guard{ m_mutex };
		return m_done;
	}

	void done() {
		{
			std::lock_guard<std::mutex> guard{ m_mutex };
			m_done = true;
		}
		m_dataChanged.notify_all();
	}

private:
	std::queue<T> m_queue;
	std::condition_variable m_dataChanged;
	mutable std::mutex m_mutex;
	bool m_done{false};
};

#endif