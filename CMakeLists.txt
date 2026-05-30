cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sw-actors LANGUAGES CXX)

find_package(CAF CONFIG REQUIRED COMPONENTS core io)

add_executable(swActor
  src/main.cpp
  src/actors/managerActor.cpp
  src/actors/clientActor.cpp
  src/actors/serialActor.cpp
  src/actors/pairActor.cpp
  src/actors/blockActor.cpp
  src/actors/outputActor.cpp
  src/utils/readFasta.cpp
  src/utils/makePairs.cpp
  src/utils/alignerInfo.cpp
)

target_compile_features(swActor PRIVATE cxx_std_23)

target_include_directories(swActor PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(swActor PRIVATE
  CAF::core
  CAF::io
)
