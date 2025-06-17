#ifndef OUTPUT_ACTOR_HPP
#define OUTPUT_ACTOR_HPP

#include "config.hpp"


namespace caf
{
    struct outputActorState
    {
        std::ofstream outFile;
    };
    
    behavior outputActor(stateful_actor<outputActorState> *self, std::string outputFile);

} // namespace caf
#endif // OUTPUT_ACTOR_HPP