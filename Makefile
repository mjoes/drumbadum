CXX = g++
CXXFLAGS = -std=c++11 -Wall -g
SRCS = drumbadum.cpp envelopes.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = drumbadum

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
