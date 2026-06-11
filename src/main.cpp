#include "actors/type_ids.hpp"
#include "actors/client_actor.hpp"
#include "actors/server_actor.hpp"
#include "actors/worker_actor.hpp"
#include "config.hpp"
#include "sw/fasta.hpp"

#include <caf/anon_mail.hpp>
#include <caf/caf_main.hpp>
#include <caf/io/middleman.hpp>

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

std::size_t maximum_sequence_length(
    const std::vector<sw::Sequence>& sequences) noexcept
{
    std::size_t maximum_length = 0;

    for (const sw::Sequence& sequence : sequences)
        maximum_length = std::max(maximum_length, sequence.data.size());

    return maximum_length;
}

void register_local_workers(
    caf::actor_system& system,
    sw::actors::ServerActor server,
    std::size_t worker_count)
{
    for (std::size_t index = 0; index < worker_count; ++index) {
        sw::actors::WorkerActor worker =
            sw::actors::spawn_worker(system);
        caf::anon_mail(sw::actors::RegisterWorker{.worker = worker}).send(server);
    }
}

}

void caf_main(caf::actor_system& system, const sw::AppConfig& config)
{
    if (config.distributed_mode && !config.server_mode) {
        if (config.worker_count == 0) {
            system.println("A worker client requires at least one worker");
            return;
        }

        sw::actors::spawn_client(
            system,
            config.host,
            config.port,
            config.worker_count);
        return;
    }

    if (!config.distributed_mode && config.worker_count == 0) {
        system.println("Local execution requires at least one worker");
        return;
    }

    if (config.query_input.empty()) {
        system.println("A query FASTA input is required");
        return;
    }

    sw::FastaResult query_result =
        sw::read_fasta(config.query_input, config.maximum_sequences);
    if (!query_result) {
        system.println(
            "Failed to read query FASTA: {}",
            query_result.error().message);
        return;
    }

    std::vector<sw::Sequence> queries = std::move(*query_result);
    if (queries.empty()) {
        system.println("Query FASTA contains no records");
        return;
    }

    std::vector<sw::Sequence> subjects;
    if (!config.subject_input.empty()) {
        sw::FastaResult subject_result =
            sw::read_fasta(config.subject_input, config.maximum_sequences);
        if (!subject_result) {
            system.println(
                "Failed to read subject FASTA: {}",
                subject_result.error().message);
            return;
        }

        subjects = std::move(*subject_result);
        if (subjects.empty()) {
            system.println("Subject FASTA contains no records");
            return;
        }
    }

    const std::size_t maximum_query_length =
        maximum_sequence_length(queries);
    const std::size_t maximum_subject_length =
        subjects.empty()
            ? maximum_query_length
            : maximum_sequence_length(subjects);

    sw::actors::ServerOptions options{
        .scoring = sw::ScoreConfig{
            .match = config.match_score,
            .mismatch = config.mismatch_score,
            .gap = config.gap_score,
        },
        .output_file = config.output_file,
        .maximum_pairs = config.maximum_pairs,
        .maximum_query_length = maximum_query_length,
        .maximum_subject_length = maximum_subject_length,
    };

    sw::actors::ServerActor server = sw::actors::spawn_server(
        system,
        std::move(options),
        std::move(queries),
        std::move(subjects));

    if (config.distributed_mode) {
        caf::expected<std::uint16_t> published_port =
            system.middleman().publish(server, config.port);
        if (!published_port) {
            system.println(
                "Failed to publish server: {}",
                published_port.error());
            caf::anon_mail(sw::actors::Shutdown{}).send(server);
            return;
        }

        system.println("Server listening on port {}", *published_port);
    }

    register_local_workers(system, server, config.worker_count);
    caf::anon_mail(sw::actors::StartPipeline{}).send(server);
}

CAF_MAIN(caf::id_block::sw_actors, caf::io::middleman)
