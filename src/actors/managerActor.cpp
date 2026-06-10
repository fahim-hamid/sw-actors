#include "managerActor.hpp"
#include "pairActor.hpp"
#include "makePairs.hpp"
#include "outputActor.hpp"
#include "sw/fasta.hpp"

#include <algorithm>
#include <utility>

namespace caf
{

    behavior managerActor(stateful_actor<managerActorState> *self, const config &cfg)
    {
        if (cfg.distributedMode)
        {
            // The below code publishes this actor on a port
            auto is_port = self->system().middleman().publish(self, cfg.port);
            if (!is_port)
            {
                self->println("Failed to publish actor on port {}", cfg.port);
                self->quit();
            }
            else
            {
                self->println("Successfully published actor on port {}", cfg.port);
            }
        }

        auto query_sequences = sw::read_fasta(cfg.queryInput, cfg.maxSequences);
        if (!query_sequences)
        {
            self->println("Failed to read query FASTA: {}", query_sequences.error().message);
            self->quit();
            return {};
        }
        self->state().querySequences = std::move(*query_sequences);
        if (self->state().querySequences.empty())
        {
            self->println("Query FASTA contains no records");
            self->quit();
            return {};
        }

        if (!cfg.subjectInput.empty())
        {
            auto subject_sequences = sw::read_fasta(cfg.subjectInput, cfg.maxSequences);
            if (!subject_sequences)
            {
                self->println("Failed to read subject FASTA: {}", subject_sequences.error().message);
                self->quit();
                return {};
            }
            self->state().subjectSequences = std::move(*subject_sequences);
            if (self->state().subjectSequences.empty())
            {
                self->println("Subject FASTA contains no records");
                self->quit();
                return {};
            }
        }

        std::vector<std::vector<int>> paires = makePairs(
            self->state().querySequences.size(),
            self->state().subjectSequences.size(),
            cfg.maxPairs);
        self->state().workList1 = paires[0];
        self->state().workList2 = paires[1];

        for (const auto &sequence : self->state().querySequences)
            self->state().maxLenQuery = std::max(
                self->state().maxLenQuery,
                static_cast<int>(sequence.data.size()));

        for (const auto &sequence : self->state().subjectSequences)
            self->state().maxLenSubject = std::max(
                self->state().maxLenSubject,
                static_cast<int>(sequence.data.size()));

        if (self->state().subjectSequences.size() == 0)
        {
            self->state().subjectSequences = self->state().querySequences;
            self->state().maxLenSubject = self->state().maxLenQuery;
        }

        self->state().start = std::chrono::high_resolution_clock::now();

        // spawn the output actor
        actor outputAct = self->spawn(outputActor, cfg.outputFile);
        self->state().output = outputAct;

        int actorNum = cfg.actorNumber;

        if (self->state().workList1.empty())
        {
            anon_mail(self, "exit").send(self->state().output);
        }

        if(self->state().workList1.size() < actorNum)
        {
            actorNum = self->state().workList1.size();
        }
        
        for (int i = 0; i < actorNum; ++i)
        {
            actor worker = self->spawn(pairActor, cfg.matchScore, cfg.mismatchScore, cfg.gapScore,
                                       cfg.dividerRow, cfg.dividerCol);

            const auto &query = self->state().querySequences[self->state().workList1[i]];
            const auto &subject = self->state().subjectSequences[self->state().workList2[i]];

            anon_mail(self->state().maxLenQuery, self->state().maxLenSubject, self->state().output).send(worker);
            anon_mail(self, i, query.id, query.data, subject.id, subject.data).send(worker);
        }

        self->state().position = actorNum - 1;

        return {
            [=](actor sender, int position, int maxScore)
            {
                self->state().position++;
                self->state().counter++;

                if (self->state().position < self->state().workList1.size())
                {
                    const auto &query = self->state().querySequences[self->state().workList1[self->state().position]];
                    const auto &subject = self->state().subjectSequences[self->state().workList2[self->state().position]];
                    anon_mail(self->state().position, query.id, query.data, subject.id, subject.data).send(sender);
                }
                else
                {
                    anon_mail("exit").send(sender);
                }

                if (self->state().counter == self->state().workList1.size())
                {
                    anon_mail(self, "exit").send(self->state().output);
                }
            },
            [=](actor clientWorker)
            {
                self->state().position++;

                if (self->state().position < self->state().workList1.size())
                {
                    anon_mail(self->state().maxLenQuery, self->state().maxLenSubject, self->state().output).send(clientWorker);

                    const auto &query = self->state().querySequences[self->state().workList1[self->state().position]];
                    const auto &subject = self->state().subjectSequences[self->state().workList2[self->state().position]];
                    anon_mail(self, self->state().position, query.id, query.data, subject.id, subject.data).send(clientWorker);
                }
                else
                {
                    anon_mail("exit").send(clientWorker);
                }
            },
            [=](const std::string message) { // Stop the timer
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - self->state().start);
                self->println("Time taken by function: {} seconds\n", duration.count());
                self->quit();
            },
        };
    }
} // namespace caf
