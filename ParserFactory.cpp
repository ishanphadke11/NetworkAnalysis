#include "ParserFactory.hpp"
#include <dlfcn.h>

namespace NetworkParser {

ParserFactory::ParserFactory(const std::unordered_map<std::string, std::string>& map) 
    : libraryMapping(map) {}

std::unique_ptr<Parser> ParserFactory::createParser(const std::string& identifier) {
    // Check built-in parsers first
    if (identifier == "Ethernet") return std::make_unique<EthernetParser>();
    if (identifier == "IP") return std::make_unique<IPParser>();
    if (identifier == "TCP") return std::make_unique<TCPParser>();
    if (identifier == "UDP") return std::make_unique<UDPParser>();

    // Handle dynamic protocols
    return loadParserDynamically(identifier);
}

std::unique_ptr<Parser> ParserFactory::loadParserDynamically(const std::string& identifier) {
    auto libIt = libraryMapping.find(identifier);
    if (libIt == libraryMapping.end()) {
        std::cerr << "No library mapping found for protocol: " << identifier << "\n";
        return nullptr;
    }

    // Check if already loaded
    auto handleIt = loadedLibraries.find(identifier);
    if (handleIt != loadedLibraries.end()) {
        void* handle = handleIt->second;
        using CreateFunc = Parser* (*)();
        CreateFunc create = (CreateFunc)dlsym(handle, "createNewParser");
        if (!create) {
            std::cerr << "Failed to find create function for " << identifier 
                     << ": " << dlerror() << "\n";
            return nullptr;
        }
        return std::unique_ptr<Parser>(create());
    }

    // Load new library
    void* handle = dlopen(libIt->second.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        std::cerr << "Failed to load library for " << identifier 
                 << ": " << dlerror() << "\n";
        return nullptr;
    }

    loadedLibraries[identifier] = handle;
    using CreateFunc = Parser* (*)();
    CreateFunc create = (CreateFunc)dlsym(handle, "createNewParser");
    
    if (!create) {
        std::cerr << "Failed to find create function for " << identifier 
                 << ": " << dlerror() << "\n";
        dlclose(handle);
        return nullptr;
    }

    return std::unique_ptr<Parser>(create());
}

} // namespace NetworkParser