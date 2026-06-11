#pragma once

#include "actors/worker_actor.hpp"
#include "sw/sequence.hpp"

#include <caf/result.hpp>
#include <caf/type_list.hpp>
#include <caf/typed_actor.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace sw::actors {

struct RegisterWorker {
    WorkerActor worker;
};

struct StartPipeline {};

struct ServerActorTrait {
    using signatures = caf::type_list<
        caf::result<void>(RegisterWorker),
        caf::result<void>(StartPipeline),
        caf::result<void>(Shutdown)>;
};

using ServerActor = caf::typed_actor<ServerActorTrait>;

template <class Inspector>
bool inspect(Inspector& inspector, RegisterWorker& message)
{
    return inspector.object(message).fields(
        inspector.field("worker", message.worker));
}

struct ServerOptions {
    ScoreConfig scoring;
    std::string output_file;
    std::size_t maximum_pairs = 0;
    std::size_t maximum_query_length = 0;
    std::size_t maximum_subject_length = 0;
};

ServerActor spawn_server(
    caf::actor_system& system,
    ServerOptions options,
    std::vector<Sequence> queries,
    std::vector<Sequence> subjects);

}
