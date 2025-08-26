#include "UDPParser.hpp"
#include <fstream>
#include <iostream>
#include <netinet/in.h>  // for ntohs()

namespace NetworkParser {

Stats UDPParser::req_stats;

UDPParser::UDPParser(std::string _filePath) : filePath(_filePath) {}

Stats UDPParser::parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) {
    req_stats = ip_add_stats;
    Stats placeholder;
    if (length < offset + sizeof(UDPHeader)) {
        //std::cerr << "Error: Malformed UDP packet - insufficient length for UDP header." << std::endl;
        return placeholder;
    }

    const UDPHeader* udpHeader = reinterpret_cast<const UDPHeader*>(packet + offset);

    uint16_t srcPort = ntohs(udpHeader->sourcePort);
    uint16_t destPort = ntohs(udpHeader->destinationPort);


    // Ensure the packet length matches the header's length field
    if (length < offset + ntohs(udpHeader->length)) {
        //std::cerr << "Error: Malformed UDP packet - length mismatch." << std::endl;
        return placeholder;
    }

    // Update statistics
    udpTotalPackets++;
    udpTotalBytes += (length - offset - sizeof(UDPHeader));

    // Update unique ports
    udpUniquePorts.insert(srcPort);
    udpUniquePorts.insert(destPort);

    // Update port stats
    udpPortStats[srcPort].packetsOut++;
    udpPortStats[destPort].packetsIn++;
    udpPortStats[srcPort].bytesOut += (length - offset - sizeof(UDPHeader));
    udpPortStats[destPort].bytesIn += (length - offset - sizeof(UDPHeader));

    // Update unique interactions and connection stats
    auto connection = (srcPort < destPort) ? std::make_pair(srcPort, destPort)
                                           : std::make_pair(destPort, srcPort);

    udpUniqueInteractions.insert(connection);

    if (srcPort < destPort) {
        udpConnectionStats[connection].packetsOut++;
        udpConnectionStats[connection].bytesOut += (length - offset - sizeof(UDPHeader));
    } else {
        udpConnectionStats[connection].packetsIn++;
        udpConnectionStats[connection].bytesIn += (length - offset - sizeof(UDPHeader));
    }

    req_stats = ip_add_stats;
    src_port = srcPort;
    dest_port = destPort;

    return req_stats;
}

size_t UDPParser::getOffset() const {
    return sizeof(UDPHeader);
}

void UDPParser::generateReport() {
    // Generate port stats report
    std::ofstream udpPortStatsFile("output-udp-csv-files/udp-port-stats.csv");
    if (udpPortStatsFile.is_open()) {
        udpPortStatsFile << "unique-port,packetsIn,packetsOut,bytesIn,bytesOut\n";
        for (const auto& [port, stats] : udpPortStats) {
            udpPortStatsFile << port << ","
                             << stats.packetsIn << ","
                             << stats.packetsOut << ","
                             << stats.bytesIn << ","
                             << stats.bytesOut << "\n";
        }
        udpPortStatsFile.close();
    } else {
        std::cerr << "Error: Could not open udp-port-stats.csv for writing.\n";
    }

    // Generate connection stats report
    std::ofstream udpConnectionStatsFile("output-udp-csv-files/udp-connection-stats.csv");
    if (udpConnectionStatsFile.is_open()) {
        udpConnectionStatsFile << "ip1,ip2,srcPort,destPort,packetsIn,packetsOut,bytesIn,bytesOut\n";
        for (const auto& [connection, stats] : udpConnectionStats) {
            udpConnectionStatsFile << req_stats.ip1 << ","
                                   << req_stats.ip2 << ","
                                   << connection.first << ","
                                   << connection.second << ","
                                   << stats.packetsIn << ","
                                   << stats.packetsOut << ","
                                   << stats.bytesIn << ","
                                   << stats.bytesOut << "\n";
        }
        udpConnectionStatsFile.close();
    } else {
        std::cerr << "Error: Could not open udp-connection-stats.csv for writing.\n";
    }

    // Generate general summary report for UDP
    std::ofstream udpSummaryFile("output-udp-csv-files/udp-general-summary.csv");
    if (udpSummaryFile.is_open()) {
        udpSummaryFile << "#packets,bytes,#unique-ports,uniqueConnections\n";
        udpSummaryFile << udpTotalPackets << ","
                       << udpTotalBytes << ","
                       << udpPortStats.size() << ","
                       << udpConnectionStats.size() << "\n";
        udpSummaryFile.close();
    } else {
        std::cerr << "Error: Could not open udp-general-summary.csv for writing.\n";
    }
}

std::string UDPParser::nextParser() const {
    size_t udpHeaderLength = (src_port < dest_port) ? sizeof(UDPHeader) : sizeof(UDPHeader);
    size_t payloadSize = udpTotalBytes - udpHeaderLength;

    if (payloadSize > 0) {
        if (dest_port == 53 || src_port == 53) {
            return "DNS";  // Next parser is DNS
        }
    }

    return "None";
}

} // namespace NetworkParser