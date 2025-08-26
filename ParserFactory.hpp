#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "Parser.hpp"
#include "Ethernet.hpp"
#include "IPParser.hpp"
#include "TCPParser.hpp"
#include "UDPParser.hpp"



namespace NetworkParser {

class ParserFactory {
public:
    explicit ParserFactory(const std::unordered_map<std::string, std::string>& map);
    std::unique_ptr<Parser> createParser(const std::string& identifier);

private:
    std::unordered_map<std::string, std::string> libraryMapping;
    std::unordered_map<std::string, void*> loadedLibraries;
    
    std::unique_ptr<Parser> loadParserDynamically(const std::string& identifier);
};

} // namespace NetworkParser