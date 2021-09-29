#include <memory>
#include <mutex>
#include <optional>
#include <variant>
#include <iostream>

template<typename T>
class threadsafe_queue {
	struct node {
		std::optional<T> data; // nullopt if dummy node
		std::unique_ptr<node> next;
		node() {}
	};

	std::mutex headM;
	std::mutex lastM;

	std::condition_variable emptiness;

	std::unique_ptr<node> head;
	node* last;

public:
	threadsafe_queue()
		: head(std::make_unique<node>())
		, last(head.get())
	{
	}

	void push(T t) {
		auto n = std::make_unique<node>();
		{
			std::scoped_lock(lastM);
			last->data = std::move(t);
			last->next = std::move(n); // create new dummy node at the end
			last = last->next.get();
		}
		emptiness.notify_one();
	}

	std::optional<T> try_pop() {
		std::unique_lock headLock(headM);

			std::unique_lock lastLock(lastM);

				auto lastV = last;

			lastLock.unlock();

			if (head.get() == lastV)
				return std::nullopt;

			auto const old_head = std::move(head);
			head = std::move(old_head->next); //must use old_head as head is moved-from

		headLock.unlock();

		return old_head->data;
	}

	node* get_tail() {
		std::scoped_lock lastLock(lastM);
		auto lastV = last;
	}

	T wait_and_pop() {
		std::unique_lock headLock(headM);
		emptiness.wait(headLock, [&, this] { return head->get() != get_tail(); });

			auto const old_head = std::move(head);
			head = std::move(old_head->next); //must use old_head as head is moved-from

		headLock.unlock();

		return old_head->data;
	}

	bool empty() {
	}
};

int main() {
	threadsafe_queue<int> q;
	q.push(42);
	q.push(43);

	std::cout << q.try_pop().value_or(0) << '\n';
	std::cout << q.try_pop().value_or(0) << '\n';
	std::cout << q.try_pop().value_or(0) << '\n';
}