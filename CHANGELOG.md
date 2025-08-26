# Changelog

All notable changes to the ATEM ESP32 Arduino Library project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2025-08-27

### Added
- Professional Arduino library structure with src/, examples/, test/, extras/
- Arduino Library Manager compliance (library.properties, keywords.txt)
- Comprehensive test suite with 35 test cases
- Dynamic build system with automatic library installation
- Multiple professional examples (ComprehensiveController, BasicATEMControl, AutomatedSwitching)
- Event-driven architecture with callback system
- Complete API documentation and getting started guide
- Hardware integration testing with real ATEM Mini Pro
- Automated regression testing in build process
- Professional repository structure and git workflow

### Changed
- **BREAKING**: Converted from single sketch to professional library structure
- **BREAKING**: Development workflow changed to library-first approach
- **BREAKING**: Root ATEM_ESP32.ino is now informative placeholder
- **BREAKING**: Configuration files moved to example-specific locations
- Updated version numbering from v1.10.1 to v2.0.0 for major release

### Removed
- Direct development in root .ino file (now uses dynamic build system)
- Redundant configuration files (templates now in examples)

## [1.10.1] - 2025-08-26

### Added
- CUT transition functionality (immediate preview to program switching)
- AUTO transition functionality (fade/wipe transitions)
- Comprehensive input switching (CAM1-4, Black, Bars, Color Generators, Media Player)
- Real-time state monitoring and event callbacks

### Fixed
- Complete ATEM protocol rewrite based on Sofie ATEM Connection library
- Packet format: flags in high 5 bits, length in low 11 bits
- HELLO response detection using NewSessionId flag (0x02)
- Session ID extraction from correct header bytes (2-3)
- Robust connection management and error handling

### Improved
- Enhanced debug output with multiple verbosity levels
- Better error handling and validation
- Improved network connectivity management
- Professional code documentation

## [1.0.0] - Initial Release

### Added
- Basic ESP32 ATEM switcher control
- UDP communication with ATEM devices
- WiFi network connectivity
- Basic input switching functionality
- ESP32 hardware support
- Compatible with ATEM Mini, ATEM Mini Pro, and other ATEM models

---

## Migration Guides

### From v1.x to v2.0.0
See [RELEASE_NOTES.md](RELEASE_NOTES.md#migration-guide) for detailed migration instructions.

Key changes:
- Use Arduino Library Manager or copy `library/` folder
- Change includes from `#include "ATEM.h"` to `#include <ATEM.h>`
- Use dynamic build system: `./build_upload_monitor.sh [example]`
- Develop in `library/` folder structure, not root .ino file
