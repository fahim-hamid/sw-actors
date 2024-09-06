/*
A program that reads Multiple sequences from a fasta file and calls the
Smith-Waterman algorithm to align any possible pair of them Using Actor Model.
The aligned sequences and the alignment score are then printed to the output
file.
 */
// include the necessary libraries
#include "config.hpp"

using namespace std;
using namespace caf;

// The Manager Actor
behavior workerActor(event_based_actor *self)
{
    // recive the messages
    return {[=](actor manager, int position)
            {
                // Set the scores
                int matchScore = 2;
                int mismatchScore = -1;
                int gapScore = -2;

                // Get the length of the sequences
                int index1 = workList1[position];
                int index2 = workList2[position];
                string seq1 = sequences[index1];
                string seq2 = sequences[index2];
                int m = seq1.length();
                int n = seq2.length();

                // Create a matrix to store the scores
                vector<vector<int>> scoreMatrix(m + 1, vector<int>(n + 1, 0));

                // Fill the matrix and find the max score
                int maxScore = 0;
                int indexFirst, indexSecond;

                // loop through the matrix row by row
                for (int i = 1; i <= m; ++i)
                {
                    for (int j = 1; j <= n; ++j)
                    {
                        // find 3 scores for adjacent cells according to the SW algorithm
                        int diagonal = scoreMatrix[i - 1][j - 1] + (seq1[i - 1] == seq2[j - 1] ? matchScore : mismatchScore);
                        int left = scoreMatrix[i - 1][j] + gapScore;
                        int above = scoreMatrix[i][j - 1] + gapScore;
                        // take the maximum of the three scores and 0
                        scoreMatrix[i][j] = max({0, diagonal, left, above});

                        // update the max score and the corresponding indices
                        if (scoreMatrix[i][j] > maxScore)
                        {
                            maxScore = scoreMatrix[i][j];
                            indexFirst = i;
                            indexSecond = j;
                        }
                    }
                }

                // Traceback and find the aligned sequences
                string alignedSeq1, alignedSeq2;
                int i = indexFirst;
                int j = indexSecond;

                // start from the cell with the max score and loop until a cell with score 0 is reached
                while (i > 0 || j > 0)
                {
                    // get the current score and the scores of the adjacent cells
                    int score = scoreMatrix[i][j];
                    // check termination condition
                    if (score == 0)
                    {
                        break;
                    }
                    // check different strategies for updating the aligned sequences
                    // match/mismatch case
                    int diagonal = scoreMatrix[i - 1][j - 1];
                    if (score == diagonal + (seq1[i - 1] == seq2[j - 1] ? matchScore : mismatchScore))
                    {
                        alignedSeq1 = seq1[i - 1] + alignedSeq1;
                        alignedSeq2 = seq2[j - 1] + alignedSeq2;
                        --i;
                        --j;
                        continue;
                    }
                    // gap for first sequence cases
                    int above = scoreMatrix[i - 1][j];
                    if (score == above + gapScore)
                    {
                        alignedSeq1 = seq1[i - 1] + alignedSeq1;
                        alignedSeq2 = '-' + alignedSeq2;
                        --i;
                        continue;
                    }
                    // gap for second sequence cases
                    int left = scoreMatrix[i][j - 1];
                    if (score == left + gapScore)
                    {
                        alignedSeq1 = '-' + alignedSeq1;
                        alignedSeq2 = seq2[j - 1] + alignedSeq2;
                        --j;
                        continue;
                    }
                }

                // write the aligned sequences and the score to the output file
                self->println("Pair: {}, {} with score: {}, \nAligned seq1: {}, \nAligned seq2: {}\n",
                              workList1[position], workList2[position], maxScore, alignedSeq1, alignedSeq2);

                // send message to the manager to request new work
                anon_mail(self, position, maxScore).send(manager);
            }};
}

// Define the state of the Manager Actor
struct state_full_actor
{
    int position = 0;
    int counter = 0;
};
// The Manager Actor
behavior managerActor(stateful_actor<state_full_actor> *self)
{
    // recive the messages
    return {
        [=](int actorNumber)
        {
            // Spawn the worker Actors and send them the position in the workList
            for (int i = 0; i < actorNumber; ++i)
            {
                actor worker = self->spawn(workerActor);
                anon_mail(self, i).send(worker);
            }
            self->state().position = actorNumber - 1;
        },
        [=](actor sender, int position, int score)
        {
            // update the position and the counter
            self->state().position++;
            self->state().counter++;

            // check if there is more work to do
            if (self->state().position < workList1.size())
            {
                // send the next work to the worker
                anon_mail(self, self->state().position).send(sender);
            }

            if (self->state().counter == workList1.size())
            {
                // Stop the timer
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
                self->println("Time taken by function: {} seconds\n", duration.count());
                self->quit();
            }
        },
    };
}

// Main now takes a config object as an additional argument
void caf_main(actor_system &system)
{
    // Read sequences from the input file
    sequences = readFasta("/globalhome/pma753/HPC/Two-Level-Test/Data/BRCA1.fasta");

    // Create a list of all possible pairs of sequences
    vector<vector<int>> paires = makePairs(sequences.size());
    workList1 = paires[0];
    workList2 = paires[1];
    // Start the timer
    start = std::chrono::high_resolution_clock::now();

    // Span the Manager Actor
    actor manager = system.spawn(managerActor);

    // Send a message to the manager with actorNumber to start the process
    int actorNumber = 40;
    anon_mail(actorNumber).send(manager);

    // The rest of the process is being don inside the managerActor
}

CAF_MAIN()