#pragma once

#include "actors/protocol.hpp"
#include "actors/worker_actor.hpp"

#include <caf/cow_string.hpp>
#include <caf/result.hpp>
#include <caf/type_list.hpp>
#include <caf/typed_actor.hpp>

#include <string>

namespace sw::actors {

struct OutputRecord {
    caf::cow_string query_id;
    caf::cow_string subject_id;
    caf::cow_string query;
    CompletedAlignment alignment;
};

struct FinishOutput {};

struct OutputActorTrait {
    using signatures = caf::type_list<
        caf::result<void>(OutputRecord),
        caf::result<bool>(FinishOutput),
        caf::result<void>(Shutdown)>;
};

using OutputActor = caf::typed_actor<OutputActorTrait>;

template <class Inspector>
bool inspect(Inspector& inspector, OutputRecord& record)
{
    return inspector.object(record).fields(
        inspector.field("query-id", record.query_id),
        inspector.field("subject-id", record.subject_id),
        inspector.field("query", record.query),
        inspector.field("alignment", record.alignment));
}

OutputActor spawn_output_actor(
    caf::actor_system& system,
    std::string output_file);

}
