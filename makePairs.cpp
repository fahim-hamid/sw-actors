#include "makePairs.hpp"

std::vector<std::vector<int>> makePairs(int seqSize)
{
    std::vector<std::vector<int>> pairs(2);
    // Create a list of all possible pairs of sequences
    for (int i = 0; i < seqSize; ++i)
    {
        for (int j = i + 1; j < seqSize; ++j)
        {
            pairs[0].push_back(i);
            pairs[1].push_back(j);
        }
    }
    return pairs;
}
