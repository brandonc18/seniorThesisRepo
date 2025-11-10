#include <cstdint>
using U64 = uint64_t;

class Bitboard {
public:
	// Construct Bitboard at 0 or a given value
	Bitboard(U64 init = 0ULL) : data(init) {}
	
	// Set a bit at square index
	void set_bit(int square) {
		data |= (1ULL << square);
	}

	// Clear a bit at square index
	void clear_bit(int square) {
		data &= ~(1ULL << square);
	}

	// Get bit value at square index
	bool get_bit(int square) {
		return (data & (1ULL << square)) != 0;
	}

	// Pop the LSB and return its index
	int pop_lsb() {
		if (data == 0) return -1;
		int index = __builtin_ctzll(data);
		data &= data -1;
		return index;
	}

	// Doesn't edit, just reads
	int pop_lsb() const {
        if (data == 0) return -1;
        return __builtin_ctzll(data);
    }

	// Count set bits
	int count() {
		return __builtin_popcountll(data);
	}

	// Overload bitwise operators for combining bitboards
    Bitboard operator|(const Bitboard& other) const {
        return Bitboard(data | other.data);
    }

    Bitboard operator&(const Bitboard& other) const {
        return Bitboard(data & other.data);
    }

    Bitboard operator~() const {
        return Bitboard(~data);
    }

    // Get the underlying data
    U64 raw() const { return data; }
    void set_raw(U64 val) { data = val; }
private:
	U64 data;
};
