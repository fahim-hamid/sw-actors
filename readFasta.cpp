#include "readFasta.hpp"

// A function to read multiple sequences from a fasta file
std::vector<std::string> readFasta(std::string filename)
{
    // a vector to store the sequences
    std::vector<std::string> sequences;

    // Read Multiple sequences from the input file
    std::ifstream inputFile(filename);
    std::string line;
    std::string seq;
    while (std::getline(inputFile, line))
    {
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line[0] == '>')
        {
            if (!seq.empty())
            {
                sequences.push_back(seq);
                seq.clear();
            }
        }
        else
        {
            seq += line;
        }
    }
    sequences.push_back(seq);
    inputFile.close();

    return sequences;
}
