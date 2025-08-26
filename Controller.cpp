#include "Controller.hpp"
#include "IPParser.hpp"
#include "TCPParser.hpp"
#include "UDPParser.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <dlfcn.h>

namespace NetworkParser {

std::unordered_map<std::string, std::string> Controller::libraryMapping;

Controller::Controller() {
    std::ifstream mappingFile("parser-mapping.dat");
    if (!mappingFile) {
        std::cerr << "Couldn't open mapping file" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(mappingFile, line)) {
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) continue;

        std::string protocol = line.substr(0, equalPos);
        std::string libPath = line.substr(equalPos + 1);
        libraryMapping[protocol] = libPath;
    }

    parserFactory = std::make_unique<ParserFactory>(libraryMapping);
}

Controller::~Controller() {
    // Close all loaded libraries
    for (void* handle : loadedHandles) {
        if (handle) dlclose(handle);
    }
}

bool Controller::loadPCAPFile(const std::string& filePath) {
    _filePath = filePath;
    if (!fileParser.parseFile(filePath)) {
        std::cerr << "Failed to parse PCAP file: " << filePath << std::endl;
        return false;
    }
    return true;
}

void Controller::processPackets() {
    const auto& packets = fileParser.getPackets();
    if (packets.empty()) {
        std::cerr << "No packets found in the file.\n";
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    int count = 0;

    for (size_t i = 0; i < packets.size(); i++) {
        const auto& packet = packets[i];
        std::string protocol = "Ethernet";
        const uint8_t* currentPacketData = packet.data();
        size_t currentPacketLength = packet.size();
        size_t offset = 0;
        Stats placeholder, ph1;

        while (!protocol.empty() && protocol != "None") {
            std::unique_ptr<Parser> parser = parserFactory->createParser(protocol);
            if (!parser) {
                std::cerr << "Error: Failed to create parser for protocol: " << protocol << "\n";
                break;
            }

            // Validate offset and length
            if (offset >= currentPacketLength) {
                //std::cerr << "Error: Offset exceeds packet length\n";
                break;
            }

            ph1 = parser->parsePacket(currentPacketData, currentPacketLength, offset, placeholder);
            placeholder = ph1;
            protocol = parser->nextParser();
            offset += parser->getOffset();
        }
        count++;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;

    // Generate core reports
    IPParser::generateReport();
    TCPParser::generateReport();
    UDPParser::generateReport();
    
    // Generate dynamic protocol reports
    generateReportsDynamically();

    // Performance metrics
    if (elapsedTime.count() > 0) {
        double packetsPerSecond = packets.size() / elapsedTime.count();
        std::cout << "Total Packets: " << count << std::endl;
        std::cout << "Elapsed time: " << elapsedTime.count() << " seconds\n";
        std::cout << "Processing Speed: " << packetsPerSecond << " packets per second\n";
    }
}

void Controller::generateReportsDynamically() {
    for (const auto& [proto, libPath] : libraryMapping) {
        void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Failed to load library for " << proto << ": " << dlerror() << "\n";
            continue;
        }

        using GenerateReportFunc = void (*)();
        GenerateReportFunc genReport = (GenerateReportFunc)dlsym(handle, "genReport");
        
        if (genReport) {
            //std::cout << "Generating report for " << proto << "...\n";
            genReport();
        } else {
            std::cerr << "Failed to find report function for " << proto 
                     << ": " << dlerror() << "\n";
        }
    }
}

} // namespace NetworkParser