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
	}
	threadsafe_stack& operator=(const threadsafe_stack&) = delete;
	void push(T new_value)
	{
	}
	std::unique_ptr<T> pop()
	{
	}
	void pop(T& value)
	{
	}
	bool empty() const
	{
	}
};