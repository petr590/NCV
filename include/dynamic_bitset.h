#ifndef NCV_DYNAMIC_BITSET_H
#define NCV_DYNAMIC_BITSET_H

#include <vector>
#include <cstdint>
#include <ostream>

namespace ncv {
	using std::vector;

	class dynamic_bitset {
		typedef uint64_t entry;
		static constexpr size_t SIZE = sizeof(entry) * 8;

		vector<entry> data;
		size_t len;

	public:
		dynamic_bitset() = default;

		dynamic_bitset(size_t size);
		
		inline size_t size() const {
			return len;
		}

		size_t count() const;


		class bit {
			entry& value;
			const uint8_t offset;
			
		public:
			bit(entry& value, uint8_t offset);

			bit& operator=(bool b);

			operator bool();
		};
		

		bool at(size_t index) const;

		bool operator[](size_t index) const;

		bit operator[](size_t index);

		friend std::ostream& operator<<(std::ostream&, const dynamic_bitset&);
	};
}

#endif /* NCV_DYNAMIC_BITSET_H */