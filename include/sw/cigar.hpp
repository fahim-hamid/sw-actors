#pragma once

#include <cstddef>
#include <cstdint>

namespace sw {

enum class CigarCode : std::uint8_t {
    match,     // =
    mismatch,  // X

    // Relative to subject
    insertion, // I
    deletion,  // D

    soft_clip, // S
};

struct CigarOp {
    std::size_t length = 0;
    CigarCode code = CigarCode::match;
};

}
