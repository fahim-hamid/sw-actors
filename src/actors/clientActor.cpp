#include "clientActor.hpp"
#include "pairActor.hpp"

namespace caf
{
    behavior clientActor(stateful_actor<clientActorState> *self, const config &cfg)
    {
        // The below code connects this actor to a published actor on a remote host
        auto serverActor = self->system().middleman().remote_actor(cfg.host, cfg.port);

        if (!serverActor)
        {
            self->println("Failed to connect to remote actor at {}: {}", cfg.host, cfg.port);
            self->quit();
            return {};
        }
        else
        {
            self->println("Successfully connected to remote actor at {}: {}", cfg.host, cfg.port);
        }

        self->state().server = *serverActor;

        for (int i = 0; i < cfg.actorNumber; ++i)
        {
            actor worker = self->spawn(pairActor, cfg.matchScore, cfg.mismatchScore, cfg.gapScore,
                                       cfg.dividerRow, cfg.dividerCol);
            anon_mail(self->state().server).send(worker);
        }

        self->quit();

        return {};
    }
} // namespace caf