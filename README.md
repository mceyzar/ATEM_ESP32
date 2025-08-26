# ATEM ESP32 Arduino Library

Professional Arduino library for controlling Blackmagic Design ATEM video switchers using ESP32 microcontrollers.

> **📢 Version 2.0.0 Released!** - Complete professional library transformation. See [Release Notes](RELEASE_NOTES.md) and [Changelog](CHANGELOG.md) for details.

## 🚀 Quick Start

### Installation

1. **Arduino Library Manager** (Recommended):
   ```
   Library Manager → Search "ATEM ESP32" → Install
   ```

2. **Manual Installation**:
   - Download the `library/` folder
   - Copy to your Arduino libraries directory

### Basic Usage

```cpp
#include <ATEM.h>
#include <WiFi.h>

ATEM atem;

void setup() {
  WiFi.begin("YOUR_WIFI", "PASSWORD");
  atem.begin(IPAddress(192, 168, 1, 240)); // Your ATEM IP
}

void loop() {
  atem.runLoop();
  
  // Change preview to CAM2
  atem.changePreviewInput(ATEM_INPUT_CAM2);
  
  // Perform CUT transition
  atem.cut();
}
```

## 📋 Features

- ✅ **Complete ATEM Protocol Support** - Based on proven Sofie ATEM Connection
- ✅ **Input Switching** - Preview/Program control for all inputs
- ✅ **Transitions** - CUT (immediate) and AUTO (fade/wipe) transitions
- ✅ **Universal Compatibility** - Works with all ATEM models
- ✅ **Event-Driven** - Real-time state monitoring with callbacks
- ✅ **Professional Testing** - Comprehensive unit and integration tests

## 🎛️ Example Application

See the **[Comprehensive Controller Example](library/examples/ComprehensiveController/)** for a complete ATEM controller with serial command interface:

```
preview 2      → Set preview to CAM2
program bars   → Set program to Color Bars
cut           → Immediate transition
auto          → Fade/wipe transition
status        → Show current state
```

This example serves as both a working controller and the test firmware for regression testing.

## 📚 Documentation

- **[Library Documentation](library/README.md)** - Complete API reference
- **[Getting Started Guide](library/GETTING_STARTED.md)** - Step-by-step setup
- **[Examples](library/examples/)** - Ready-to-use code examples

## 🧪 Testing

Professional test suite with unit and hardware integration tests:

```bash
cd library/test
node library_test_suite.js --unit-only    # Fast unit tests
node library_test_suite.js --hardware     # Full hardware validation
```

## 🛠️ Development

**All development happens in the `library/` folder structure:**

- **Source Code**: `library/src/` - Core library implementation
- **Examples**: `library/examples/` - Reference implementations and demos
- **Tests**: `library/test/` - Comprehensive test suite
- **Documentation**: `library/README.md` - Complete API reference

**Development Workflow**:
```bash
# Build and test with dynamic example system
./build_upload_monitor.sh ComprehensiveController

# Run different examples
./build_upload_monitor.sh BasicATEMControl
./build_upload_monitor.sh AutomatedSwitching

# Development with monitoring
./build_upload_monitor.sh --monitor
```

The build system automatically installs the latest library code and runs comprehensive regression tests (35 test cases, 100% pass rate required).

## 🤝 Contributing

**All development happens in the `library/` folder!** 

- **New Features**: Add to `library/src/`
- **Examples**: Create in `library/examples/`
- **Tests**: Add to `library/test/`
- **Bug Fixes**: Modify `library/src/` files

The root `ATEM_ESP32.ino` is dynamically generated during builds. See our [test suite](library/test/) for quality standards and the dynamic build system for development workflow.

## 📄 License

## 📄 License

This project is licensed under the Apache License 2.0 - see [LICENSE](LICENSE) file for details.

### Third Party Acknowledgments

This library is based on protocol specifications from the [Sofie ATEM Connection](https://github.com/Sofie-Automation/sofie-atem-connection) library (MIT License). See [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md) for complete attribution and license details.

MIT License - see [LICENSE](LICENSE) file for details.

---

**Made with ❤️ for the ATEM community**
