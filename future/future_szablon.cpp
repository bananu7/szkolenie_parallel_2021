std::future<int> foo() {
	// create promise
	// get future from the promise
	// move promise to a thread that will fulfill it
}

int main() {
	auto f = foo();
	std::cout << f.get();
}