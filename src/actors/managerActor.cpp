#include "managerActor.hpp"
#include "pairActor.hpp"
#include "makePairs.hpp"
#include "readFasta.hpp"

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

        self->state().querySequences = readFasta(cfg.queryInput);
        self->state().subjectSequences = readFasta(cfg.subjectInput);

        std::vector<std::vector<int>> paires = makePairs(self->state().querySequences.size(), self->state().subjectSequences.size());
        self->state().workList1 = paires[0];
        self->state().workList2 = paires[1];

        for (int i = 0; i < self->state().querySequences.size(); i++)
        {
            if (self->state().querySequences[i].size() > self->state().maxLenQuery)
                self->state().maxLenQuery = self->state().querySequences[i].size();
        }
        for (int i = 0; i < self->state().subjectSequences.size(); i++)
        {
            if (self->state().subjectSequences[i].size() > self->state().maxLenSubject)
                self->state().maxLenSubject = self->state().subjectSequences[i].size();
        }

        if (self->state().subjectSequences.size() == 0)
        {
            self->state().subjectSequences = self->state().querySequences;
            self->state().maxLenSubject = self->state().maxLenQuery;
        }

        self->state().start = std::chrono::high_resolution_clock::now();

        self->println("Query sequences: {}", self->state().querySequences.size());
        self->println("Subject sequences: {}", self->state().subjectSequences.size());

        for (int i = 0; i < cfg.actorNumber; ++i)
        {
            actor worker = self->spawn(pairActor, cfg.matchScore, cfg.mismatchScore, cfg.gapScore,
                                       cfg.deviderRow, cfg.deviderCol);

            std::string seq1 = self->state().querySequences[self->state().workList1[i]];
            std::string seq2 = self->state().subjectSequences[self->state().workList2[i]];

            anon_mail(self->state().maxLenQuery, self->state().maxLenSubject).send(worker);
            anon_mail(self, i, seq1, seq2).send(worker);
        }

        self->state().position = cfg.actorNumber - 1;

        return {
            [=](actor sender, int position, int maxScore)
            {
                self->state().position++;
                self->state().counter++;

                if (self->state().position < self->state().workList1.size())
                {
                    std::string seq1 = self->state().querySequences[self->state().workList1[self->state().position]];
                    std::string seq2 = self->state().subjectSequences[self->state().workList2[self->state().position]];
                    anon_mail(self->state().position, seq1, seq2).send(sender);
                }
                else
                {
                    anon_mail("exit").send(sender);
                }

                if (self->state().counter == self->state().workList1.size())
                {
                    // Stop the timer
                    auto stop = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - self->state().start);
                    self->println("Time taken by function: {} seconds\n", duration.count());
                    self->quit();
                }
            },
            [=](actor clientWorker)
            {
                self->state().position++;

                if (self->state().position < self->state().workList1.size())
                {
                    anon_mail(self->state().maxLenQuery, self->state().maxLenSubject).send(clientWorker);

                    std::string seq1 = self->state().querySequences[self->state().workList1[self->state().position]];
                    std::string seq2 = self->state().subjectSequences[self->state().workList2[self->state().position]];
                    anon_mail(self, self->state().position, seq1, seq2).send(clientWorker);
                }
                else
                {
                    anon_mail("exit").send(clientWorker);
                }
            },
        };
    }
} // namespace caf