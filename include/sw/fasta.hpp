#pragma once

#include "sw/sequence.hpp"

#include <cstddef>
#include <expected>
#include <filesystem>
#include <istream>
#include <string>
#include <vector>

namespace sw
{

enum class FastaErrorCode
{
    open_failed,
    sequence_before_header,
    empty_identifier,
    empty_sequence,
    read_failed,
};

struct FastaError
{
    FastaErrorCode code;
    std::size_t line = 0;
    std::string message;
};

using FastaResult = std::expected<std::vector<Sequence>, FastaError>;

[[nodiscard]]
FastaResult read_fasta(std::istream &input, std::size_t max_sequences = 0);

[[nodiscard]]
FastaResult read_fasta(
    const std::filesystem::path &path,
    std::size_t max_sequences = 0);

} // namespace sw
