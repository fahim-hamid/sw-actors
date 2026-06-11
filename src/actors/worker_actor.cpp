#include "actors/type_ids.hpp"
#include "actors/worker_actor.hpp"

#include "sw/smith_waterman.hpp"

#include <caf/typed_event_based_actor.hpp>

#include <cassert>
#include <string_view>

namespace sw::actors {
namespace {

struct WorkerState {
    ScoreConfig scoring;
    AlignmentScratch scratch;
    bool configured = false;
};

WorkerActor::behavior_type worker_actor(
    WorkerActor::stateful_pointer<WorkerState> self)
{
    return {
        [self](const WorkerConfig& config) {
            self->state().scoring = config.scoring;
            self->state().scratch.prepare(
                static_cast<std::size_t>(config.maximum_query_length),
                static_cast<std::size_t>(config.maximum_subject_length));
            self->state().configured = true;
        },
        [self](const AlignmentJob& job) -> CompletedAlignment {
            assert(self->state().configured);

            const std::string_view query = job.query;
            const std::string_view subject = job.subject;
            const AlignmentResult result = smith_waterman(
                query,
                subject,
                self->state().scoring,
                self->state().scratch);

            return CompletedAlignment{
                .id = job.id,
                .query_index = job.query_index,
                .subject_index = job.subject_index,
                .score = result.score,
                .query_begin = static_cast<std::uint64_t>(result.query_begin),
                .query_end = static_cast<std::uint64_t>(result.query_end),
                .subject_begin = static_cast<std::uint64_t>(result.subject_begin),
                .subject_end = static_cast<std::uint64_t>(result.subject_end),
                .edits = static_cast<std::uint64_t>(result.edits),
                .cigar = std::vector<CigarOp>(
                    result.cigar.begin(),
                    result.cigar.end()),
            };
        },
        [self](Shutdown) {
            self->quit();
        },
    };
}

}

WorkerActor spawn_worker(caf::actor_system& system)
{
    return system.spawn(worker_actor);
}

}
