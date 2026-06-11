cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sw-actors LANGUAGES CXX)

find_package(CAF CONFIG REQUIRED COMPONENTS core io)

add_executable(swActor
  src/main.cpp
  src/fasta.cpp
  src/smith_waterman.cpp
  src/actors/client_actor.cpp
  src/actors/server_actor.cpp
  src/actors/worker_actor.cpp
  src/actors/output_actor.cpp
)

target_compile_features(swActor PRIVATE cxx_std_23)

target_include_directories(swActor PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(swActor PRIVATE
  CAF::core
  CAF::io
)
