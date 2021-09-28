#include <vector>
#include <iostream>

std::vector<int> mergesort(std::vector<int> v) {
	if (v.size() <= 1)
		return v;

	std::vector<int> left(v.begin(), v.begin() + v.size()/2);
	std::vector<int> right(v.begin() + v.size() / 2, v.end());

	auto leftSorted = mergesort(std::move(left));
	auto rightSorted = mergesort(std::move(right));

	std::vector<int> result;
	auto itL = leftSorted.begin();
	auto itR = rightSorted.begin();

	while (not (itL == leftSorted.end() and itR == rightSorted.end())) {
		if (
			(itL != leftSorted.end())
			&&
			(
				(itR == rightSorted.end())
				||
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
}

int main() {
	std::vector<int> v{ 4,6,7,1,3,8,9 };
	auto u = mergesort(v);
	for (int i : u) {
		std::cout << i << ' ';
	}
}