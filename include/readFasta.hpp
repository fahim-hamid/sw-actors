#ifndef READFASTA_HPP
#define READFASTA_HPP

#include "config.hpp"

// A function to read multiple sequences from a fasta file
std::vector<std::vector<std::string>> readFasta(const std::string& filename, size_t maxSequences = 0);

#endif // READFASTA_HPP
