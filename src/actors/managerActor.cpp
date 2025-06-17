#include "managerActor.hpp"
#include "pairActor.hpp"
#include "makePairs.hpp"
#include "readFasta.hpp"
#include "outputActor.hpp"

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
            if (self->state().querySequences[i][1].size() > self->state().maxLenQuery)
                self->state().maxLenQuery = self->state().querySequences[i][1].size();
        }
        for (int i = 0; i < self->state().subjectSequences.size(); i++)
        {
            if (self->state().subjectSequences[i][1].size() > self->state().maxLenSubject)
                self->state().maxLenSubject = self->state().subjectSequences[i][1].size();
        }

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

        if(self->state().workList1.size() < actorNum)
        {
            actorNum = self->state().workList1.size();
        }
        
        for (int i = 0; i < actorNum; ++i)
        {
            actor worker = self->spawn(pairActor, cfg.matchScore, cfg.mismatchScore, cfg.gapScore,
                                       cfg.dividerRow, cfg.dividerCol);

            std::string id1 = self->state().querySequences[self->state().workList1[i]][0];
            std::string seq1 = self->state().querySequences[self->state().workList1[i]][1];
            std::string id2 = self->state().subjectSequences[self->state().workList2[i]][0];
            std::string seq2 = self->state().subjectSequences[self->state().workList2[i]][1];

            anon_mail(self->state().maxLenQuery, self->state().maxLenSubject, self->state().output).send(worker);
            anon_mail(self, i, id1, seq1, id2, seq2).send(worker);
        }

        self->state().position = cfg.actorNumber - 1;

        return {
            [=](actor sender, int position, int maxScore)
            {
                self->state().position++;
                self->state().counter++;

                if (self->state().position < self->state().workList1.size())
                {
                    std::string id1 = self->state().querySequences[self->state().workList1[self->state().position]][0];
                    std::string seq1 = self->state().querySequences[self->state().workList1[self->state().position]][1];
                    std::string id2 = self->state().subjectSequences[self->state().workList2[self->state().position]][0];
                    std::string seq2 = self->state().subjectSequences[self->state().workList2[self->state().position]][1];
                    anon_mail(self->state().position, id1, seq1, id2, seq2).send(sender);
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

                    std::string id1 = self->state().querySequences[self->state().workList1[self->state().position]][0];
                    std::string seq1 = self->state().querySequences[self->state().workList1[self->state().position]][1];
                    std::string id2 = self->state().subjectSequences[self->state().workList2[self->state().position]][0];
                    std::string seq2 = self->state().subjectSequences[self->state().workList2[self->state().position]][1];
                    anon_mail(self, self->state().position, id1, seq1, id2, seq2).send(clientWorker);
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