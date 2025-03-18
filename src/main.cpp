#include "config.hpp"
#include "managerActor.hpp"
#include "clientActor.hpp"

void caf_main(caf::actor_system &system, const caf::config &cfg)
{
    caf::scoped_actor self{system};

    if (cfg.distributedMode && !cfg.serverMode)
        auto client = system.spawn(caf::clientActor, std::cref(cfg));
    else
        auto server = system.spawn(caf::managerActor, std::cref(cfg));
}

CAF_MAIN(caf::id_block::myType, caf::io::middleman)
