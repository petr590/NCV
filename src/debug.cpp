#ifndef NCV_DEBUG_CPP
#define NCV_DEBUG_CPP

#include <fstream>
#include <vector>
#include <set>
#include <map>

namespace ncv {
	using std::ostream;
	using std::endl;
	using std::hex;
	using std::dec;

	std::ofstream log("log.txt");


	template<typename Iterator>
	ostream& outIterators(ostream& out, Iterator begin, Iterator end) {
		if (begin == end) {
			return out;
		}

		--end;

		for (; begin != end; ++begin) {
			out << *begin << ", ";
		}

		return out << *end;
	}


	template<typename T>
	ostream& operator<<(ostream& out, const std::vector<T>& v) {
		return outIterators(out << '[', v.begin(), v.end()) << ']';
	}


	template<typename T>
	ostream& operator<<(ostream& out, const std::set<T>& s) {
		return outIterators(out << '{', s.begin(), s.end()) << '}';
	}


	template<typename K, typename V>
	ostream& operator<<(ostream& out, const std::map<K, V>& m) {
		return outIterators(out << '{', m.begin(), m.end()) << '}';
	}


	template<typename K, typename V>
	ostream& operator<<(ostream& out, const std::pair<K, V>& p) {
		return out << '{' << p.first << ", " << p.second << '}';
	}
}

#endif