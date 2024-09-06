# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -O3 -I/globalhome/pma753/HPC/root/install/include

# Linker flags
LDFLAGS = -L/globalhome/pma753/HPC/root/install/lib64

# Libraries
LIBS = -lcaf_core -lcaf_io

# Source files
SRCS = SW-Actor.cpp readFasta.cpp makePairs.cpp

# Target executable
TARGET = swActor

# Set library path
export LD_LIBRARY_PATH := /globalhome/pma753/HPC/root/install/lib64:$(LD_LIBRARY_PATH)

# Build rule
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	rm -f $(TARGET)
