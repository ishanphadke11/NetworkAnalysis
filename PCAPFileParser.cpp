#include "PCAPFileParser.hpp"
#include <fstream>
#include <iostream>

namespace NetworkParser {

bool PCAPFileParser::parseFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false; // Failed to open the file
    }

    // Read global header
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (file.gcount() != sizeof(header)) {
        return false; // Failed to read the global header
    }

    headerParsed = true;

    // Parse packets
    while (file) {
        PcapPacketHeader packetHeader;
        file.read(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader));

        if (file.gcount() != sizeof(packetHeader)) {
            break; // End of file or read error
        }

        std::vector<uint8_t> packet(packetHeader.incl_len);
        file.read(reinterpret_cast<char*>(packet.data()), packetHeader.incl_len);

        if (file.gcount() != static_cast<std::streamsize>(packetHeader.incl_len)) {
            break; // End of file or read error
        }

        packets.push_back(std::move(packet));
    }

    return true;
}

const std::vector<std::vector<uint8_t>>& PCAPFileParser::getPackets() const {
    return packets;
}
}