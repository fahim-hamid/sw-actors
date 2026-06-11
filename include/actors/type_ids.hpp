#pragma once

#include "actors/client_actor.hpp"
#include "actors/output_actor.hpp"
#include "actors/server_actor.hpp"
#include "actors/worker_actor.hpp"

#include <caf/type_id.hpp>

CAF_BEGIN_TYPE_ID_BLOCK(sw_actors, caf::first_custom_type_id)

CAF_ADD_TYPE_ID(sw_actors, (sw::ScoreConfig))
CAF_ADD_TYPE_ID(sw_actors, (sw::CigarCode))
CAF_ADD_TYPE_ID(sw_actors, (sw::CigarOp))
CAF_ADD_TYPE_ID(sw_actors, (std::vector<sw::CigarOp>))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::Shutdown))

CAF_ADD_TYPE_ID(sw_actors, (sw::actors::WorkerConfig))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::AlignmentJob))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::CompletedAlignment))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::WorkerActor))

CAF_ADD_TYPE_ID(sw_actors, (sw::actors::OutputRecord))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::FinishOutput))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::OutputActor))

CAF_ADD_TYPE_ID(sw_actors, (sw::actors::RegisterWorker))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::StartPipeline))
CAF_ADD_TYPE_ID(sw_actors, (sw::actors::ServerActor))

CAF_ADD_TYPE_ID(sw_actors, (sw::actors::ClientActor))

CAF_END_TYPE_ID_BLOCK(sw_actors)
