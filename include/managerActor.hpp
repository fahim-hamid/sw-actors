#ifndef MANAGER_ACTOR_HPP
#define MANAGER_ACTOR_HPP

#include "config.hpp"
#include "sw/sequence.hpp"

namespace caf
{

    struct managerActorState
    {
        std::vector<int> workList1;
        std::vector<int> workList2;
        std::vector<sw::Sequence> querySequences;
        std::vector<sw::Sequence> subjectSequences;
        std::chrono::high_resolution_clock::time_point start; // Explicitly specify the type
        int position = 0;
        int counter = 0;
        int maxLenQuery = 0;
        int maxLenSubject = 0;
        actor output;
    };

    behavior managerActor(stateful_actor<managerActorState> *self, const config &cfg);

} // namespace caf

#endif // MANAGER_ACTOR_HPP
