#include "actors/type_ids.hpp"
#include "actors/client_actor.hpp"

#include "actors/server_actor.hpp"
#include "actors/worker_actor.hpp"

#include <caf/io/middleman.hpp>
#include <caf/typed_event_based_actor.hpp>

#include <utility>
#include <vector>

namespace sw::actors {
namespace {

struct ClientState {
    ServerActor server;
    std::vector<WorkerActor> workers;
};

void shutdown_workers(ClientActor::stateful_pointer<ClientState> self)
{
    for (const WorkerActor& worker : self->state().workers)
        self->mail(Shutdown{}).send(worker);

    self->state().workers.clear();
}

ClientActor::behavior_type client_actor(
    ClientActor::stateful_pointer<ClientState> self,
    std::string host,
    std::uint16_t port,
    std::size_t worker_count)
{
    caf::expected<ServerActor> server =
        self->system().middleman().remote_actor<ServerActor>(
            std::move(host),
            port);

    if (!server) {
        self->println("Failed to connect to server: {}", server.error());
        self->quit();
        return {
            [self](Shutdown) {
                self->quit();
            },
        };
    }

    self->state().server = *server;
    self->state().workers.reserve(worker_count);

    self->monitor(
        self->state().server,
        [self](const caf::error&) {
            shutdown_workers(self);
            self->quit();
        });

    for (std::size_t index = 0; index < worker_count; ++index) {
        WorkerActor worker = spawn_worker(self->system());
        self->state().workers.push_back(worker);
        self->mail(RegisterWorker{.worker = worker}).send(self->state().server);
    }

    self->println("Connected {} worker(s) to the server", worker_count);

    return {
        [self](Shutdown) {
            shutdown_workers(self);
            self->quit();
        },
    };
}

}

ClientActor spawn_client(
    caf::actor_system& system,
    std::string host,
    std::uint16_t port,
    std::size_t worker_count)
{
    return system.spawn(
        client_actor,
        std::move(host),
        port,
        worker_count);
}

}
