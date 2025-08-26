#include "Ethernet.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>
#include "IPParser.hpp"

namespace NetworkParser {

Stats EthernetParser::parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) {
    if (length < offset + sizeof(EthernetFrameHeader)) {
        std::cerr << "Error: Malformed Ethernet packet - insufficient length." << std::endl;
        return ip_add_stats;
    }

    const EthernetFrameHeader* ethHeader = reinterpret_cast<const EthernetFrameHeader*>(packet + offset);
    uint16_t ethType = ntohs(ethHeader->etherType);

    // Determine the next protocol to parse
    if (ethType == 0x0800) {
        nextProtocol = "IP";
    } else {
        nextProtocol = "";
    }

    return ip_add_stats;
}

size_t EthernetParser::getOffset() const {
    return sizeof(EthernetFrameHeader); // Ethernet header is always 14 bytes
}

std::string EthernetParser::nextParser() const {
    return nextProtocol;
}

} // namespace NetworkParser