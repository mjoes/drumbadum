CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRCS = drumbadum.cpp
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
