#pragma once
#include "Parser.hpp"
#include <iostream>
#include <fstream>

namespace NetworkParser {
class EthernetParser : public Parser {
public:
    EthernetParser() = default;
    Stats parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) override;
    size_t getOffset() const override;
    std::string nextParser() const override;

private:
    std::string nextProtocol = "IP";
};
#pragma pack(push, 1) // Ensure no padding in structs
// PCAP Global Header
struct PcapGlobalHeader {
    uint32_t magic_number; 
    uint16_t version_major;
    uint16_t version_minor;
    int32_t  thiszone;        
    uint32_t sigfigs; 
    uint32_t snaplen; 
    uint32_t network;  
};

// PCAP Packet Header
struct PcapPacketHeader {
    uint32_t ts_sec;   
    uint32_t ts_usec;  
    uint32_t incl_len;
    uint32_t orig_len;
};

// Ethernet frame header
struct EthernetFrameHeader {
    uint8_t destinationMac[6];
    uint8_t sourceMac[6];
    uint16_t etherType;        
};
#pragma pack(pop)
};