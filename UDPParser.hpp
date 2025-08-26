#pragma once
#include "Parser.hpp"
#include <string>
#include <map>
#include <set>
#include <vector>

namespace NetworkParser {

static std::map<uint16_t, Stats> udpPortStats;  // Stats per port
static std::map<std::pair<uint16_t, uint16_t>, Stats> udpConnectionStats;  // Stats per connection
static size_t udpTotalPackets;
static size_t udpTotalBytes;
static std::set<uint16_t> udpUniquePorts;
static std::set<std::pair<uint16_t, uint16_t>> udpUniqueInteractions;

class UDPParser : public Parser {
public:
    UDPParser(std::string _filePath = "");
    Stats parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) override;
    std::string nextParser() const override;
    static void generateReport();
    size_t getOffset() const override;

private:
    std::string filePath;
    static Stats req_stats;
    uint16_t src_port;
    uint16_t dest_port;
};

#pragma pack(push, 1)
struct UDPHeader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint16_t length;
    uint16_t checksum;
};
#pragma pack(pop)

} // namespace NetworkParser