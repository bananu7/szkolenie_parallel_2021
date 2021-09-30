#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <format>
using namespace std::chrono_literals;

template<typename T>
class lock_free_stack
{
private:
	struct node
	{
		T data;
		node* next;
		node(T data) : data(data)
		{}
	};
	std::atomic<node*> head;
public:
	void push(T data)
	{
	}
};

int main() {

}