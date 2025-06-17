#include "outputActor.hpp"

namespace caf
{
    behavior outputActor(stateful_actor<outputActorState> *self, std::string outputFile)
    {
        // Open the output file
        self->state().outFile.open(outputFile);
        if (!self->state().outFile.is_open())
        {
            std::cerr << "Error opening output file: " << outputFile << std::endl;
            return {};
        }

        // Receive messages and write to the output file
        return {
            [=](const std::string message)
            {
                self->state().outFile << message << std::endl;
            },
            [=](actor manager, const std::string message)
            {
                // sort the output file
                self->state().outFile.close();

                // Read lines from the file
                std::ifstream inFile(outputFile);
                std::vector<std::string> lines;
                std::string line;

                while (std::getline(inFile, line))
                {
                    lines.push_back(line);
                }
                inFile.close();

                // Sort lines by the integer before the first comma
                std::sort(lines.begin(), lines.end(), [](const std::string &a, const std::string &b)
                          {
                        int numA = std::stoi(a.substr(0, a.find(',')));
                        int numB = std::stoi(b.substr(0, b.find(',')));
                        return numA < numB; });

                // Write sorted lines back to the file
                std::ofstream outFile(outputFile);
                for (const auto &sortedLine : lines)
                {
                    std::size_t commaPos = sortedLine.find(',');
                    if (commaPos != std::string::npos)
                    {
                        outFile << sortedLine.substr(commaPos + 1) << '\n';
                    }
                }
                outFile.close();

                self->state().outFile.close();
                self->quit();
                anon_mail("exit").send(manager);
            },
        };
    }
} // namespace caf