#pragma once

#include <string>
#include <string_view>

namespace sw
{

using SequenceView = std::string_view;

struct Sequence
{
    std::string id;
    std::string data;

    [[nodiscard]] SequenceView view() const noexcept
    {
        return data;
    }
};

} // namespace sw
