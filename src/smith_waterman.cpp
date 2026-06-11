#include "sw/smith_waterman.hpp"

#include <algorithm>
#include <cassert>
#include <span>
#include <utility>

namespace sw {
namespace {

enum class TraceDirection : std::uint8_t {
    stop,
    diagonal,
    up,
    left,
};

void append_cigar(
    std::span<CigarOp> cigar,
    std::size_t& cigar_size,
    CigarCode code,
    std::size_t length = 1) noexcept
{
    if (length == 0)
        return;

    if (cigar_size != 0 && cigar[cigar_size - 1].code == code) {
        cigar[cigar_size - 1].length += length;
        return;
    }

    assert(cigar_size < cigar.size());
    cigar[cigar_size++] = CigarOp{
        .length = length,
        .code = code,
    };
}

AlignmentResult traceback(
    std::string_view query,
    std::string_view subject,
    std::span<const std::uint8_t> trace,
    std::span<CigarOp> cigar,
    std::size_t columns,
    std::size_t endpoint_row,
    std::size_t endpoint_column,
    std::int32_t score) noexcept
{
    std::size_t row = endpoint_row;
    std::size_t column = endpoint_column;
    std::size_t cigar_size = 0;
    std::size_t edits = 0;

    append_cigar(cigar, cigar_size, CigarCode::soft_clip, query.size() - row);

    while (row > 0 && column > 0) {
        const TraceDirection direction =
            static_cast<TraceDirection>(trace[row * columns + column]);
        if (direction == TraceDirection::stop)
            break;

        switch (direction) {
        case TraceDirection::diagonal:
            if (query[row - 1] == subject[column - 1]) {
                append_cigar(cigar, cigar_size, CigarCode::match);
            } else {
                append_cigar(cigar, cigar_size, CigarCode::mismatch);
                ++edits;
            }
            --row;
            --column;
            break;

        case TraceDirection::up:
            append_cigar(cigar, cigar_size, CigarCode::insertion);
            ++edits;
            --row;
            break;

        case TraceDirection::left:
            append_cigar(cigar, cigar_size, CigarCode::deletion);
            ++edits;
            --column;
            break;

        case TraceDirection::stop:
            break;
        }
    }

    append_cigar(cigar, cigar_size, CigarCode::soft_clip, row);
    std::ranges::reverse(cigar.first(cigar_size));

    return AlignmentResult{
        .score = score,
        .query_begin = row,
        .query_end = endpoint_row,
        .subject_begin = column,
        .subject_end = endpoint_column,
        .edits = edits,
        .cigar = cigar.first(cigar_size),
    };
}

}

void AlignmentScratch::prepare(
    std::size_t maximum_query_length,
    std::size_t maximum_subject_length)
{
    const std::size_t rows = maximum_query_length + 1;
    const std::size_t columns = maximum_subject_length + 1;

    previous_row.resize(columns);
    current_row.resize(columns);
    trace.resize(rows * columns);
    cigar.resize(maximum_query_length + maximum_subject_length + 2);
}

AlignmentResult smith_waterman(
    std::string_view query,
    std::string_view subject,
    ScoreConfig scoring,
    AlignmentScratch& scratch) noexcept
{
    const std::size_t rows = query.size() + 1;
    const std::size_t columns = subject.size() + 1;
    const std::size_t trace_size = rows * columns;
    const std::size_t maximum_cigar_size = query.size() + subject.size() + 2;

    assert(scratch.previous_row.size() >= columns);
    assert(scratch.current_row.size() >= columns);
    assert(scratch.trace.size() >= trace_size);
    assert(scratch.cigar.size() >= maximum_cigar_size);

    std::span<std::int32_t> previous =
        std::span{scratch.previous_row}.first(columns);
    std::span<std::int32_t> current =
        std::span{scratch.current_row}.first(columns);
    std::span<std::uint8_t> trace =
        std::span{scratch.trace}.first(trace_size);
    std::span<CigarOp> cigar =
        std::span{scratch.cigar}.first(maximum_cigar_size);

    std::ranges::fill(previous, std::int32_t{0});

    std::int32_t maximum_score = 0;
    std::size_t endpoint_row = 0;
    std::size_t endpoint_column = 0;

    for (std::size_t row = 1; row < rows; ++row) {
        current[0] = 0;
        const std::size_t trace_row = row * columns;

        for (std::size_t column = 1; column < columns; ++column) {
            const std::int32_t substitution =
                query[row - 1] == subject[column - 1]
                    ? scoring.match
                    : scoring.mismatch;

            const std::int32_t diagonal = previous[column - 1] + substitution;
            const std::int32_t up = previous[column] + scoring.gap;
            const std::int32_t left = current[column - 1] + scoring.gap;

            std::int32_t score = 0;
            TraceDirection direction = TraceDirection::stop;

            // Deterministic tie order: diagonal, up, then left.
            if (diagonal > score) {
                score = diagonal;
                direction = TraceDirection::diagonal;
            }
            if (up > score) {
                score = up;
                direction = TraceDirection::up;
            }
            if (left > score) {
                score = left;
                direction = TraceDirection::left;
            }

            current[column] = score;
            trace[trace_row + column] =
                static_cast<std::uint8_t>(direction);

            if (score > maximum_score) {
                maximum_score = score;
                endpoint_row = row;
                endpoint_column = column;
            }
        }

        std::swap(previous, current);
    }

    return traceback(
        query,
        subject,
        trace,
        cigar,
        columns,
        endpoint_row,
        endpoint_column,
        maximum_score);
}

}
