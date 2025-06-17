#include "alignerInfo.hpp"
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
        [=](actor manager, int maxLenQuery, int maxLenSubject, actor output)
        {
          self->state().scoreMatrix = std::vector<std::vector<int>>(maxLenQuery + 1, std::vector<int>(maxLenSubject + 1, 0));
          self->state().manager = manager;
          self->state().output = output;
        },
        [=](int position, std::string id1, std::string seq1, std::string id2, std::string seq2)
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
          anon_mail(position, id1, seq1, id2, seq2, "SAM", indexFirst, indexSecond).send(self);
        },
        [=](int position, std::string id1, std::string seq1, std::string id2, std::string seq2, std::string outputFormat, int indexFirst, int indexSecond)
        {
          // Traceback to find the aligned sequences
          alignerInfo alignment;
          int i = indexFirst;
          int j = indexSecond;
          int maxScore = self->state().scoreMatrix[i][j];
          int softClipEnd = seq1.length() - indexFirst;
          if (softClipEnd > 0)
            alignment.newOp('S', softClipEnd);

          int nm = 0;

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
              if (matchMismatchScore == self->state().matchScore)
              {
                alignment.newOp('=', 1);
              }
              else
              {
                alignment.newOp('X', 1);
                nm++;
              }
              --i;
              --j;
            }
            else if (score == above + gapScore)
            {
              alignment.newOp('I', 1);
              nm++;
              --i;
            }
            else if (score == left + gapScore)
            {
              alignment.newOp('D', 1);
              nm++;
              --j;
            }
          }
          int pos = j + 1;
          int softClipStart = i;
          if (softClipStart > 0)
            alignment.newOp('S', softClipStart);

          std::string cigar = alignment.str();

          std::ostringstream oss;
          oss << position << "," << id1 << "\t0\t" << id2 << "\t" << pos << "\t255\t" << cigar
              << "\t*\t0\t0\t" << seq1 << "\t*\tAS:i:" << maxScore << "\tNM:i:" << nm;
          std::string OutMess = oss.str();
          anon_mail(OutMess).send(self->state().output);
          anon_mail(self, position, maxScore).send(self->state().manager);
        },
        [=](std::string exit)
        {
          self->quit();
        },
    };
  }
} // namespace caf
