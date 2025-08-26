# ATEM ESP32 Arduino Library - Release Notes

## Version 2.0.0 - Major Release (August 27, 2025)

🎉 **MAJOR RELEASE** - Complete transformation from prototype sketch to professional Arduino library ecosystem.

### 🚀 Major Architecture Changes

**Complete Library Transformation**
- Converted from single-file Arduino sketch to professional Arduino library structure
- Implemented library-first development workflow following Arduino Library Specification 1.5
- All development now happens in `library/` folder structure
- Root `ATEM_ESP32.ino` transformed to informative placeholder with build guidance

**Professional Library Structure**
```
library/
├── src/                    # Core library implementation
│   ├── ATEM.h             # Main library interface
│   ├── ATEM.cpp           # Core ATEM protocol implementation
│   ├── ATEM_Inputs.h      # Input constants and definitions
│   └── ATEM_Models.h      # ATEM model definitions
├── examples/              # Professional examples
│   ├── ComprehensiveController/    # Full-featured controller
│   ├── BasicATEMControl/          # Simple usage example
│   └── AutomatedSwitching/        # Timer-based automation
├── test/                  # Comprehensive test suite
├── extras/               # Additional utilities
└── documentation/        # Complete API reference
```

### 🎯 New Features

**Arduino Library Manager Compliance**
- `library.properties` with proper metadata
- `keywords.txt` for IDE syntax highlighting
- Professional documentation structure
- Ready for Arduino Library Manager submission

**Comprehensive Examples**
- **ComprehensiveController**: Full-featured ATEM controller with serial command interface
- **BasicATEMControl**: Simple usage example for quick start
- **AutomatedSwitching**: Timer-based automated switching demonstration

**Event-Driven Architecture**
- Real-time state monitoring with callback system
- `onConnectionStateChanged()` - Connection status updates
- `onProgramInputChanged()` - Program input change notifications
- `onPreviewInputChanged()` - Preview input change notifications
- `onStateChanged()` - General ATEM state updates

**Dynamic Build System**
- Automatic library installation before builds
- Example-specific configuration management
- Automatic placeholder restoration after builds
- Clean repository state maintenance

### 🧪 Testing Infrastructure

**Comprehensive Test Suite**
- **35 test cases** covering all library functionality
- **Unit tests** for core library functions
- **Integration tests** with real ATEM Mini Pro hardware
- **Regression testing** integrated into build process
- **100% test success rate** required for deployment

**Test Categories**
- Connection and handshake validation
- Input switching functionality
- Transition controls (CUT/AUTO)
- State tracking and synchronization
- Error handling and edge cases
- Protocol compliance verification

**Automated Testing**
- Node.js-based test runner
- Hardware integration test suite
- Continuous regression testing
- Build verification with real hardware

### 🛠️ Development Workflow

**Library-First Development**
- All development in `library/` folder structure
- Professional git workflow
- Automated build and test integration
- Clean repository management

**Build System Enhancement**
```bash
# Dynamic build with automatic library installation
./build_upload_monitor.sh ComprehensiveController

# Different examples
./build_upload_monitor.sh BasicATEMControl
./build_upload_monitor.sh AutomatedSwitching

# Development with monitoring
./build_upload_monitor.sh --monitor
```

**Repository Management**
- Automatic placeholder restoration
- Clean commit history
- Professional documentation
- Arduino Library Manager ready

### 📚 Documentation

**Complete API Documentation**
- `library/README.md` - Complete API reference
- `library/GETTING_STARTED.md` - Step-by-step setup guide
- Individual example documentation
- Professional code comments and examples

**User Guides**
- Installation instructions for Arduino Library Manager
- Manual installation procedures
- Configuration guides
- Troubleshooting documentation

### 🔧 Technical Improvements

**Protocol Implementation**
- Based on proven Sofie ATEM Connection library
- Robust packet handling and retransmission
- Proper session management
- Comprehensive error handling

**Code Quality**
- Professional error handling
- Comprehensive input validation
- Memory-safe implementations
- Static code analysis compliance

**Configuration Management**
- Example-specific configuration files
- Template-based setup
- Environment-specific settings
- Professional configuration documentation

---

## Version 1.10.1 - Protocol Enhancement (August 26, 2025)

### 🔧 Protocol Improvements

**ATEM Protocol Fixes**
- Complete rewrite based on proven Sofie ATEM Connection library
- Fixed packet format: flags in high 5 bits, length in low 11 bits
- Corrected HELLO response detection using NewSessionId flag (0x02)
- Fixed session ID extraction from bytes 2-3 of response header

**Core Functionality**
- ✅ **CUT Transitions** - Immediate preview to program switching
- ✅ **AUTO Transitions** - Fade/wipe transitions between inputs
- ✅ **Input Switching** - Preview and program input control
- ✅ **State Monitoring** - Real-time ATEM state tracking

**Supported Inputs**
- CAM1-CAM4 (Camera inputs)
- Black/Matte
- Color Bars
- Color Generators 1-2
- Media Player 1

### 🎛️ Features

**Connection Management**
- Robust WiFi connection handling
- Automatic ATEM discovery and connection
- Real-time connection state monitoring
- Automatic reconnection on network issues

**Control Interface**
- Serial command interface for testing
- Real-time status reporting
- Comprehensive error handling
- Debug output with multiple verbosity levels

---

## Version 1.0.0 - Initial Release

### 🎯 Core Features

**Basic ATEM Control**
- ESP32-based ATEM switcher control
- UDP communication with ATEM devices
- Basic input switching functionality
- WiFi network connectivity

**Hardware Support**
- ESP32 microcontroller family
- Tested with ESP32-C6 development boards
- Compatible with ATEM Mini, ATEM Mini Pro, and other ATEM models

---

## 🔄 Migration Guide

### From v1.x to v2.0.0

**Breaking Changes**
- Library structure completely reorganized
- Development workflow changed to library-first approach
- Configuration files moved to example-specific locations

**Migration Steps**
1. **Install Library**: Use Arduino Library Manager or copy `library/` folder
2. **Update Includes**: Change `#include "ATEM.h"` to `#include <ATEM.h>`
3. **Use Examples**: Start with `ComprehensiveController` or `BasicATEMControl`
4. **Development**: Make changes in `library/` folder, not root `.ino` file

**New Workflow**
```bash
# Old: Edit ATEM_ESP32.ino directly
# New: Use dynamic build system
./build_upload_monitor.sh [example_name]
```

### 🎉 Benefits of Upgrade

- **Professional Library Structure**: Arduino Library Manager compatible
- **Comprehensive Testing**: 35 test cases ensure reliability
- **Better Documentation**: Complete API reference and guides  
- **Multiple Examples**: Choose the example that fits your needs
- **Automated Workflow**: Build, test, and deploy with single command
- **Future-Proof**: Professional structure for long-term development

---

## 🤝 Contributing

All development happens in the `library/` folder structure:
- **Core Library**: `library/src/`
- **Examples**: `library/examples/`
- **Tests**: `library/test/`
- **Documentation**: `library/README.md`

See our [test suite](library/test/) for quality standards and development workflow.

---

## 📄 License

## 📄 License

This project is licensed under the Apache License 2.0 - see [LICENSE](LICENSE) file for details.

### Third Party Acknowledgments

This library is based on protocol specifications from the [Sofie ATEM Connection](https://github.com/Sofie-Automation/sofie-atem-connection) library (MIT License). See [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md) for complete attribution and license details.

**Made with ❤️ for the ATEM community**
