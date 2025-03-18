#include "clientActor.hpp"
#include "serialActor.hpp"
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

        if (cfg.deviderRow == 1 && cfg.deviderCol == 1)
        {
            for (int i = 0; i < cfg.actorNumber; ++i)
            {
                actor worker = self->spawn(serialActor, cfg.matchScore, cfg.mismatchScore, cfg.gapScore);
                anon_mail(self->state().server).send(worker);
            }
        }
        else
        {
            for (int i = 0; i < cfg.actorNumber; ++i)
            {
                actor worker = self->spawn(pairActor, cfg.matchScore, cfg.mismatchScore, cfg.gapScore,
                                           cfg.deviderRow, cfg.deviderCol);
                anon_mail(self->state().server).send(worker);
            }
        }

        return {};
    }
} // namespace caf