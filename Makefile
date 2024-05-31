CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -I ./utils/ -I ./pattern/ -pg -g
SRCS = drumbadum.cpp ./utils/envelopes.cpp ./utils/utils.cpp ./utils/global.cpp ./pattern/rhythmic_pattern.cpp
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
