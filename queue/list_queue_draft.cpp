#include <memory>
#include <mutex>
#include <optional>
#include <condition_variable>

template<typename T>
class threadsafe_queue {
	struct node {
		T data;
		std::unique_ptr<node> next;
		node(T data) : data(std::move(data)) {}
	};

	std::mutex headMutex;
	std::mutex lastMutex;
	std::condition_variable emptiness;

	std::unique_ptr<node> head;
	node* last;

public:
	threadsafe_queue()
		: last(nullptr)
	{ }

	void push(T t) {
	}

	std::optional<T> try_pop() {
	}

	T wait_and_pop() {
	}
};

int main() {
	threadsafe_queue<int> q;
}