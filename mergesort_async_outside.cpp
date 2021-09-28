#include <vector>
#include <iostream>
#include <future>

std::future<std::vector<int>> mergesort(std::vector<int> v) {
	if (v.size() <= 1)
		return std::async(std::launch::async, [v = std::move(v)]{ return v; });

	std::vector<int> left(v.begin(), v.begin() + v.size() / 2);
	std::vector<int> right(v.begin() + v.size() / 2, v.end());

	auto leftSortedF =  mergesort(std::move(left));
	auto rightSortedF = mergesort(std::move(right));

	return std::async(std::launch::async,
		[leftSortedF = std::move(leftSortedF), rightSortedF = std::move(rightSortedF)]
	{
		// get called inside async, doesn't block the calling thread
		auto leftSorted = leftSortedF.get();
		auto rightSorted = leftSortedF.get();

		std::vector<int> result;
		auto itL = leftSorted.begin();
		auto itR = rightSorted.begin();

		while (itL != leftSorted.end() or itR != rightSorted.end()) {
			if (
				(itL != leftSorted.end())
				and
				(
					(itR == rightSorted.end())
					or
					(*itL < *itR)
					)
				) {
				result.push_back(*itL);
				itL++;
			}
			else {
				result.push_back(*itR);
				itR++;
			}
		}

		return result;
	});
}

int main() {
	std::vector<int> v{ 4,6,7,1,3,8,9 };
	auto u = mergesort(v).get();
	for (int i : u) {
		std::cout << i << ' ';
	}
}