#include "actors/type_ids.hpp"
#include "actors/output_actor.hpp"

#include <caf/typed_event_based_actor.hpp>

#include <fstream>
#include <utility>

namespace sw::actors {
namespace {

char cigar_character(CigarCode code) noexcept
{
    switch (code) {
    case CigarCode::match:
        return '=';
    case CigarCode::mismatch:
        return 'X';
    case CigarCode::insertion:
        return 'I';
    case CigarCode::deletion:
        return 'D';
    case CigarCode::soft_clip:
        return 'S';
    }

    return '?';
}

struct OutputState {
    std::ofstream output;
    bool healthy = false;
};

OutputActor::behavior_type output_actor(
    OutputActor::stateful_pointer<OutputState> self,
    std::string output_file)
{
    self->state().output.open(output_file);
    self->state().healthy = self->state().output.is_open();

    if (!self->state().healthy)
        self->println("Failed to open output file '{}'", output_file);

    return {
        [self](const OutputRecord& record) {
            if (!self->state().healthy)
                return;

            const CompletedAlignment& alignment = record.alignment;
            std::ofstream& output = self->state().output;

            output
                << record.query_id.str()
                << "\t0\t"
                << record.subject_id.str()
                << '\t'
                << alignment.subject_begin + 1
                << "\t255\t";

            for (const CigarOp& operation : alignment.cigar)
                output << operation.length << cigar_character(operation.code);

            output
                << "\t*\t0\t0\t"
                << record.query.str()
                << "\t*\tAS:i:"
                << alignment.score
                << "\tNM:i:"
                << alignment.edits
                << '\n';

            if (!output)
                self->state().healthy = false;
        },
        [self](FinishOutput) -> bool {
            if (self->state().output.is_open()) {
                self->state().output.flush();
                self->state().output.close();
            }

            return self->state().healthy;
        },
        [self](Shutdown) {
            self->quit();
        },
    };
}

}

OutputActor spawn_output_actor(
    caf::actor_system& system,
    std::string output_file)
{
    return system.spawn(output_actor, std::move(output_file));
}

}
