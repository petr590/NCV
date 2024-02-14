#include "dynamic_bitset.cpp"
#include <iostream>
 
int main() {
	using namespace ncv;
	using namespace std;

	dynamic_bitset b(100);

	b[2] = 1;
	b[10] = 1;

	for (int i = 0; i < 100; ++i) {
		ASSERT((i == 2 || i == 10) ? b.at(i) : !b.at(i));
	}

	ASSERT(b.count() == 2);

	cout << b << endl;
}