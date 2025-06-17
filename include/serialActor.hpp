#ifndef SERIAL_ACTOR_HPP
#define SERIAL_ACTOR_HPP

#include "config.hpp"

namespace caf
{
    struct serialActorState
    {
        int matchScore = 0;
        int mismatchScore = 0;
        int gapScore = 0;
        std::vector<std::vector<int>> scoreMatrix;
        actor manager;
        actor output;
    };

    behavior serialActor(stateful_actor<serialActorState> *self, int matchScore, int mismatchScore, int gapScore);
} // namespace caf

#endif // SERIAL_ACTOR_HPP