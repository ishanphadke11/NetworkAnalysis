# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -g

# Source files and output
SRCS = IPParser.cpp Ethernet.cpp main.cpp Controller.cpp ParserFactory.cpp PCAPFileParser.cpp TCPParser.cpp UDPParser.cpp
HEADERS = IPParser.hpp Ethernet.hpp Parser.hpp ParserFactory.hpp TCPParser.hpp PCAPFileParser.hpp Controller.hpp UDPParser.hpp
TARGET = Parser

# Build target
$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Clean up build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: clean