#ifndef PAIR_ACTOR_HPP
#define PAIR_ACTOR_HPP

#include "config.hpp"
#include "alignerInfo.hpp"

namespace caf
{

    struct pairActorState
    {
        int matchScore = 0;
        int mismatchScore = 0;
        int gapScore = 0;
        int dividerRow = 1;
        int dividerCol = 1;
        int maximumScore = 0;
        int maxRow = 0;
        int maxCol = 0;
        int maxRowPos = 0;
        std::vector<actor> workers;
        std::string seq1;
        std::string id1;
        std::string id2;
        alignerInfo alignment;
        int nm = 0;
        actor manager;
        actor output;
        int position;
        int maxLenQuery = 0;
        int maxLenSubject = 0;
    };

    behavior pairActor(stateful_actor<pairActorState> *self, int matchScore, int mismatchScore, int gapScore, int dividerRow, int dividerCol);

} // namespace caf

#endif // PAIR_ACTOR_HPP