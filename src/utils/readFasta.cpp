#include "readFasta.hpp"

// A function to read multiple sequences from a fasta file
std::vector<std::string> readFasta(std::string filename)
{
    // a vector to store the sequences
    std::vector<std::string> sequences;

    // Read Multiple sequences from the input file
    std::ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        return {}; // Return an empty vector if file can't be opened
    }
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
