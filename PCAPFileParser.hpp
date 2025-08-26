#pragma once

#include <vector>
#include <string>
#include "Ethernet.hpp"

namespace NetworkParser {
class PCAPFileParser {
public:
    PCAPFileParser() : headerParsed(false) {}
    bool parseFile(const std::string& filePath);
    const std::vector<std::vector<uint8_t>>& getPackets() const;

private:
    NetworkParser::PcapGlobalHeader header;
    bool headerParsed;
    std::vector<std::vector<uint8_t>> packets;  // Store packets
};
}