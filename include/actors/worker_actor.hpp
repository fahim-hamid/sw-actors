#pragma once

#include "actors/protocol.hpp"

#include <caf/cow_string.hpp>
#include <caf/result.hpp>
#include <caf/type_list.hpp>
#include <caf/typed_actor.hpp>

#include <cstdint>
#include <vector>

namespace sw::actors {

struct WorkerConfig {
    ScoreConfig scoring;
    std::uint64_t maximum_query_length = 0;
    std::uint64_t maximum_subject_length = 0;
};

struct AlignmentJob {
    std::uint64_t id = 0;
    std::uint64_t query_index = 0;
    std::uint64_t subject_index = 0;
    caf::cow_string query;
    caf::cow_string subject;
};

struct CompletedAlignment {
    std::uint64_t id = 0;
    std::uint64_t query_index = 0;
    std::uint64_t subject_index = 0;
    std::int32_t score = 0;
    std::uint64_t query_begin = 0;
    std::uint64_t query_end = 0;
    std::uint64_t subject_begin = 0;
    std::uint64_t subject_end = 0;
    std::uint64_t edits = 0;
    std::vector<CigarOp> cigar;
};

struct WorkerActorTrait {
    using signatures = caf::type_list<
        caf::result<void>(WorkerConfig),
        caf::result<CompletedAlignment>(AlignmentJob),
        caf::result<void>(Shutdown)>;
};

using WorkerActor = caf::typed_actor<WorkerActorTrait>;

template <class Inspector>
bool inspect(Inspector& inspector, WorkerConfig& config)
{
    return inspector.object(config).fields(
        inspector.field("scoring", config.scoring),
        inspector.field("maximum-query-length", config.maximum_query_length),
        inspector.field("maximum-subject-length", config.maximum_subject_length));
}

template <class Inspector>
bool inspect(Inspector& inspector, AlignmentJob& job)
{
    return inspector.object(job).fields(
        inspector.field("id", job.id),
        inspector.field("query-index", job.query_index),
        inspector.field("subject-index", job.subject_index),
        inspector.field("query", job.query),
        inspector.field("subject", job.subject));
}

template <class Inspector>
bool inspect(Inspector& inspector, CompletedAlignment& alignment)
{
    return inspector.object(alignment).fields(
        inspector.field("id", alignment.id),
        inspector.field("query-index", alignment.query_index),
        inspector.field("subject-index", alignment.subject_index),
        inspector.field("score", alignment.score),
        inspector.field("query-begin", alignment.query_begin),
        inspector.field("query-end", alignment.query_end),
        inspector.field("subject-begin", alignment.subject_begin),
        inspector.field("subject-end", alignment.subject_end),
        inspector.field("edits", alignment.edits),
        inspector.field("cigar", alignment.cigar));
}

WorkerActor spawn_worker(caf::actor_system& system);

}
