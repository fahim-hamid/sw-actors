#include "serialActor.hpp"
#include "pairActor.hpp"
#include "blockActor.hpp"

namespace caf
{
    behavior pairActor(stateful_actor<pairActorState> *self, int matchScore, int mismatchScore, int gapScore, int deviderRow, int deviderCol)
    {
        self->state().matchScore = matchScore;
        self->state().mismatchScore = mismatchScore;
        self->state().gapScore = gapScore;
        self->state().deviderRow = deviderRow;
        self->state().deviderCol = deviderCol;

        // recive the messages
        return {
            [=](actor serverOrManager)
            {
                anon_mail(self).send(serverOrManager);
            },
            [=](actor manager, int position, std::string seq1, std::string seq2)
            {
                if (self->state().deviderRow == 1 && self->state().deviderCol == 1)
                {
                    actor serialAct = self->spawn(serialActor, self->state().matchScore, self->state().mismatchScore, self->state().gapScore);
                    anon_mail(manager, self->state().maxLenQuery, self->state().maxLenSubject).send(serialAct);
                    anon_mail(position, seq1, seq2).send(serialAct);
                }
                else
                {
                    actor blockAct = self->spawn(blockActor, actor_cast<actor>(self), 0, seq1, seq2, self->state().matchScore, self->state().mismatchScore, self->state().gapScore, self->state().deviderRow, self->state().deviderCol);
                }
                self->state().manager = manager;
                self->state().position = position;
                self->state().workers.resize(self->state().deviderRow);
            },
            [=](int position, std::string seq1, std::string seq2)
            {
                actor blockAct = self->spawn(blockActor, actor_cast<actor>(self), 0, seq1, seq2, self->state().matchScore, self->state().mismatchScore, self->state().gapScore, self->state().deviderRow, self->state().deviderCol);
                self->state().position = position;
            },
            [=](actor worker, int rowPose, int maxScore, int maxRow, int maxCol)
            {
                self->state().workers[rowPose] = worker;

                if (maxScore > self->state().maximumScore)
                {
                    self->state().maximumScore = maxScore;
                    self->state().maxRow = maxRow;
                    self->state().maxCol = maxCol;
                    self->state().maxRowPos = rowPose;
                }

                if (rowPose == self->state().deviderRow - 1)
                {
                    anon_mail(self->state().maxRow, self->state().maxCol).send(self->state().workers[self->state().maxRowPos]);
                }
            },
            [=](int row, int col, int rowPose, int score, std::string aligned1, std::string aligned2)
            {
                self->state().alignedSeq1 = (aligned1 + self->state().alignedSeq1);
                self->state().alignedSeq2 = (aligned2 + self->state().alignedSeq2);
                if (score == 0)
                {
                    self->println("Pair: {} Alignment Score: {} \nAligned Sequence 1: {}\nAligned Sequence 2: {}\n", self->state().position,
                                  self->state().maximumScore, self->state().alignedSeq1, self->state().alignedSeq2);

                    anon_mail(self, self->state().position, self->state().maximumScore).send(self->state().manager);

                    for (auto &worker : self->state().workers)
                    {
                        anon_mail("exit").send(worker);
                    }

                    self->state().maximumScore = 0;
                    self->state().alignedSeq1 = "";
                    self->state().alignedSeq2 = "";
                }
                else
                {
                    anon_mail(row, col).send(self->state().workers[rowPose]);
                }
            },
            [=](int maxLenQuery, int maxLenSubject)
            {
                self->state().maxLenQuery = maxLenQuery;
                self->state().maxLenSubject = maxLenSubject;
            },
            [=](std::string exit)
            {
                self->quit();
            },
        };
    }
} // namespace caf