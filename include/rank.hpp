#ifndef RANK_H_
#define RANK_H_

#include "bitvector.hpp"

#include <assert.h>

#include <vector>

#include "popcount.h"

namespace surf {

class BitVectorRank : public BitVector {
public:
    BitVectorRank() : basic_block_size_(0), rank_lut_(NULL) {};

    BitVectorRank(const position_t basic_block_size, 
		  const std::vector<std::vector<word_t> >& bitvector_per_level, 
		  const std::vector<position_t>& num_bits_per_level)
	: BitVector(bitvector_per_level, num_bits_per_level) {
	basic_block_size_ = basic_block_size;
	initRankLut();
    }

    BitVectorRank(const position_t basic_block_size, 
		  const std::vector<std::vector<word_t> >& bitvector_per_level, 
		  const std::vector<position_t>& num_bits_per_level,
		  const level_t start_level,
		  const level_t end_level/* non-inclusive */) 
	: BitVector(bitvector_per_level, num_bits_per_level, start_level, end_level) {
	basic_block_size_ = basic_block_size;
	initRankLut();
    }

    ~BitVectorRank() {
	delete[] rank_lut_;
    }

    //TODO: change to zero-based
    position_t rank(position_t pos) {
        assert(pos <= num_bits_);
        position_t word_per_basic_block = basic_block_size_ / kWordSize;
        position_t block_id = pos / basic_block_size_;
        position_t offset = pos & (basic_block_size_ - 1);
        return (rank_lut_[block_id] 
		+ popcountLinear(bits_, block_id * word_per_basic_block, offset));
    }

    position_t size() {
        position_t bitvector_mem = num_bits_ / 8;
        position_t rank_lut_mem = num_bits_ / basic_block_size_ * sizeof(uint32_t);
        return (sizeof(BitVectorRank) + bitvector_mem + rank_lut_mem);
    }

private:
    void initRankLut() {
        position_t word_per_basic_block = basic_block_size_ / kWordSize;
        position_t num_blocks = num_bits_ / basic_block_size_;
	rank_lut_ = new position_t[num_blocks];

        position_t cumu_rank = 0;
        for (position_t i = 0; i < num_blocks; i++) {
            rank_lut_[i] = cumu_rank;
            cumu_rank += popcountLinear(bits_, i * word_per_basic_block, basic_block_size_);
        }
    }

    position_t basic_block_size_;
    position_t* rank_lut_; //rank look-up table
};

} // namespace surf

#endif // RANK_H_