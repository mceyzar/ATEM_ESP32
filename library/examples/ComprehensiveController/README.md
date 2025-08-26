# Comprehensive Controller Example

This example demonstrates advanced usage of the ATEM ESP32 library with a full-featured serial command interface.

## Features

- **WiFi Connection Management** - Automatic connection with timeout handling
- **Event-Driven Monitoring** - Real-time callbacks for all ATEM state changes
- **Serial Command Interface** - Complete control via Serial Monitor
- **Input Switching** - Preview/Program control with validation
- **Transition Control** - CUT (immediate) and AUTO (fade/wipe) transitions
- **Status Reporting** - Detailed connection and state information
- **Error Handling** - Comprehensive error checking and user feedback

## Hardware Requirements

- ESP32 development board (tested with ESP32-C6)
- WiFi network connection
- Blackmagic ATEM switcher on same network

## Setup

1. **Configure Network Settings**:
   ```bash
   cp config_sample.h config.h
   # Edit config.h with your WiFi and ATEM settings
   ```

2. **Update config.h**:
   ```cpp
   #define WIFI_SSID "Your_WiFi_Network"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   #define ATEM_IP "192.168.1.240"  // Your ATEM IP address
   ```

3. **Upload and Monitor**:
   - Upload to ESP32
   - Open Serial Monitor (115200 baud)
   - Send any character to start

## Serial Commands

### Basic Commands
- `help` - Show all available commands
- `status` - Display current ATEM connection and input status

### Input Control
- `preview <input>` or `pvw <input>` - Set preview input
- `program <input>` or `pgm <input>` - Set program input

### Transitions
- `cut` - Immediate CUT transition (preview → program)
- `auto` - AUTO transition with current effect (fade/wipe)

### Input Options
- `1-4` - Camera inputs (CAM1-CAM4)
- `black` - Black/Matte
- `bars` - Color Bars
- `col1/col2` - Color Generators
- `mp1` - Media Player 1

## Example Usage

```
preview 2       → Set preview to CAM2
program bars    → Set program to Color Bars
cut            → Immediate switch
auto           → Fade transition
status         → Show current state
```

## Code Structure

### Custom Event Handler
```cpp
class MyATEMHandler : public ATEM {
public:
  void onConnectionStateChanged(ATEMConnectionState state) override;
  void onProgramInputChanged(uint16_t input) override;
  void onPreviewInputChanged(uint16_t input) override;
  void onStateChanged() override;
};
```

### Command Processing
- `processUARTCommands()` - Main command parser
- `handlePreviewCommand()` - Preview input switching
- `handleProgramCommand()` - Program input switching  
- `handleCutCommand()` - CUT transition execution
- `handleAutoCommand()` - AUTO transition execution

## Testing Integration

This example serves as the test firmware for library regression testing:
- Comprehensive command interface for automated testing
- Event callbacks for state validation
- Status reporting for test verification
- Professional error handling and feedback

## Troubleshooting

### Connection Issues
1. Verify WiFi credentials in config.h
2. Check ATEM IP address (use ATEM Software Control to confirm)
3. Ensure ESP32 and ATEM are on same network
4. Check firewall settings (UDP port 9910)

### Serial Monitor Tips
- Use 115200 baud rate
- Send any character to start after upload
- Type commands and press Enter
- Use `help` command to see all options

### Debug Output
The example includes configurable debug logging:
```cpp
#define ATEM_DEFAULT_LOG_LEVEL ATEM_LOG_DEBUG
```

Available levels: `ATEM_LOG_NONE`, `ATEM_LOG_ERROR`, `ATEM_LOG_WARN`, `ATEM_LOG_INFO`, `ATEM_LOG_DEBUG`, `ATEM_LOG_VERBOSE`
