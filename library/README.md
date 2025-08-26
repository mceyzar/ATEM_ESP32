# ATEM ESP32 Library

A comprehensive Arduino library for controlling Blackmagic Design ATEM video switchers using ESP32 microcontrollers.

## Features

- **Full ATEM Protocol Support**: Based on the proven Sofie ATEM Connection implementation
- **Input Switching**: Control preview and program inputs across all ATEM models
- **Transitions**: Perform CUT (immediate) and AUTO (fade/wipe) transitions
- **Real-time Monitoring**: Track connection state and input changes
- **Universal Compatibility**: Works with ATEM Mini, ATEM Mini Pro, and all ATEM models
- **Event-Driven Architecture**: Respond to ATEM state changes with custom handlers
- **Network Communication**: Robust UDP communication with automatic reconnection
- **Comprehensive Logging**: Configurable debug levels for development and troubleshooting

## Supported ATEM Models

✅ **Tested and Verified:**
- ATEM Mini Pro
- ATEM Mini

🔧 **Compatible (based on protocol):**
- ATEM Mini Extreme
- ATEM Mini Extreme ISO
- ATEM Television Studio HD
- ATEM Television Studio Pro HD
- ATEM Television Studio Pro 4K
- ATEM Constellation HD/4K
- ATEM 1 M/E Production Studio 4K
- ATEM 2 M/E Production Studio 4K
- And more...

## Hardware Requirements

- **ESP32 Development Board** (ESP32C6, ESP32-S3, ESP32-WROOM, etc.)
- **ATEM Video Switcher** connected to the same network
- **WiFi Network** accessible by both devices

## Installation

### Method 1: Arduino Library Manager (Recommended)
1. Open Arduino IDE
2. Go to **Tools** → **Manage Libraries**
3. Search for "ATEM ESP32"
4. Click **Install**

### Method 2: Manual Installation
1. Download the latest release from [GitHub](https://github.com/mceyzar/ATEM_ESP32)
2. Extract the ZIP file
3. Copy the `ATEM_ESP32` folder to your Arduino libraries directory:
   - **Windows**: `Documents\Arduino\libraries\`
   - **macOS**: `~/Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`
4. Restart Arduino IDE

## Quick Start

### 1. Basic Setup

```cpp
#include <ATEM.h>

// Create ATEM controller
ATEM myAtem;

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin("Your_WiFi_SSID", "Your_WiFi_Password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  // Connect to ATEM
  myAtem.begin("192.168.1.100"); // Your ATEM's IP address
}

void loop() {
  myAtem.loop(); // Keep ATEM connection alive
}
```

### 2. Control Inputs

```cpp
// Switch preview to Camera 2
myAtem.changePreviewInput(ATEM_INPUT_CAM2);

// Switch program to Color Bars
myAtem.changeProgramInput(ATEM_INPUT_BARS);

// Perform immediate CUT transition
myAtem.cut();

// Perform AUTO transition (fade/wipe)
myAtem.autoTransition();
```

### 3. Monitor State Changes

```cpp
class MyATEMHandler : public ATEM {
public:
  void onConnectionStateChanged(ATEMConnectionState state) override {
    if (state == ATEM_CONNECTED) {
      Serial.println("ATEM Connected!");
    }
  }
  
  void onProgramInputChanged(uint16_t input) override {
    Serial.print("Program changed to: ");
    Serial.println(input);
  }
  
  void onPreviewInputChanged(uint16_t input) override {
    Serial.print("Preview changed to: ");
    Serial.println(input);
  }
};

MyATEMHandler myAtem; // Use custom handler instead of base ATEM class
```

## Input Constants

The library provides convenient constants for all ATEM inputs:

```cpp
ATEM_INPUT_BLACK    // Black/Matte
ATEM_INPUT_CAM1     // Camera 1
ATEM_INPUT_CAM2     // Camera 2
ATEM_INPUT_CAM3     // Camera 3
ATEM_INPUT_CAM4     // Camera 4
ATEM_INPUT_BARS     // Color Bars
ATEM_INPUT_COLOR1   // Color Generator 1
ATEM_INPUT_COLOR2   // Color Generator 2
ATEM_INPUT_MP1      // Media Player 1
ATEM_INPUT_MP2      // Media Player 2
// ... and many more
```

## API Reference

### Core Methods

#### `begin(const char* ip)`
Initialize connection to ATEM switcher at specified IP address.

#### `loop()`
Must be called repeatedly in main loop to maintain connection and process packets.

#### `getConnectionState()`
Returns current connection state: `ATEM_DISCONNECTED`, `ATEM_CONNECTING`, `ATEM_CONNECTED`, or `ATEM_ERROR`.

#### `getState()`
Returns current ATEM state including program/preview inputs.

### Control Methods

#### `changePreviewInput(uint16_t input)`
Set the preview input on the main Mix Effect (ME).

#### `changeProgramInput(uint16_t input)`
Set the program input on the main Mix Effect (ME).

#### `cut()`
Perform an immediate CUT transition (preview becomes program).

#### `autoTransition()`
Perform an AUTO transition with the current transition effect (fade, wipe, etc.).

### Event Handlers (Override in Subclass)

#### `onConnectionStateChanged(ATEMConnectionState state)`
Called when connection state changes.

#### `onProgramInputChanged(uint16_t input)`
Called when program input changes.

#### `onPreviewInputChanged(uint16_t input)`
Called when preview input changes.

#### `onStateChanged()`
Called when any ATEM state changes.

## Examples

The library includes several examples:

- **[ComprehensiveController](examples/ComprehensiveController/)**: Advanced controller with full serial interface (also serves as regression test firmware)
- **[BasicATEMControl](examples/BasicATEMControl/)**: Complete controller with serial commands  
- **[SimpleInputSwitching](examples/SimpleInputSwitching/)**: Minimal input switching example
- **[AutomatedSwitching](examples/AutomatedSwitching/)**: Timer-based automated switching

### Recommended Starting Point

For new users, start with **ComprehensiveController** - it demonstrates all library features and includes extensive documentation and error handling.

## Configuration

### WiFi and Network
```cpp
// Standard WiFi connection
WiFi.begin("Your_SSID", "Your_Password");

// ATEM connection
myAtem.begin("192.168.1.100"); // Replace with your ATEM's IP
```

### Debug Logging
```cpp
// Enable debug logging (before including ATEM.h)
#define ATEM_DEFAULT_LOG_LEVEL ATEM_LOG_DEBUG
#include <ATEM.h>
```

Available log levels:
- `ATEM_LOG_NONE` - No logging
- `ATEM_LOG_ERROR` - Errors only
- `ATEM_LOG_WARN` - Warnings and errors
- `ATEM_LOG_INFO` - General information
- `ATEM_LOG_DEBUG` - Detailed debugging
- `ATEM_LOG_VERBOSE` - Everything including packets

## Troubleshooting

### Connection Issues
1. **Verify Network**: Ensure ESP32 and ATEM are on the same network
2. **Check IP Address**: Confirm ATEM's IP using ATEM Software Control
3. **Firewall**: Disable firewall or allow UDP port 9910
4. **Power Cycle**: Restart both ATEM and ESP32

### Serial Monitor Tips
```cpp
// Enable debug output to see detailed connection info
#define ATEM_DEFAULT_LOG_LEVEL ATEM_LOG_DEBUG
```

### Common Error Messages
- `"Connection failed"` - Check IP address and network connectivity
- `"Packet timeout"` - Network congestion or wrong IP
- `"Authentication failed"` - ATEM may be in use by another controller

## Performance

- **Connection Time**: Typically 2-3 seconds
- **Command Latency**: ~50ms for input switching
- **Transition Speed**: Immediate for CUT, configurable for AUTO
- **Memory Usage**: ~15KB RAM, ~45KB Flash

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup
1. Clone the repository
2. Use the provided build scripts for testing
3. Run regression tests before submitting PRs

## Testing

This library includes a comprehensive test suite following Arduino library best practices:

### Quick Test
```bash
# Run fast unit tests (no hardware required)
cd library/test
node library_test_suite.js --unit-only
```

### Full Test Suite
```bash
# Run all tests including hardware integration
node library_test_suite.js --hardware
```

### Test Categories
- **Unit Tests**: Fast mock-based tests (< 1 second)
- **Integration Tests**: Hardware tests with real ATEM devices
- **Regression Tests**: Full protocol validation suite

See `test/README.md` for detailed testing documentation.

## License

## 📄 License

This project is licensed under the Apache License 2.0 - see [LICENSE](../LICENSE) file for details.

### Third Party Acknowledgments

This library is based on protocol specifications from the [Sofie ATEM Connection](https://github.com/Sofie-Automation/sofie-atem-connection) library (MIT License). See [THIRD_PARTY_LICENSES.md](../THIRD_PARTY_LICENSES.md) for complete attribution and license details.

## Acknowledgments

- Based on the [Sofie ATEM Connection](https://github.com/nrkno/sofie-atem-connection) protocol implementation
- Inspired by the ATEM community and Blackmagic Design's excellent documentation
- Thanks to all contributors and testers

## Support

- **Documentation**: [GitHub Wiki](https://github.com/mceyzar/ATEM_ESP32/wiki)
- **Issues**: [GitHub Issues](https://github.com/mceyzar/ATEM_ESP32/issues)
- **Discussions**: [GitHub Discussions](https://github.com/mceyzar/ATEM_ESP32/discussions)

---

**Made with ❤️ for the ATEM community**
