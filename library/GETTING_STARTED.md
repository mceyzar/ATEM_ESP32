# Getting Started with ATEM ESP32 Library

## Hardware Setup

### Required Components
- ESP32 development board (any variant: ESP32C6, ESP32-S3, ESP32-WROOM, etc.)
- ATEM video switcher (Mini, Mini Pro, or any ATEM model)
- WiFi network accessible by both devices
- USB cable for programming ESP32

### Network Configuration

1. **Find your ATEM's IP address**:
   - Open ATEM Software Control on your computer
   - Go to **Settings** → **Network**
   - Note the IP address (e.g., `192.168.1.100`)

2. **Ensure network connectivity**:
   - Both ESP32 and ATEM must be on the same network
   - Test connectivity by pinging ATEM from your computer
   - Disable any firewalls that might block UDP port 9910

## Software Installation

### Arduino IDE Setup

1. **Install ESP32 Board Support**:
   - Open Arduino IDE
   - Go to **File** → **Preferences**
   - Add this URL to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools** → **Board** → **Boards Manager**
   - Search for "ESP32" and install the latest version

2. **Install ATEM ESP32 Library**:
   - Go to **Tools** → **Manage Libraries**
   - Search for "ATEM ESP32"
   - Click **Install**

### Board Selection

Choose your ESP32 board in **Tools** → **Board**:
- **ESP32C6**: "XIAO ESP32C6" or "ESP32C6 Dev Module"
- **ESP32-S3**: "ESP32S3 Dev Module"
- **ESP32**: "ESP32 Dev Module"

## First Project

### 1. Create New Sketch

```cpp
#include <WiFi.h>
#include <ATEM.h>

// WiFi credentials
const char* ssid = "Your_WiFi_Name";
const char* password = "Your_WiFi_Password";

// ATEM IP address
const char* atem_ip = "192.168.1.100";

// Create ATEM controller
ATEM myAtem;

void setup() {
  Serial.begin(115200);
  Serial.println("ATEM ESP32 Controller Starting...");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
  
  // Connect to ATEM
  myAtem.begin(atem_ip);
  Serial.println("Connecting to ATEM...");
}

void loop() {
  myAtem.loop();
  
  // Check if connected
  if (myAtem.getConnectionState() == ATEM_CONNECTED) {
    // Switch to Camera 2 every 5 seconds
    static unsigned long lastSwitch = 0;
    if (millis() - lastSwitch > 5000) {
      Serial.println("Switching to Camera 2");
      myAtem.changePreviewInput(ATEM_INPUT_CAM2);
      lastSwitch = millis();
    }
  }
}
```

### 2. Upload and Test

1. **Connect ESP32** via USB
2. **Select correct port** in **Tools** → **Port**
3. **Upload sketch** (Ctrl+U)
4. **Open Serial Monitor** (Ctrl+Shift+M) at 115200 baud
5. **Watch for connection messages**

### 3. Expected Output

```
ATEM ESP32 Controller Starting...
WiFi connected!
Connecting to ATEM...
[ATEM] Connection state: CONNECTING
[ATEM] Connection state: CONNECTED
Switching to Camera 2
```

## Basic Commands

Once connected, you can control your ATEM:

```cpp
// Input switching
myAtem.changePreviewInput(ATEM_INPUT_CAM1);  // Preview to Camera 1
myAtem.changeProgramInput(ATEM_INPUT_CAM2);  // Program to Camera 2

// Transitions
myAtem.cut();             // Immediate cut
myAtem.autoTransition();  // Fade/wipe transition

// Check current state
ATEMState state = myAtem.getState();
Serial.print("Program: ");
Serial.println(state.program_input);
```

## Available Inputs

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
```

## Troubleshooting

### Connection Issues

**Problem**: "Connection failed"
- **Solution**: Check WiFi and ATEM IP address
- **Test**: Ping ATEM IP from computer

**Problem**: "WiFi won't connect"
- **Solution**: Verify SSID and password
- **Test**: Try connecting with phone/computer

**Problem**: "ATEM keeps disconnecting"
- **Solution**: Check network stability and interference
- **Test**: Move closer to WiFi router

### Debug Mode

Enable detailed logging:

```cpp
#define ATEM_DEFAULT_LOG_LEVEL ATEM_LOG_DEBUG
#include <ATEM.h>
```

This shows all packets and connection details.

## Next Steps

1. **Try the examples**: Load **BasicATEMControl** for full serial control
2. **Add event handling**: Override ATEM methods to respond to changes
3. **Build custom controllers**: Create your own switching logic
4. **Explore advanced features**: AUX outputs, keys, audio control

## Need Help?

- **Examples**: Check the library examples folder
- **Documentation**: Read the full README.md
- **Issues**: Report problems on GitHub
- **Community**: Join discussions for tips and projects
