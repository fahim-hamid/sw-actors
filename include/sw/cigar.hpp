#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

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

inline void append_cigar(
    std::span<CigarOp> storage,
    std::size_t& size,
    CigarCode code,
    std::size_t length = 1) noexcept
{
    if (length == 0)
        return;

    if (size != 0 && storage[size - 1].code == code) {
        storage[size - 1].length += length;
        return;
    }

    assert(size < storage.size());
    storage[size++] = CigarOp{
        .length = length,
        .code = code,
    };
}

}
