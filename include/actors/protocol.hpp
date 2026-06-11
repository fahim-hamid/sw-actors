#pragma once

#include "sw/alignment.hpp"

#include <cstdint>

namespace sw {

template <class Inspector>
bool inspect(Inspector& inspector, CigarCode& code)
{
    std::uint8_t value = static_cast<std::uint8_t>(code);
    const bool success = inspector.apply(value);
    if (!success)
        return false;

    if constexpr (Inspector::is_loading) {
        if (value > static_cast<std::uint8_t>(CigarCode::soft_clip))
            return false;

        code = static_cast<CigarCode>(value);
    }

    return true;
}

template <class Inspector>
bool inspect(Inspector& inspector, ScoreConfig& scoring)
{
    return inspector.object(scoring).fields(
        inspector.field("match", scoring.match),
        inspector.field("mismatch", scoring.mismatch),
        inspector.field("gap", scoring.gap));
}

template <class Inspector>
bool inspect(Inspector& inspector, CigarOp& operation)
{
    return inspector.object(operation).fields(
        inspector.field("length", operation.length),
        inspector.field("code", operation.code));
}

}

namespace sw::actors {

struct Shutdown {};

}
