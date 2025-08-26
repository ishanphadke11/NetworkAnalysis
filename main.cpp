#include <iostream>
#include "Controller.hpp"
#include "Ethernet.hpp"

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <pcap_file>" << std::endl;
        return 1;
    }
    
    std::string pcapFilePath = argv[1];

    try {
        // Initialize the Controller
        NetworkParser::Controller controller;

        // Open the PCAP file and load the packets
        std::cout << "Loading PCAP file: " << pcapFilePath << "..." << std::endl;
        if (!controller.loadPCAPFile(pcapFilePath)) {
            std::cerr << "Failed to load PCAP file: " << pcapFilePath << std::endl;
            return 1;
        }

        // Process the packets
        std::cout << "Processing packets..." << std::endl;
        controller.processPackets();
        std::cout << "Packet processing complete" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}