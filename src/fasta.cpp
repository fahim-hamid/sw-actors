#include "sw/fasta.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string_view>
#include <utility>

namespace sw
{
namespace
{

[[nodiscard]]
bool is_space(char value) noexcept
{
    return std::isspace(static_cast<unsigned char>(value)) != 0;
}

[[nodiscard]]
std::string_view trim(std::string_view value) noexcept
{
    while (!value.empty() && is_space(value.front()))
        value.remove_prefix(1);

    while (!value.empty() && is_space(value.back()))
        value.remove_suffix(1);

    return value;
}

[[nodiscard]]
std::string parse_identifier(std::string_view header)
{
    header = trim(header);
    const auto end = std::find_if(header.begin(), header.end(), is_space);
    return std::string(header.begin(), end);
}

[[nodiscard]]
FastaError make_error(
    FastaErrorCode code,
    std::size_t line,
    std::string message)
{
    return FastaError{
        .code = code,
        .line = line,
        .message = std::move(message),
    };
}

} // namespace

FastaResult read_fasta(std::istream &input, std::size_t max_sequences)
{
    std::vector<Sequence> sequences;
    std::string line;
    Sequence current;
    std::size_t line_number = 0;
    std::size_t header_line = 0;
    bool has_header = false;

    while (std::getline(input, line))
    {
        ++line_number;
        const auto content = trim(line);
        if (content.empty())
            continue;

        if (content.front() == '>')
        {
            if (has_header)
            {
                if (current.data.empty())
                {
                    return std::unexpected(make_error(
                        FastaErrorCode::empty_sequence,
                        header_line,
                        "FASTA record '" + current.id + "' has no sequence data"));
                }

                sequences.push_back(std::move(current));
                current = {};

                if (max_sequences != 0 && sequences.size() >= max_sequences)
                    return sequences;
            }

            current.id = parse_identifier(content.substr(1));
            if (current.id.empty())
            {
                return std::unexpected(make_error(
                    FastaErrorCode::empty_identifier,
                    line_number,
                    "FASTA header has no identifier"));
            }

            header_line = line_number;
            has_header = true;
            continue;
        }

        if (!has_header)
        {
            return std::unexpected(make_error(
                FastaErrorCode::sequence_before_header,
                line_number,
                "FASTA sequence data appears before the first header"));
        }

        for (const char value : content)
        {
            if (!is_space(value))
                current.data.push_back(value);
        }
    }

    if (input.bad())
    {
        return std::unexpected(make_error(
            FastaErrorCode::read_failed,
            line_number,
            "failed while reading FASTA input"));
    }

    if (has_header)
    {
        if (current.data.empty())
        {
            return std::unexpected(make_error(
                FastaErrorCode::empty_sequence,
                header_line,
                "FASTA record '" + current.id + "' has no sequence data"));
        }

        sequences.push_back(std::move(current));
    }

    return sequences;
}

FastaResult read_fasta(
    const std::filesystem::path &path,
    std::size_t max_sequences)
{
    std::ifstream input(path);
    if (!input)
    {
        return std::unexpected(make_error(
            FastaErrorCode::open_failed,
            0,
            "unable to open FASTA file '" + path.string() + "'"));
    }

    return read_fasta(input, max_sequences);
}

} // namespace sw
