# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -Iinclude -I dependencies/install/caf/include -L dependencies/install/caf/lib64

# Libraries
LIBS = -lcaf_core -lcaf_io

# Source files (organized under src/)
SRCS = src/main.cpp \
       src/actors/managerActor.cpp \
       src/utils/readFasta.cpp \
       src/utils/makePairs.cpp \
       src/utils/alignerInfo.cpp \
       src/actors/clientActor.cpp \
       src/actors/serialActor.cpp \
       src/actors/pairActor.cpp \
       src/actors/blockActor.cpp\
       src/actors/outputActor.cpp

# Target executable (store in build/)
TARGET = build/swActor

# Set library path
export LD_LIBRARY_PATH := dependencies/install/caf/lib64:$(LD_LIBRARY_PATH)

# Build rule
all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	rm -rf build/
