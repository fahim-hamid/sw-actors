#include "alignerInfo.hpp"
#include "serialActor.hpp"
#include "pairActor.hpp"
#include "blockActor.hpp"

namespace caf
{
    behavior pairActor(stateful_actor<pairActorState> *self, int matchScore, int mismatchScore, int gapScore, int dividerRow, int dividerCol)
    {
        self->state().matchScore = matchScore;
        self->state().mismatchScore = mismatchScore;
        self->state().gapScore = gapScore;
        self->state().dividerRow = dividerRow;
        self->state().dividerCol = dividerCol;

        // recive the messages
        return {
            [=](actor serverOrManager)
            {
                anon_mail(self).send(serverOrManager);
            },
            [=](actor manager, int position, std::string id1, std::string seq1, std::string id2, std::string seq2)
            {
                if (self->state().dividerRow == 1 && self->state().dividerCol == 1)
                {
                    actor serialAct = self->spawn(serialActor, self->state().matchScore, self->state().mismatchScore, self->state().gapScore);
                    anon_mail(manager, self->state().maxLenQuery, self->state().maxLenSubject, self->state().output).send(serialAct);
                    anon_mail(position, id1, seq1, id2, seq2).send(serialAct);
                    self->quit();
                }
                else
                {
                    actor blockAct = self->spawn(blockActor, actor_cast<actor>(self), 0, seq1, seq2, self->state().matchScore, self->state().mismatchScore, self->state().gapScore, self->state().dividerRow, self->state().dividerCol);
                }
                self->state().manager = manager;
                self->state().position = position;
                self->state().workers.resize(self->state().dividerRow);
                self->state().id1 = id1;
                self->state().seq1 = seq1;
                self->state().id2 = id2;
            },
            [=](int position, std::string id1, std::string seq1, std::string id2, std::string seq2)
            {
                actor blockAct = self->spawn(blockActor, actor_cast<actor>(self), 0, seq1, seq2, self->state().matchScore, self->state().mismatchScore, self->state().gapScore, self->state().dividerRow, self->state().dividerCol);
                self->state().position = position;
                self->state().id1 = id1;
                self->state().seq1 = seq1;
                self->state().id2 = id2;
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

                if (rowPose == self->state().dividerRow - 1)
                {
                    anon_mail(self->state().maxRow, self->state().maxCol).send(self->state().workers[self->state().maxRowPos]);
                    int softClipEnd = self->state().seq1.length() - self->state().maxRow;
                    if (softClipEnd > 0)
                        self->state().alignment.newOp('S', softClipEnd);
                }
            },
            [=](int row, int col, int rowPose, int score)
            {
                if (score == 0)
                {
                    int softClipStart = row;
                    if (softClipStart > 0)
                        self->state().alignment.newOp('S', softClipStart);

                    // reverse the alignment
                    std::string cigar = self->state().alignment.str();

                    int pos = col + 1;
                    
                    std::ostringstream oss;
                    oss << self->state().position << "," << self->state().id1 << "\t0\t" << self->state().id2 << "\t" << pos << "\t255\t" << cigar
                        << "\t*\t0\t0\t" << self->state().seq1 << "\t*\tAS:i:" << self->state().maximumScore << "\tNM:i:" << self->state().nm;
                    std::string OutMess = oss.str();
                    anon_mail(OutMess).send(self->state().output);
                    anon_mail(self, self->state().position, self->state().maximumScore).send(self->state().manager);

                    for (auto &worker : self->state().workers)
                    {
                        anon_mail("exit").send(worker);
                    }
                    self->state().maximumScore = 0;
                    alignerInfo newAlign;
                    self->state().alignment = newAlign;
                }
                else
                {
                    anon_mail(row, col).send(self->state().workers[rowPose]);
                }
            },
            [=](char op)
            {
                if (op != '=')
                    self->state().nm++;
                self->state().alignment.newOp(op, 1);
            },
            [=](int maxLenQuery, int maxLenSubject, actor output)
            {
                self->state().maxLenQuery = maxLenQuery;
                self->state().maxLenSubject = maxLenSubject;
                self->state().output = output;
            },
            [=](std::string exit)
            {
                self->quit();
            },
        };
    }
} // namespace caf