# Network Protocol Analyzer

This project is a modular C++ network analysis tool that parses `.pcap` files and generates detailed CSV reports for each protocol layer, including:

- IPv4
- TCP
- UDP
- HTTP (https://github.com/ishanphadke11/http-parser)
- DNS (https://github.com/ishanphadke11/dns-parser)
- FTP (https://github.com/ishanphadke11/ftp-parser)

The tool is built using object-oriented programming principles and employs the **Factory Design Pattern** to dynamically select the appropriate parser for each packet.

---

## Features

- **Layered Parsing**: Supports parsing of multiple protocol layers.
- **Modular Design**: Application-layer parsers (HTTP, DNS, FTP) are implemented as separate dynamic libraries.
- **Extensibility**: New protocol parsers can be added without modifying the core codebase.
- **CSV Reporting**: Generates structured CSV reports for each protocol layer.
- **Factory Pattern**: Centralized parser creation logic for clean and scalable architecture.

---

## Architecture Overview

### 1. **Parser Base Class**
All protocol parsers inherit from a common `Parser` base class, which defines the interface for parsing packets and generating reports.

### 2. **Derived Parsers**
Each protocol (IPv4, TCP, UDP, etc.) has its own derived parser class that implements the parsing logic specific to that protocol.

### 3. **Parser Factory**
The `ParserFactory` class is responsible for returning the correct parser instance based on the packet's protocol type.

### 4. **Controller Class**
The `Controller` class manages the overall workflow:
- Loads the `.pcap` file
- Iterates through packets
- Uses the factory to instantiate the correct parser
- Delegates parsing and report generation

### 5. **Dynamic Libraries**
Application-layer parsers (HTTP, DNS, FTP) are compiled as separate dynamic libraries. These are loaded at runtime based on a mapping file, allowing seamless integration of new protocols.

---

## Adding New Protocol Parsers

To add a new application-layer parser:
1. Create a new parser class that inherits from `Parser`.
2. Compile it as a dynamic library.
3. Add its entry to the protocol mapping file.
4. No changes are required in the main codebase.

---

## Dependencies

- Standard C++ STL
- Dynamic linking support (`dlopen`, `dlsym` on Unix-like systems)

---

## Future Improvements

I am currenlty working on implementing an LLM supported chatbot so that users can ask questions about the generated reports and get information including visualizations.
