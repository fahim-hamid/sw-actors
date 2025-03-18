#include "blockActor.hpp"

namespace caf
{
    behavior blockActor(stateful_actor<blockActorState> *self, actor workManager, int rowPose, std::string seq1, std::string seq2, int matchScore, int mismatchScore, int gapScore, int deviderRow, int deviderCol)
    {

        int m = seq1.length();
        int n = seq2.length();

        self->state().rowChunk = m / deviderRow;
        int colChunk = n / deviderCol;

        std::vector<int> rowBlockIndex = std::vector<int>((deviderRow + 1), (m + 1));
        self->state().colBlockIndex = std::vector<int>((deviderCol + 1), (n + 1));

        for (int i = 0; i < deviderRow; i++)
        {
            rowBlockIndex[i] = i * self->state().rowChunk + 1;
        }

        for (int i = 0; i < deviderCol; i++)
        {
            self->state().colBlockIndex[i] = i * colChunk + 1;
        }

        int matrixSize = rowBlockIndex[rowPose + 1] - rowBlockIndex[rowPose];
        self->state().blockScoreMatrix = std::vector<std::vector<int>>(matrixSize + 1, std::vector<int>(n + 1, 0));

        self->state().rowPose = rowPose;
        self->state().workManager = workManager;
        self->state().seq1 = seq1;
        self->state().seq2 = seq2;
        self->state().matchScore = matchScore;
        self->state().mismatchScore = mismatchScore;
        self->state().gapScore = gapScore;

        self->state().rowStart = rowBlockIndex[self->state().rowPose];
        self->state().rowEnd = rowBlockIndex[self->state().rowPose + 1] - 1;

        if (rowPose < deviderRow - 1)
            self->state().nextBlockActor = self->spawn(blockActor, workManager, (rowPose + 1), seq1, seq2, matchScore, mismatchScore, gapScore, deviderRow, deviderCol);

        if (rowPose == 0)
        {
            int colPose = 0;

            while (colPose < deviderCol)
            {
                int colStarting = self->state().colBlockIndex[colPose];
                int colEnding = self->state().colBlockIndex[colPose + 1] - 1;

                for (int i = self->state().rowStart; i <= self->state().rowEnd; ++i)
                {
                    for (int j = colStarting; j <= colEnding; ++j)
                    {
                        int matchMismatchScore = (seq1[i - 1] == seq2[j - 1]) ? matchScore : mismatchScore;
                        int diagonal = self->state().blockScoreMatrix[i - 1][j - 1] + matchMismatchScore;
                        int left = self->state().blockScoreMatrix[i - 1][j] + gapScore;
                        int above = self->state().blockScoreMatrix[i][j - 1] + gapScore;
                        self->state().blockScoreMatrix[i][j] = std::max({0, diagonal, left, above});

                        if (self->state().blockScoreMatrix[i][j] > self->state().maxScore)
                        {
                            self->state().maxScore = self->state().blockScoreMatrix[i][j];
                            self->state().maxRow = i;
                            self->state().maxCol = j;
                        }
                    }
                }

                if (self->state().rowPose < deviderRow - 1)
                {
                    std::vector<int> partialVector(
                        self->state().blockScoreMatrix.back().begin() + colStarting,
                        self->state().blockScoreMatrix.back().begin() + colEnding + 1);
                    anon_mail(colPose, partialVector, deviderRow, deviderCol).send(self->state().nextBlockActor);
                }

                colPose++;
            }
            anon_mail(self, self->state().rowPose, self->state().maxScore, self->state().maxRow, self->state().maxCol).send(self->state().workManager);
        }

        // receive the messages
        return {
            [=](int colPose, std::vector<int> previousvector, int deviderRow, int deviderCol)
            {
                int colStarting = self->state().colBlockIndex[colPose];
                int colEnding = self->state().colBlockIndex[colPose + 1] - 1;
                int rowPoseOverhead = self->state().rowPose * self->state().rowChunk;

                std::copy(
                    previousvector.begin(),
                    previousvector.end(),
                    self->state().blockScoreMatrix[0].begin() + colStarting);

                for (int i = self->state().rowStart; i <= self->state().rowEnd; ++i)
                {
                    for (int j = colStarting; j <= colEnding; ++j)
                    {
                        int matchMismatchScore = (self->state().seq1[i - 1] == self->state().seq2[j - 1]) ? self->state().matchScore : self->state().mismatchScore;
                        int diagonal = self->state().blockScoreMatrix[i - rowPoseOverhead - 1][j - 1] + matchMismatchScore;
                        int left = self->state().blockScoreMatrix[i - rowPoseOverhead - 1][j] + self->state().gapScore;
                        int above = self->state().blockScoreMatrix[i - rowPoseOverhead][j - 1] + self->state().gapScore;
                        self->state().blockScoreMatrix[i - rowPoseOverhead][j] = std::max({0, diagonal, left, above});

                        if (self->state().blockScoreMatrix[i - rowPoseOverhead][j] > self->state().maxScore)
                        {
                            self->state().maxScore = self->state().blockScoreMatrix[i - rowPoseOverhead][j];
                            self->state().maxRow = i;
                            self->state().maxCol = j;
                        }
                    }
                }
                if (self->state().rowPose < deviderRow - 1)
                {
                    std::vector<int> partialVector(
                        self->state().blockScoreMatrix.back().begin() + colStarting,
                        self->state().blockScoreMatrix.back().begin() + colEnding + 1);
                    anon_mail(colPose, partialVector, deviderRow, deviderCol).send(self->state().nextBlockActor);
                }
                if (colPose == deviderCol - 1)
                    anon_mail(self, self->state().rowPose, self->state().maxScore, self->state().maxRow, self->state().maxCol).send(self->state().workManager);
            },
            [=](int row, int col)
            {
                int rowPoseOverhead = self->state().rowPose * self->state().rowChunk;
                std::string alignedSeq1;
                std::string alignedSeq2;
                int score = self->state().blockScoreMatrix[row - rowPoseOverhead][col];
                int diagonal = 0;
                int upScore = 0;
                int leftScore = 0;
                while ((row - rowPoseOverhead) > 0 && col > 0)
                {

                    if (score == 0)
                    {
                        break;
                    }

                    diagonal = self->state().blockScoreMatrix[row - rowPoseOverhead - 1][col - 1];
                    upScore = self->state().blockScoreMatrix[row - rowPoseOverhead - 1][col];
                    leftScore = self->state().blockScoreMatrix[row - rowPoseOverhead][col - 1];
                    int matchMismatchScore = self->state().seq1[row - 1] == self->state().seq2[col - 1] ? self->state().matchScore : self->state().mismatchScore;

                    if (score == diagonal + matchMismatchScore)
                    {
                        alignedSeq1 = self->state().seq1[row - 1] + alignedSeq1;
                        alignedSeq2 = self->state().seq2[col - 1] + alignedSeq2;
                        --row;
                        --col;
                        score = diagonal;
                    }
                    else if (score == upScore + self->state().gapScore)
                    {
                        alignedSeq1 = self->state().seq1[row - 1] + alignedSeq1;
                        alignedSeq2 = "-" + alignedSeq2;
                        --row;
                        score = upScore;
                    }
                    else if (score == leftScore + self->state().gapScore)
                    {
                        alignedSeq1 = "-" + alignedSeq1;
                        alignedSeq2 = self->state().seq2[col - 1] + alignedSeq2;
                        --col;
                        score = leftScore;
                    }
                }
                anon_mail(row, col, self->state().rowPose - 1, score, alignedSeq1, alignedSeq2).send(self->state().workManager);
            },
            [=](std::string exit)
            {
                self->quit();
            },
        };
    }
} // namespace caf
