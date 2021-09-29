#include <stack>
#include <mutex>

template<typename T>
class threadsafe_stack
{
private:
	std::stack<T> data;
	mutable std::mutex m;
public:
	threadsafe_stack() {}

	threadsafe_stack(const threadsafe_stack& other)
	{
		std::scoped_lock lock(other.m);
		data = other.data;
	}

	threadsafe_stack& operator=(const threadsafe_stack&) = delete;
	void push(T new_value)
	{
		std::scoped_lock lock(m);
		data.push(std::move(new_value));
	}

	template<typename = std::enable_if_t<!std::is_nothrow_move_constructible_v<T>>>
	std::unique_ptr<T> pop()
	{
		std::scoped_lock lock(m);
		if (data.empty()) {
			return nullptr;
			throw std::runtime_error("empty stack");
		}

		auto const v = std::make_unique(std::move(data.top()));
		data.pop();
		return v;
	}

	void pop(T& value)
	{
		std::scoped_lock lock(m);
		if (data.empty()) {
			throw std::runtime_error("empty stack");
		}

		value = std::move(data.top());
		data.pop();
	}

	template<typename = std::enable_if_t<std::is_nothrow_move_constructible_v<T>>>
	T pop() {
		std::scoped_lock lock(m);
		if (data.empty()) {
			throw std::runtime_error("empty stack");
		}

		T t(std::move(data.top()));
		data.pop();
		return t;
	}

	bool [[nodiscard]] empty() const
	{
		std::scoped_lock lock(m);
		return data.empty();
	}
};

int main() {
	threadsafe_stack<int> s;
	int a = s.pop();
}