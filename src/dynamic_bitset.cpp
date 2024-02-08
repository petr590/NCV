#ifndef NCV_DYNAMIC_BITSET_CPP
#define NCV_DYNAMIC_BITSET_CPP

#include "ncv_assert.h"
#include <vector>
#include <cstdint>

namespace ncv {
	using std::vector;

	class dynamic_bitset {
		typedef uint64_t entry;
		static constexpr size_t SIZE = sizeof(entry);

		vector<entry> data;
		size_t len;

	public:
		dynamic_bitset() {}

		dynamic_bitset(size_t size):
			data((size + SIZE - 1) / SIZE),
			len(size) {}
		
		inline size_t size() const {
			return len;
		}

		size_t count() const {
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


		class bit {
			entry& value;
			const uint8_t offset;
			
		public:
			bit(entry& value, uint8_t offset):
				value(value), offset(offset) {}

			bit& operator=(bool b) {
				if (b) {
					value |= 1ull << offset;
				} else {
					value &= ~(1ull << offset);
				}

				return *this;
			}

			operator bool() {
				return static_cast<bool>(value >> offset);
			}
		};
		


		bool at(size_t index) const {
			ASSERT(static_cast<int64_t>(index) >= 0);
			ASSERT(index < len);
			return data[index / sizeof(entry)] << (index % sizeof(entry));
		}

		inline bool operator[](size_t index) const {
			return at(index);
		}

		bit operator[](size_t index) {
			ASSERT(static_cast<int64_t>(index) >= 0);
			ASSERT(index < len);
			return bit(data[index / sizeof(entry)], index % sizeof(entry));
		}
	};
}

#endif /* NCV_DYNAMIC_BITSET_CPP */