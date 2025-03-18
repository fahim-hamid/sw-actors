#include <filesystem>
#include <iostream>
#include <limits>
#include <mutex>
#include <ranges>
#include <thread>
#include <typeinfo>
#include <utility>
#include <vector>

#include <seqan3/alignment/configuration/align_config_method.hpp>
#include <seqan3/alignment/configuration/align_config_output.hpp>
#include <seqan3/alignment/configuration/align_config_parallel.hpp>
#include <seqan3/alignment/configuration/align_config_scoring_scheme.hpp>
#include <seqan3/alignment/configuration/align_config_vectorised.hpp>
#include <seqan3/alignment/configuration/align_config_on_result.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alignment/scoring/nucleotide_scoring_scheme.hpp>
#include <seqan3/alphabet/container/bitpacked_sequence.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/utility/views/pairwise_combine.hpp>

using namespace seqan3::literals;

struct my_traits : seqan3::sequence_file_input_default_traits_dna
{
    using sequence_alphabet = seqan3::dna4;
};

int main(int argc, char *argv[])
{
    std::filesystem::path fastaPath = "BRCA1.fasta";

    seqan3::sequence_file_input<my_traits> inputFile{fastaPath};
    std::vector<seqan3::dna4_vector> sequences{};
    for (auto &seq : inputFile)
    {
        sequences.push_back(std::move(seq.sequence()));
    }

    auto sequencePairs = seqan3::views::pairwise_combine(sequences);

    // Configure the alignment:
    // Output the score, alignment (traceback), begin, and end positions.
    auto output_config = seqan3::align_cfg::output_score{} 
                       | seqan3::align_cfg::output_alignment{}
                       | seqan3::align_cfg::output_begin_position{}
                       | seqan3::align_cfg::output_end_position{};

    // Set up a minimal configuration: local alignment with a scoring scheme and affine gap costs.
    auto alignment_cfg =
        seqan3::align_cfg::method_local{} |
        seqan3::align_cfg::scoring_scheme{
            seqan3::nucleotide_scoring_scheme{
                seqan3::match_score{2},
                seqan3::mismatch_score{-1}
            }
        } |
        seqan3::align_cfg::gap_cost_affine{
            seqan3::align_cfg::open_score{0},      // gap opening penalty
            seqan3::align_cfg::extension_score{-2}   // gap extension penalty
        } |
        output_config |
        seqan3::align_cfg::parallel{std::thread::hardware_concurrency()};


    std::mutex out_mutex;

    auto configWithCallback = alignment_cfg | 
        seqan3::align_cfg::on_result{[&](auto && result)
        {
            // Lock the mutex to ensure safe printing.
            std::lock_guard lock{out_mutex};
            auto && [aligned_seq1, aligned_seq2] = result.alignment();
            seqan3::debug_stream << "Score: " << result.score() << "\n"
                                << "Alignment: \n" 
                                << aligned_seq1 << "\n"
                                << aligned_seq2 << "\n\n";
        }};

    seqan3::align_pairwise(sequencePairs, configWithCallback);

    return 0;
}
