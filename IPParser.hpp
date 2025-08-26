#pragma once
#include "Parser.hpp"
#include <string>
#include <map>
#include <vector>
#include <set>
#include <ctime>
#include <cstdint>
#include <netinet/in.h> // For ntohl and ntohs

namespace NetworkParser {

// Declare global variables
extern std::map<std::string, Stats> ipIndividualStats;
extern std::map<std::string, Stats> ipInteractionStats;
extern std::string ipFirstTimestamp;
extern std::string ipLastTimestamp;
extern size_t ipTotalPackets;
extern size_t ipTotalBytes;

// IPv4 Header structure (packed for alignment)
#pragma pack(push, 1)
struct IPv4Header {
    uint8_t version_internet_header_length;
    uint8_t tos;   
    uint16_t totalLength; 
    uint16_t identification; 
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum; 
    uint32_t sourceIP;
    uint32_t destinationIP; 
};
#pragma pack(pop)

class IPParser : public Parser {
public:
    Stats parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) override;
    std::string nextParser() const override;
    static void generateReport();
    size_t getOffset() const override;
private:
    std::string ipAddToString(const uint32_t ipAdd);
    const IPv4Header* ipHeader;
};

}  // namespace NetworkParser