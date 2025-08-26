#pragma once
#include "Parser.hpp"
#include "IPParser.hpp"
#include <string>
#include <map>
#include <set>
#include <vector>

namespace NetworkParser {

static std::map<uint16_t, Stats> tcpPortStats;  // Stats per port
static std::map<std::pair<uint16_t, uint16_t>, Stats> tcpConnectionStats;  // Stats per connection
static size_t tcpTotalPackets;
static size_t tcpTotalBytes;
static std::set<uint16_t> tcpUniquePorts;
static std::set<std::pair<uint16_t, uint16_t>> tcpUniqueInteractions; 

class TCPParser : public Parser {
public:
    TCPParser(std::string _filePath = "");
    Stats parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) override;
    std::string nextParser() const override;
    static void generateReport();
    size_t getOffset() const override;

private:
    std::string filePath;
    uint16_t src_port;
    uint16_t dest_port;
    uint8_t tcpHeaderLength;
};

#pragma pack(push, 1)
struct TCPHeader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint32_t sequenceNumber;
    uint32_t acknowledgmentNumber;
    uint8_t dataOffset;  // Header length + Reserved + Flags
    uint8_t flags;
    uint16_t windowSize;
    uint16_t checksum;
    uint16_t urgentPointer;
};
#pragma pack(pop)

} // namespace NetworkParser