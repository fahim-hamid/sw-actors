#ifndef PAIR_ACTOR_HPP
#define PAIR_ACTOR_HPP

#include "config.hpp"

namespace caf
{

    struct pairActorState
    {
        int matchScore = 0;
        int mismatchScore = 0;
        int gapScore = 0;
        int deviderRow = 1;
        int deviderCol = 1;
        int maximumScore = 0;
        int maxRow = 0;
        int maxCol = 0;
        int maxRowPos = 0;
        std::vector<actor> workers;
        std::string alignedSeq1;
        std::string alignedSeq2;
        actor manager;
        int position;
    };

    behavior pairActor(stateful_actor<pairActorState> *self, int matchScore, int mismatchScore, int gapScore, int deviderRow, int deviderCol);

} // namespace caf

#endif // PAIR_ACTOR_HPP