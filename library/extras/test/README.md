# ATEM ESP32 Library Testing

This directory contains integration tests and regression testing for the ATEM ESP32 library.

## Test Components

### Hardware Integration Tests (`test_hardware_integration.cpp`)
Unity-based tests that run on actual ESP32 hardware with real ATEM devices.

### Regression Tests (`regression_test.js`)
Comprehensive Node.js-based testing that validates the complete ATEM protocol implementation. 

**Test Firmware**: Uses the [ComprehensiveController example](../../examples/ComprehensiveController/) as test firmware, which provides a serial command interface for automated testing.

## Running Tests

### Prerequisites
- ESP32 device with ComprehensiveController example uploaded
- Physical ATEM switcher connected to network  
- Node.js with serialport dependencies

### Quick Test
```bash
./run_tests.sh regression
```

### Via Library Test Suite
```bash
cd ../test
node library_test_suite.js --hardware  # Includes regression tests
```

### Direct Execution
```bash
AUTOMATED=true node regression_test.js
```

## Test Coverage

The regression tests validate:
- ✅ WiFi and ATEM connectivity
- ✅ Preview input switching (all supported inputs)
- ✅ Program input switching (all supported inputs)  
- ✅ CUT transition execution and verification
- ✅ AUTO transition execution and verification
- ✅ State tracking and synchronization
- ✅ Error handling and recovery
- ✅ Rapid command execution
- ✅ Connection stability over time

## Integration with Build Process

The regression tests are automatically executed by the main project's build script (`../../build_upload_monitor.sh`) after compilation and upload, ensuring every build is validated against real hardware.
