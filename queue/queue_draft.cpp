#include <thread>
#include <mutex>
#include <condition_variable>

template<class T>
class queue {
	std::mutex m;
	std::condition_variable emptiness;
public:
	void push(T t);
	bool empty();

	T wait_and_pop();
	std::optional<T> try_pop();
};

int main() {

}