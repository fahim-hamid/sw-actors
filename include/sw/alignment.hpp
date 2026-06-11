#pragma once

#include "sw/cigar.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace sw {

struct ScoreConfig {
    std::int32_t match = 2;
    std::int32_t mismatch = -1;
    std::int32_t gap = -2;
};

struct AlignmentResult {
    std::int32_t score = 0;

    // Half-open range of what we aligned
    // [query_begin, query_end)
    std::size_t query_begin = 0;
    std::size_t query_end = 0;

    // [subject_begin, subject_end)
    std::size_t subject_begin = 0;
    std::size_t subject_end = 0;

    std::size_t edits = 0;

    // Valid until the scratch storage is prepared or used for another alignment.
    std::span<const CigarOp> cigar;
};

}
