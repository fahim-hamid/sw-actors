#include "readFasta.hpp"

// Reads a FASTA file and returns a pair of vectors: (sequence IDs, sequences)
std::vector<std::vector<std::string>> readFasta(const std::string& filename) {
    std::vector<std::vector<std::string>> result;

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        return {};
    }
    std::string line;
    std::string seq;
    std::string current_id;

    while (std::getline(inputFile, line)) {
        // Trim trailing whitespace
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty()) continue;

        if (line[0] == '>') {
            if (!seq.empty()) {
                result.push_back({current_id, seq});
                seq.clear();
            }
            current_id = line.substr(1); // Remove the '>' character
            current_id = current_id.substr(0, current_id.find_first_of(" ")); // Keep only the ID part
            
            line.erase();
        } else {
            seq += line; // Append the sequence line
        }
    }
    if (!seq.empty()) {
        result.push_back({current_id, seq});
    }
    inputFile.close();
    return result;
}