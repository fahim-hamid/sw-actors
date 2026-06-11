#pragma once

#include "sw/alignment.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace sw {

struct AlignmentScratch {
    std::vector<std::int32_t> previous_row;
    std::vector<std::int32_t> current_row;
    std::vector<std::uint8_t> trace;
    std::vector<CigarOp> cigar;

    void prepare(
        std::size_t maximum_query_length,
        std::size_t maximum_subject_length);
};

AlignmentResult smith_waterman(
    std::string_view query,
    std::string_view subject,
    ScoreConfig scoring,
    AlignmentScratch& scratch) noexcept;

}
