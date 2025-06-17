#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <string_view>
#include <utility>
#include <tuple>

CAF_BEGIN_TYPE_ID_BLOCK(myType, first_custom_type_id)
CAF_ADD_TYPE_ID(myType, (std::vector<int>))
CAF_END_TYPE_ID_BLOCK(myType)

namespace caf
{

    class config : public actor_system_config
    {
    public:
        uint16_t port = 0;
        std::string host = "localhost";
        bool distributedMode = false;
        bool serverMode = false;
        std::string queryInput = "";
        std::string subjectInput = "";
        std::string outputFile = "output.txt";
        int actorNumber = 1;
        int matchScore = 2;
        int mismatchScore = -1;
        int gapScore = -2;
        int dividerRow = 1;
        int dividerCol = 1;

        config()
        {
            // Add the port and host to the config
            opt_group{custom_options_, "global"}
                .add(port, "port,p", "set port")
                .add(host, "host,H", "set host (ignored in server mode)")
                .add(distributedMode, "distributedMode,D", "enable distributed mode")
                .add(serverMode, "serverMode,s", "enable server mode")
                .add(queryInput, "query,Q", "input query sequences")
                .add(subjectInput, "subject,S", "input subject sequences")
                .add(outputFile, "output,O", "output file for results (default: output.txt)")
                .add(actorNumber, "actorNumber,A", "Number of worker actors per each node (default: 1)")
                .add(matchScore, "match,m", "score for a match (default: 2)")
                .add(mismatchScore, "mismatch,M", "penalty for a mismatch (default: -1)")
                .add(gapScore, "gap,g", "penalty for a gap (default: -2)")
                .add(dividerRow, "dividerRow, R", "division counts for scor matrix rows (default: 1)")
                .add(dividerCol, "dividerCol, C", "division counts for scor matrix columns (default: 1)");
        }
    };

} // namespace caf

#endif // CONFIG_HPP