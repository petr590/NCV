#ifndef NCV_DYNAMIC_BITSET
#define NCV_DYNAMIC_BITSET

#include <vector>
#include <cstdint>
#include <cassert>

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

			for (entry entry : data) {
				while (entry != 0) {
					res += entry & 0x1;
					entry >>= 1;
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
		


		bit operator[](size_t index) {
			assert(static_cast<int64_t>(index) >= 0);
			assert(index < len);
			return bit(data[index / sizeof(entry)], index % sizeof(entry));
		}

		bool operator[](size_t index) const {
			assert(static_cast<int64_t>(index) >= 0);
			assert(index < len);
			return data[index / sizeof(entry)] << (index % sizeof(entry));
		}
	};
}

#endif