#ifndef BLOCK_ACTOR_HPP
#define BLOCK_ACTOR_HPP

#include "config.hpp"

namespace caf
{
    struct blockActorState
    {
        int rowPose = 0;
        std::string seq1;
        std::string seq2;
        int matchScore = 0;
        int mismatchScore = 0;
        int gapScore = 0;
        std::vector<int> colBlockIndex;
        int rowChunk = 0;
        int rowStart = 0;
        int rowEnd = 0;
        std::vector<std::vector<int>> blockScoreMatrix;
        actor nextBlockActor;
        int maxScore = 0;
        int maxRow = 0;
        int maxCol = 0;
        actor workManager;
    };

    behavior blockActor(stateful_actor<blockActorState> *self, actor workManager, int rowPose, std::string seq1, std::string seq2, int matchScore, int mismatchScore, int gapScore, int dividerRow, int dividerCol);

} // namespace caf

#endif // BLOCK_ACTOR_HPP