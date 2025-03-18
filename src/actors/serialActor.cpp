#include "serialActor.hpp"

namespace caf
{

    behavior serialActor(stateful_actor<serialActorState> *self, int matchScore, int mismatchScore, int gapScore)
    {
        self->state().matchScore = matchScore;
        self->state().mismatchScore = mismatchScore;
        self->state().gapScore = gapScore;

        // receive the messages
        return {
            [=](int maxLenQuery, int maxLenSubject)
            {
                self->state().scoreMatrix = std::vector<std::vector<int>>(maxLenQuery + 1, std::vector<int>(maxLenSubject + 1, 0));
            },
            [=](actor manager, int position, std::string seq1, std::string seq2)
            {
                int m = seq1.length();
                int n = seq2.length();

                // Fill the matrix and find the max score
                int maxScore = 0;
                int indexFirst = 0, indexSecond = 0;

                for (int i = 1; i <= m; ++i)
                {
                    for (int j = 1; j <= n; ++j)
                    {
                        int matchMismatchScore = (seq1[i - 1] == seq2[j - 1]) ? self->state().matchScore : self->state().mismatchScore;
                        int diagonal = self->state().scoreMatrix[i - 1][j - 1] + matchMismatchScore;
                        int left = self->state().scoreMatrix[i - 1][j] + self->state().gapScore;
                        int above = self->state().scoreMatrix[i][j - 1] + self->state().gapScore;
                        self->state().scoreMatrix[i][j] = std::max({0, diagonal, left, above});

                        if (self->state().scoreMatrix[i][j] > maxScore)
                        {
                            maxScore = self->state().scoreMatrix[i][j];
                            indexFirst = i;
                            indexSecond = j;
                        }
                    }
                }

                // Traceback to find the aligned sequences
                std::string alignedSeq1, alignedSeq2;
                int i = indexFirst;
                int j = indexSecond;

                while (i > 0 && j > 0)
                {
                    int score = self->state().scoreMatrix[i][j];
                    if (score == 0)
                        break;

                    int matchMismatchScore = (seq1[i - 1] == seq2[j - 1]) ? self->state().matchScore : self->state().mismatchScore;
                    int diagonal = self->state().scoreMatrix[i - 1][j - 1];
                    int above = self->state().scoreMatrix[i - 1][j];
                    int left = self->state().scoreMatrix[i][j - 1];

                    if (score == diagonal + matchMismatchScore)
                    {
                        alignedSeq1 = seq1[i - 1] + alignedSeq1;
                        alignedSeq2 = seq2[j - 1] + alignedSeq2;
                        --i;
                        --j;
                    }
                    else if (score == above + gapScore)
                    {
                        alignedSeq1 = seq1[i - 1] + alignedSeq1;
                        alignedSeq2 = '-' + alignedSeq2;
                        --i;
                    }
                    else if (score == left + gapScore)
                    {
                        alignedSeq1 = '-' + alignedSeq1;
                        alignedSeq2 = seq2[j - 1] + alignedSeq2;
                        --j;
                    }
                }
                self->println("Pair: {}, with score: {}, \nAligned seq1: {}, \nAligned seq2: {}\n", position, maxScore, alignedSeq1, alignedSeq2);

                anon_mail(self, position, maxScore).send(manager);
            },
            [=](actor serverOrManager)
            {
                anon_mail(self).send(serverOrManager);
            },
            [=](std::string exit)
            {
                self->quit();
            },
        };
    }
} // namespace caf
