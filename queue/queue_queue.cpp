#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <utility>

template<class T>
class queue {
	std::mutex m;
	std::condition_variable emptiness;
	std::queue<T> q;
public:
	void push(T t) {
		{
			std::scoped_lock lock(m);
			q.push(std::move(t));
		}
		emptiness.notify_one();
	}

	T wait_and_pop() {
		std::unique_lock lock(m);
		emptiness.wait(lock, [this] { return not q.empty(); });

		auto const e = std::move_if_noexcept(q.front());
		q.pop();
		return e;
	}

	std::optional<T> try_pop() {
		std::scoped_lock lock(m);
		if (q.empty)
			return std::nullopt;
		else {
			auto const e = std::move_if_noexcept(q.front());
			q.pop();
			return e;
		}
	}

	bool empty() {
		std::scoped_lock lock(m);
		return q.empty();
	}
};

int main() {

}