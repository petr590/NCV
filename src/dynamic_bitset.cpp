#include "dynamic_bitset.h"
#include "ncv_assert.h"

namespace ncv {
	dynamic_bitset::dynamic_bitset(size_t size):
			data((size + SIZE - 1) / SIZE),
			len(size) {}

	size_t dynamic_bitset::count() const {
		size_t res = 0;

		for (size_t i = 0, size = data.size(); i < size; ++i) {
			entry e = data[i];

			while (e != 0) {
				res += e & 0x1;
				e >>= 1;
			}
		}

		return res;
	}


	dynamic_bitset::bit::bit(entry& value, uint8_t offset):
		value(value), offset(offset) {}

	dynamic_bitset::bit& dynamic_bitset::bit::operator=(bool b) {
		if (b) {
			value |= 1ull << offset;
		} else {
			value &= ~(1ull << offset);
		}

		return *this;
	}

	dynamic_bitset::bit::operator bool() {
		return static_cast<bool>((value >> offset) & 0x1);
	}
	


	bool dynamic_bitset::at(size_t index) const {
		ASSERT(static_cast<int64_t>(index) >= 0);
		ASSERT(index < len);
		return (data[index / SIZE] >> (index % SIZE)) & 0x1;
	}

	bool dynamic_bitset::operator[](size_t index) const {
		return at(index);
	}

	dynamic_bitset::bit dynamic_bitset::operator[](size_t index) {
		ASSERT(static_cast<int64_t>(index) >= 0);
		ASSERT(index < len);
		return bit(data[index / SIZE], index % SIZE);
	}

	std::ostream& operator<<(std::ostream& out, const dynamic_bitset& bitset) {
		using entry = dynamic_bitset::entry;
		const auto SIZE = dynamic_bitset::SIZE;

		const vector<entry>& data = bitset.data;

		for (size_t i = 0, l = data.size(); i < l; ) {
			entry e = data[i];

			int s = ++i == l ? (bitset.len - 1) % SIZE + 1 : SIZE;

			for (int j = 0; j < s; ++j) {
				out << static_cast<char>('0' + (e & 0x1));
				e >>= 1;
			}
		}

		return out;
	}
}