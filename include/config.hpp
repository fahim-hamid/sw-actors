#pragma once

#include <caf/actor_system_config.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

namespace sw {

class AppConfig : public caf::actor_system_config {
public:
    std::uint16_t port = 0;
    std::string host = "localhost";
    bool distributed_mode = false;
    bool server_mode = false;

    std::string query_input;
    std::string subject_input;
    std::string output_file = "output.txt";

    std::size_t worker_count = 1;
    std::size_t maximum_sequences = 0;
    std::size_t maximum_pairs = 0;

    std::int32_t match_score = 2;
    std::int32_t mismatch_score = -1;
    std::int32_t gap_score = -2;

    AppConfig()
    {
        opt_group{custom_options_, "global"}
            .add(port, "port,p", "server port")
            .add(host, "host,H", "server host")
            .add(
                distributed_mode,
                "distributedMode,D",
                "enable distributed execution")
            .add(
                server_mode,
                "serverMode,s",
                "run the distributed server")
            .add(query_input, "query,Q", "query FASTA input")
            .add(subject_input, "subject,S", "subject FASTA input")
            .add(output_file, "output,O", "alignment output file")
            .add(
                worker_count,
                "actorNumber,A",
                "persistent workers in this process")
            .add(match_score, "match,m", "match score")
            .add(mismatch_score, "mismatch,M", "mismatch score")
            .add(gap_score, "gap,g", "gap score")
            .add(
                maximum_sequences,
                "max-sequences",
                "maximum FASTA records per input, 0 means unlimited")
            .add(
                maximum_pairs,
                "max-pairs",
                "maximum alignment pairs, 0 means unlimited");
    }
};

}
