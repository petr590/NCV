#if !defined(NDEBUG) && !defined(NCV_DEBUG_H)
#define NCV_DEBUG_H

#include <fstream>
#include <vector>
#include <set>
#include <map>

namespace ncv {
	using std::ostream;
	using std::endl;
	using std::hex;
	using std::dec;

	extern std::ofstream log;

	static inline uint64_t millis() {
		return clock() * 1000 / CLOCKS_PER_SEC;
	}


	namespace debug {
		template<typename Iterator>
		ostream& outIterators(ostream& out, Iterator begin, Iterator end,
							ostream&(*writer)(ostream&, const Iterator&)) {
			if (begin == end) {
				return out;
			}

			for (--end; begin != end; ++begin) {
				writer(out, begin) << ", ";
			}

			return writer(out, end);
		}

		template<typename Iterator>
		ostream& outIter(ostream& output, const Iterator& it) {
			return output << *it;
		}


		template<typename Iterator>
		ostream& outIterators(ostream& out, Iterator begin, Iterator end) {
			return outIterators(out, begin, end, outIter);
		}
	}


	template<typename T>
	ostream& operator<<(ostream& out, const std::vector<T>& v) {
		return debug::outIterators(out << '[', v.begin(), v.end()) << ']';
	}


	template<typename T>
	ostream& operator<<(ostream& out, const std::set<T>& s) {
		return debug::outIterators(out << '{', s.begin(), s.end()) << '}';
	}


	template<typename K, typename V>
	ostream& operator<<(ostream& out, const std::map<K, V>& m) {
		// return debug::outIterators(out << '{', m.begin(), m.end(),
		// 		[] (ostream& output, const std::map<K, V>::const_iterator& it) { return output << it->first << " = " << it->second; }) << '}';
		return debug::outIterators(out << '{', m.begin(), m.end()) << '}';
	}


	template<typename K, typename V>
	ostream& operator<<(ostream& out, const std::pair<K, V>& p) {
		return out << '{' << p.first << ", " << p.second << '}';
	}
}

#endif /* !defined(NDEBUG) && !defined(NCV_DEBUG_H) */