CXX = g++
# Changed -O3 to -O0 (no optimization) for instant compiling
# Added -pipe to use more RAM and less disk during build
CXXFLAGS = -std=c++20 -O0 -pipe -I. 
LDFLAGS = -lpthread -lssl -lcrypto -lrt

SRCS = main.cpp concurrency/RWLock.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = graph_engine

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
