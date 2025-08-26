# ATEM ESP32 Library Test Suite

This directory contains the comp### Integration Tests (Requires hardware)

1. **Configure Hardware Settings**:
   Edit `extras/test/test_hardware_integration.cpp`:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   const char* ATEM_IP = "192.168.1.240";  // Your ATEM IP
   ```

2. **Run Tests**:
   ```bash
   cd library/test
   pio test -e esp32_hardware

   # Using test runner script
   node library_test_suite.js --hardware
   ```

### Regression Tests (Full Protocol Validation)

The comprehensive regression test suite validates all ATEM protocol functionality:

```bash
cd library/extras/test
./run_tests.sh regression

# Or from main test suite
cd library/test  
node library_test_suite.js --hardware  # Includes regression tests
```

**Note**: Regression tests require:
- ESP32 device with ATEM ESP32 firmware uploaded
- Physical ATEM switcher connected to network
- Node.js with serialport dependencieste for the ATEM ESP32 Arduino library, following Arduino library testing best practices.

## Test Categories

### 1. Unit Tests (`test/`)
- **Purpose**: Fast, isolated tests of core functionality without hardware dependencies
- **Framework**: Unity Test Framework with PlatformIO
- **Environment**: Native (runs on development machine)
- **Characteristics**:
  - Mock-based testing
  - No network or hardware requirements
  - Fast execution (< 1 second)
  - Suitable for CI/CD pipelines
  - Tests core logic, input validation, state management

### 2. Integration Tests (`extras/test/`)
- **Purpose**: End-to-end testing with real ESP32 and ATEM hardware
- **Framework**: Unity Test Framework on ESP32
- **Environment**: ESP32 with actual ATEM switcher
- **Characteristics**:
  - Requires physical hardware setup
  - Tests actual network communication
  - Validates real ATEM protocol implementation
  - Longer execution time (several minutes)
  - Hardware compatibility validation

### 3. Hardware Tests (within Integration Tests)
- **Purpose**: Multi-device compatibility testing
- **Scope**: Different ATEM models, network conditions, edge cases
- **Execution**: Manual or automated with multiple ATEM devices

## Test Structure

```
library/
├── test/                           # Unit tests (PlatformIO/Arduino IDE 2.0)
│   ├── test_atem_core.cpp         # Core functionality tests
│   ├── platformio.ini             # PlatformIO test configuration
│   ├── test_runner.js             # Node.js test runner script
│   └── README.md                  # This file
└── extras/
    └── test/                      # Integration tests
        └── test_hardware_integration.cpp
```

## Running Tests

### Prerequisites

1. **For Unit Tests**: PlatformIO CLI installed
2. **For Integration Tests**: ESP32 device + ATEM switcher + WiFi network

### Unit Tests (Recommended for development)

```bash
# Run all unit tests
cd library/test
pio test -e native

# Using test runner script
node test_runner.js --unit-only
```

### Integration Tests (Requires hardware)

1. **Configure Hardware Settings**:
   Edit `extras/test/test_hardware_integration.cpp`:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   const char* ATEM_IP = "192.168.1.240";  // Your ATEM IP
   ```

2. **Run Tests**:
   ```bash
   cd library/test
   pio test -e esp32_hardware

   # Using test runner script
   node test_runner.js --hardware
   ```

### All Tests

```bash
# Run unit tests + integration tests
node test_runner.js --hardware
```

## Test Coverage

### Unit Tests Cover:
- ✅ Basic initialization and configuration
- ✅ Input constant definitions
- ✅ State management and tracking  
- ✅ Command generation (mock validation)
- ✅ Error handling and edge cases
- ✅ Event callback system
- ✅ Connection state management

### Integration Tests Cover:
- ✅ Real ATEM hardware connection
- ✅ WiFi connectivity and network communication
- ✅ Preview input changes with hardware feedback
- ✅ Program input changes with hardware feedback  
- ✅ CUT transition execution and verification
- ✅ AUTO transition execution and verification
- ✅ Rapid command execution and queuing
- ✅ Real-time state synchronization

## CI/CD Integration

### GitHub Actions Example

```yaml
name: ATEM Library Tests

on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Setup PlatformIO
        run: pip install platformio
      - name: Run Unit Tests
        run: |
          cd library/test
          pio test -e native
  
  # Hardware tests would require self-hosted runners with ATEM hardware
```

### Local Development Workflow

1. **During Development**: Run unit tests frequently
   ```bash
   node test_runner.js -u
   ```

2. **Before Commits**: Run full test suite
   ```bash
   node test_runner.js --hardware
   ```

3. **Release Testing**: Run on multiple ATEM models if available

## Adding New Tests

### Unit Test Pattern
```cpp
void test_new_feature(void) {
    // Arrange
    MockATEM mockAtem;
    
    // Act
    bool result = mockAtem.newFeature();
    
    // Assert
    TEST_ASSERT_TRUE(result);
}
```

### Integration Test Pattern
```cpp
void test_hardware_new_feature(void) {
    // Arrange: Use real ATEM connection from setUp
    
    // Act
    atem.newFeature();
    
    // Assert: Wait for hardware response
    unsigned long startTime = millis();
    while (!responseReceived && millis() - startTime < 2000) {
        atem.loop();
        delay(10);
    }
    TEST_ASSERT_TRUE(responseReceived);
}
```

## Troubleshooting

### Common Issues

1. **Unit Tests Fail on Native**:
   - Check PlatformIO installation: `pio --version`
   - Verify Unity framework dependency

2. **Integration Tests Timeout**:
   - Verify ATEM IP address and network connectivity
   - Check WiFi credentials in test file
   - Ensure ATEM is powered on and accessible

3. **ESP32 Upload Issues**:
   - Check USB cable and drivers
   - Verify correct board selection in platformio.ini
   - Try different USB port

### Debug Options

Add debug flags to `platformio.ini`:
```ini
build_flags = 
    -DCORE_DEBUG_LEVEL=3
    -DDEBUG_ATEM_PROTOCOL
```
- **ATEM connectivity**
- **Command execution**
- **State synchronization**
- **Transition control**
- **Real-time performance**

### 🚀 **Hardware Tests** (`/extras/test/hardware/`)
- **Multiple ATEM model compatibility**
- **Network stress testing**
- **Extended operation validation**
- **Timing and latency analysis**

## CI/CD Integration

Tests are automatically run on:
- **Pull requests** - Unit tests + basic integration
- **Releases** - Full test suite including hardware validation
- **Nightly builds** - Extended hardware compatibility testing

## Test Configuration

### Mock Testing
- No hardware required
- Fast execution (<30 seconds)
- Validates core logic and protocols

### Hardware Testing
- Requires ATEM device and ESP32
- Network configuration needed
- Extended validation (2-5 minutes)

## Contributing Tests

When adding new features:
1. **Add unit tests** for core logic
2. **Update integration tests** for hardware validation
3. **Document test scenarios** in feature PRs
4. **Verify CI/CD passes** before merging
