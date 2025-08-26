#include "IPParser.hpp"
#include <iostream>
#include <fstream>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace NetworkParser {

std::map<std::string, Stats> ipIndividualStats;
std::map<std::string, Stats> ipInteractionStats;
std::string ipFirstTimestamp;
std::string ipLastTimestamp;
size_t ipTotalPackets = 0;
size_t ipTotalBytes = 0;


Stats IPParser::parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) {
    Stats placeholder;
    if (length < offset + sizeof(IPv4Header)) {
        std::cerr << "Error: Malformed IP packet - insufficient length for IPv4 header." << std::endl;
        return placeholder;
    }

    ipHeader = reinterpret_cast<const IPv4Header*>(packet + offset);

    uint8_t version = (ipHeader->version_internet_header_length >> 4) & 0x0F;
    uint8_t IHL = (ipHeader->version_internet_header_length) & 0x0F; 
    uint16_t totalLength = ntohs(ipHeader->totalLength);
    uint8_t headerLengthInBytes = IHL * 4;

    // Validate the version
    if (version != 4) {
        std::cerr << "Error: Invalid IP version (" << static_cast<int>(version) << "). Expected 4 (IPv4)." << std::endl;
        return placeholder;
    }

    // Validate the IHL (must be at least 5, as the minimum IPv4 header size is 20 bytes)
    if (IHL < 5 || headerLengthInBytes > length - offset) {
        std::cerr << "Error: Malformed IP packet - invalid header length." << std::endl;
        return placeholder;
    }

    // Validate the total length (must be at least the header length and not exceed the packet length)
    if (totalLength < headerLengthInBytes || totalLength > length - offset) {
        std::cerr << "Error: Malformed IP packet - invalid total length." << std::endl;
        return placeholder;
    }

    // Convert source and destination IP addresses to string format
    uint32_t sourceIP = ntohl(ipHeader->sourceIP);
    std::string srcIpStr = ipAddToString(sourceIP);

    uint32_t destIP = ntohl(ipHeader->destinationIP);
    std::string destIpStr = ipAddToString(destIP);

    // Update global statistics
    ipTotalPackets++;
    ipTotalBytes += (totalLength - headerLengthInBytes);



    // Update individual IP statistics
    ipIndividualStats[srcIpStr].packetsOut++;
    ipIndividualStats[srcIpStr].bytesOut += (totalLength - headerLengthInBytes);
    ipIndividualStats[destIpStr].packetsIn++;
    ipIndividualStats[destIpStr].bytesIn += (totalLength - headerLengthInBytes);

    // Update interaction statistics
    std::string interactionKey = srcIpStr + "<--->" + destIpStr;
    ipInteractionStats[interactionKey].packetsOut++;
    ipInteractionStats[interactionKey].bytesOut += (totalLength);
    
    placeholder.ip1 = srcIpStr;
    placeholder.ip2 = destIpStr;
    return placeholder;
}

size_t IPParser::getOffset() const {
    return sizeof(IPv4Header);
}

std::string IPParser::nextParser() const {
    uint8_t protocol = ipHeader->protocol;
    return (protocol == 6) ? "TCP" : "UDP";
}

std::string IPParser::ipAddToString(const uint32_t ipAdd) {
    std::string ipString = std::to_string((ipAdd >> 24) & 0xFF) + "." +
                           std::to_string((ipAdd >> 16) & 0xFF) + "." +
                           std::to_string((ipAdd >> 8) & 0xFF) + "." +
                           std::to_string(ipAdd & 0xFF);
    return ipString;
}


void IPParser::generateReport() {
    // Generate IP individual stats report
    std::ofstream ipStatsFile("output-ip-csv-files/ip-individual-stats.csv");
    if (ipStatsFile.is_open()) {
        ipStatsFile << "ipAddress,packetsIn,packetsOut,bytesIn,bytesOut\n";
        for (const auto& [ipAddress, stats] : ipIndividualStats) {
            ipStatsFile << ipAddress << ","
                        << stats.packetsIn << ","
                        << stats.packetsOut << ","
                        << stats.bytesIn << ","
                        << stats.bytesOut << "\n";
        }
        ipStatsFile.close();
    } else {
        std::cerr << "Error: Could not open ip-individual-stats.csv for writing.\n";
    }

    // Generate IP interaction stats report
    std::ofstream ipInteractionStatsFile("output-ip-csv-files/ip-interaction-stats.csv");
    if (ipInteractionStatsFile.is_open()) {
    ipInteractionStatsFile << "srcIp,destIp,packetsIn,packetsOut,bytesIn,bytesOut\n";
    for (const auto& [interaction, stats] : ipInteractionStats) {
        // Find the separator "<--->" in the interaction string
        size_t separatorPos = interaction.find("<--->");
        if (separatorPos != std::string::npos) {
            // Extract the source IP and destination IP
            std::string srcIp = interaction.substr(0, separatorPos);
            std::string destIp = interaction.substr(separatorPos + 5); // Skip the separator length

            // Write to the CSV file
            ipInteractionStatsFile << srcIp << ","
                                   << destIp << ","
                                   << stats.packetsIn << ","
                                   << stats.packetsOut << ","
                                   << stats.bytesIn << ","
                                   << stats.bytesOut << "\n";
        }
    }
    ipInteractionStatsFile.close();
} else {
    std::cerr << "Error: Could not open ip-interaction-stats.csv for writing.\n";
}

    // Generate general summary report for IP
    std::ofstream ipSummaryFile("output-ip-csv-files/ip-general-summary.csv");
    if (ipSummaryFile.is_open()) {
        ipSummaryFile << "#packets,bytes,#unique-ips,uniqueInteractions\n";
        ipSummaryFile << ipTotalPackets << ","
                      << ipTotalBytes << ","
                      << ipIndividualStats.size() << ","
                      << ipInteractionStats.size() << "\n";
        ipSummaryFile.close();
    } else {
        std::cerr << "Error: Could not open ip-general-summary.csv for writing.\n";
    }
}

} // namespace NetworkParser