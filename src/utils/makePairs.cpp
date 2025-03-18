#include "makePairs.hpp"

std::vector<std::vector<int>> makePairs(int querySize, int subjectSize)
{
    std::vector<std::vector<int>> pairs(2);

    // Create a list of all possible pairs of sequences
    if (subjectSize == 0)
    {
        for (int i = 0; i < querySize; ++i)
        {
            for (int j = i + 1; j < querySize; ++j)
            {
                pairs[0].push_back(i);
                pairs[1].push_back(j);
            }
        }
    }
    else
    {
        for (int i = 0; i < querySize; ++i)
        {
            for (int j = 0; j < subjectSize; ++j)
            {
                pairs[0].push_back(i);
                pairs[1].push_back(j);
            }
        }
    }
    return pairs;
}
