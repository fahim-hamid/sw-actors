#ifndef CLIENT_ACTOR_HPP
#define CLIENT_ACTOR_HPP

#include "config.hpp"

namespace caf
{

    struct clientActorState
    {
        actor server;
    };

    behavior clientActor(stateful_actor<clientActorState> *self, const config &cfg);

} // namespace caf

#endif // CLIENT_ACTOR_HPP