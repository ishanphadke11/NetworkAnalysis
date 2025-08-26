#include "TCPParser.hpp"
#include <fstream>
#include <iostream>
#include <netinet/in.h> 

namespace NetworkParser {

TCPParser::TCPParser(std::string _filePath) : filePath(_filePath) {}

Stats TCPParser::parsePacket(const uint8_t* packet, size_t length, size_t offset, Stats ip_add_stats) {
    Stats placeholder;
    if (length < offset + sizeof(TCPHeader)) {
        std::cerr << "Error: Malformed TCP packet - insufficient length for TCP header." << std::endl;
        return placeholder;
    }

    const TCPHeader* tcpHeader = reinterpret_cast<const TCPHeader*>(packet + offset);

    uint16_t srcPort = ntohs(tcpHeader->sourcePort);
    uint16_t destPort = ntohs(tcpHeader->destinationPort);

    // Calculate TCP header length (dataOffset is in 32-bit words)
    uint8_t headerLength = (tcpHeader->dataOffset >> 4) * 4;
    tcpHeaderLength = headerLength;

    // Ensure the packet is large enough to contain the TCP header
    if (length < offset + headerLength) {
        std::cerr << "Error: Malformed TCP packet - insufficient length for TCP header." << std::endl;
        return placeholder;
    }

    // Update statistics
    tcpTotalPackets++;
    tcpTotalBytes += (length - offset - headerLength);

    // Update unique ports
    tcpUniquePorts.insert(srcPort);
    tcpUniquePorts.insert(destPort);

    // Update port stats
    tcpPortStats[srcPort].packetsOut++;
    tcpPortStats[destPort].packetsIn++;
    tcpPortStats[srcPort].bytesOut += (length - offset - headerLength);
    tcpPortStats[destPort].bytesIn += (length - offset - headerLength);

    // Update unique interactions and connection stats
    auto connection = (srcPort < destPort) ? std::make_pair(srcPort, destPort)
                                           : std::make_pair(destPort, srcPort);

    tcpUniqueInteractions.insert(connection);

    // Update connection stats with IP addresses
    tcpConnectionStats[connection].ip1 = ip_add_stats.ip1;
    tcpConnectionStats[connection].ip2 = ip_add_stats.ip2;

    if (srcPort < destPort) {
        tcpConnectionStats[connection].packetsOut++;
        tcpConnectionStats[connection].bytesOut += (length - offset - headerLength);
    } else {
        tcpConnectionStats[connection].packetsIn++;
        tcpConnectionStats[connection].bytesIn += (length - offset - headerLength);
    }

    src_port = srcPort;
    dest_port = destPort;

    return ip_add_stats;
}

size_t TCPParser::getOffset() const {
    // The dataOffset field (upper 4 bits) gives the header length in 32-bit words, multiply by 4 for bytes
    return tcpHeaderLength;
}

std::string TCPParser::nextParser() const {
    size_t tcpHeaderLength = (src_port < dest_port) ? sizeof(TCPHeader) : sizeof(TCPHeader);
    size_t payloadSize = tcpTotalBytes - tcpHeaderLength;
    /*
    if (payloadSize > 0) {
        if (dest_port == 80 || dest_port == 8080 || src_port == 80 || src_port == 8080) {
            return "HTTP";  // Next parser is HTTP
        }
    }*/

    std::ifstream tcp_mapping_file("tcp-port-mapping.dat");
    if (!tcp_mapping_file) {
        std::cerr << "Error opening dat file for mapping" << std::endl;
        return "";
    }

    std::string line;
    while(std::getline(tcp_mapping_file, line)) {
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }

        int mappedPort = std::stoi(line.substr(0, equalPos));
        std::string protocol = line.substr(equalPos + 1);

        if (dest_port == mappedPort || src_port == mappedPort) {
            return protocol;
        }
    }

    return "";
}

void TCPParser::generateReport() {
    // Generate port stats report
    std::ofstream tcpPortStatsFile("output-tcp-csv-files/tcp-port-stats.csv");
    if (tcpPortStatsFile.is_open()) {
        tcpPortStatsFile << "unique-port,packetsIn,packetsOut,bytesIn,bytesOut\n";
        for (const auto& [port, stats] : tcpPortStats) {
            tcpPortStatsFile << port << ","
                             << stats.packetsIn << ","
                             << stats.packetsOut << ","
                             << stats.bytesIn << ","
                             << stats.bytesOut << "\n";
        }
        tcpPortStatsFile.close();
    } else {
        std::cerr << "Error: Could not open tcp-port-stats.csv for writing.\n";
    }

    // Generate connection stats report
    std::ofstream tcpConnectionStatsFile("output-tcp-csv-files/tcp-connection-stats.csv");
    if (tcpConnectionStatsFile.is_open()) {
        tcpConnectionStatsFile << "ip1,ip2,srcPort,destPort,packetsIn,packetsOut,bytesIn,bytesOut\n";
        for (const auto& [connection, stats] : tcpConnectionStats) {
            tcpConnectionStatsFile << stats.ip1 << ","
                                   << stats.ip2 << ","
                                   << connection.first << ","
                                   << connection.second << ","
                                   << stats.packetsIn << ","
                                   << stats.packetsOut << ","
                                   << stats.bytesIn << ","
                                   << stats.bytesOut << "\n";
        }
        tcpConnectionStatsFile.close();
    } else {
        std::cerr << "Error: Could not open tcp-connection-stats.csv for writing.\n";
    }

    // Generate general summary report for TCP
    std::ofstream tcpSummaryFile("output-tcp-csv-files/tcp-general-summary.csv");
    if (tcpSummaryFile.is_open()) {
        tcpSummaryFile << "#packets,bytes,#unique-ports,uniqueConnections\n";
        tcpSummaryFile << tcpTotalPackets << ","
                       << tcpTotalBytes << ","
                       << tcpPortStats.size() << ","
                       << tcpConnectionStats.size() << "\n";
        tcpSummaryFile.close();
    } else {
        std::cerr << "Error: Could not open tcp-general-summary.csv for writing.\n";
    }
}

} // namespace NetworkParser