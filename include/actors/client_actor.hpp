#pragma once

#include "actors/protocol.hpp"

#include <caf/result.hpp>
#include <caf/type_list.hpp>
#include <caf/typed_actor.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

namespace sw::actors {

struct ClientActorTrait {
    using signatures = caf::type_list<caf::result<void>(Shutdown)>;
};

using ClientActor = caf::typed_actor<ClientActorTrait>;

ClientActor spawn_client(
    caf::actor_system& system,
    std::string host,
    std::uint16_t port,
    std::size_t worker_count);

}
