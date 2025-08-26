#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "PCAPFileParser.hpp"
#include "ParserFactory.hpp"

namespace NetworkParser {

class Controller {
public:
    Controller();
    ~Controller();
    bool loadPCAPFile(const std::string& filePath);
    void processPackets();

private:
    PCAPFileParser fileParser;
    std::unique_ptr<ParserFactory> parserFactory;
    std::string _filePath;
    static std::unordered_map<std::string, std::string> libraryMapping;
    
    std::vector<void*> loadedHandles;  // Track all loaded library handles
    
    void generateReportsDynamically();
    void loadProtocolLibraries();
};

} // namespace NetworkParser