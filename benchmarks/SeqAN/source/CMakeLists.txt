cmake_minimum_required (VERSION 3.5)
project (seqan3_tutorial CXX)
 
set (CMAKE_CXX_FLAGS "-O3 -march=native")
# add seqan3 to search path
list (APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../seqan3/cmake")
 
# require seqan3 with a version between >=3.0.0 and <4.0.0
find_package (seqan3 3.0 REQUIRED)
 
# build app with seqan3
add_executable (SeqAN SeqAN.cpp)
target_link_libraries (SeqAN seqan3::seqan3)