/*
 * BasicATEMControl.ino
 * 
 * Basic example for the ATEM ESP32 library
 * 
 * This example demonstrates:
 * - Connecting to an ATEM switcher over WiFi
 * - Monitoring connection state and input changes
 * - Controlling preview/program inputs via serial commands
 * - Performing CUT and AUTO transitions
 * 
 * Required Hardware:
 * - ESP32 development board (ESP32C6, ESP32-S3, etc.)
 * - ATEM Mini, ATEM Mini Pro, or compatible ATEM switcher
 * - Both devices on the same network
 * 
 * Setup:
 * 1. Copy config_sample.h to config.h
 * 2. Edit config.h with your WiFi and ATEM IP settings
 * 3. Upload this sketch to your ESP32
 * 4. Open Serial Monitor at 115200 baud
 * 5. Type 'help' to see available commands
 * 
 * Author: Mirza Ceyzar
 * Based on Sofie ATEM Connection protocol implementation
 */

#include <WiFi.h>
#include <ATEM.h>

// WiFi Configuration - Replace with your actual values
const char* ssid = "Your_WiFi_SSID";        // Replace with your WiFi network name
const char* password = "Your_WiFi_Password"; // Replace with your WiFi password
const char* atem_ip = "192.168.1.100";      // Replace with your ATEM's IP address

// Custom ATEM event handler class
class MyATEMHandler : public ATEM {
public:
  void onConnectionStateChanged(ATEMConnectionState state) override {
    Serial.print("[ATEM] Connection state: ");
    switch (state) {
      case ATEM_DISCONNECTED: 
        Serial.println("DISCONNECTED"); 
        break;
      case ATEM_CONNECTING: 
        Serial.println("CONNECTING"); 
        break;
      case ATEM_CONNECTED: 
        Serial.println("CONNECTED");
        printConnectionInfo();
        break;
      case ATEM_ERROR: 
        Serial.println("ERROR"); 
        break;
    }
  }
  
  void onProgramInputChanged(uint16_t input) override {
    Serial.print("[ATEM] Program input changed to: ");
    Serial.println(input);
  }
  
  void onPreviewInputChanged(uint16_t input) override {
    Serial.print("[ATEM] Preview input changed to: ");
    Serial.println(input);
  }
  
  void onStateChanged() override {
    Serial.println("[ATEM] State changed");
    ATEMState state = getState();
    Serial.print("  Program: ");
    Serial.print(state.program_input);
    Serial.print(", Preview: ");
    Serial.println(state.preview_input);
  }
  
private:
  void printConnectionInfo() {
    Serial.println("=== ATEM Connection Info ===");
    Serial.print("State: CONNECTED to ");
    Serial.println(atem_ip);
    ATEMState state = getState();
    Serial.print("Program Input: ");
    Serial.println(state.program_input);
    Serial.print("Preview Input: ");
    Serial.println(state.preview_input);
    Serial.println("==========================");
  }
};

// Create ATEM controller instance
MyATEMHandler myAtem;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ATEM ESP32 Controller Starting...");
  Serial.println("================================");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("WiFi connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize ATEM connection
  Serial.print("Connecting to ATEM at: ");
  Serial.println(atem_ip);
  myAtem.begin(atem_ip);
  
  Serial.println("================================");
  Serial.println("ATEM ESP32 Controller Ready!");
  Serial.println("Type 'help' for available commands");
  Serial.println("================================");
}

void loop() {
  // Process ATEM communication
  myAtem.loop();
  
  // Process serial commands
  processSerialCommands();
  
  // Small delay to prevent overwhelming the system
  delay(10);
}

void processSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.length() == 0) return;
    
    Serial.print("[CMD] ");
    Serial.println(command);
    
    // Parse and execute commands
    if (command.equalsIgnoreCase("help")) {
      printHelp();
    }
    else if (command.equalsIgnoreCase("status")) {
      printStatus();
    }
    else if (command.startsWith("preview ") || command.startsWith("pvw ")) {
      handlePreviewCommand(command);
    }
    else if (command.startsWith("program ") || command.startsWith("pgm ")) {
      handleProgramCommand(command);
    }
    else if (command.equalsIgnoreCase("cut")) {
      handleCutTransition();
    }
    else if (command.equalsIgnoreCase("auto")) {
      handleAutoTransition();
    }
    else {
      Serial.println("Unknown command. Type 'help' for available commands.");
    }
  }
}

void printHelp() {
  Serial.println("========================================");
  Serial.println("ATEM ESP32 Controller - Commands");
  Serial.println("========================================");
  Serial.println("help          - Show this help");
  Serial.println("status        - Show current ATEM status");
  Serial.println("preview <N>   - Set preview input (pvw <N>)");
  Serial.println("program <N>   - Set program input (pgm <N>)");
  Serial.println("cut           - Perform CUT transition");
  Serial.println("auto          - Perform AUTO transition");
  Serial.println("========================================");
  Serial.println("Input options:");
  Serial.println("1-4           - Camera inputs");
  Serial.println("black         - Black/Matte");
  Serial.println("bars          - Color Bars");
  Serial.println("col1, col2    - Color Generators");
  Serial.println("mp1           - Media Player 1");
  Serial.println("========================================");
  Serial.println("Examples:");
  Serial.println("preview 2     - Set preview to CAM2");
  Serial.println("program bars  - Set program to Color Bars");
  Serial.println("cut           - Immediate transition");
  Serial.println("auto          - Fade/wipe transition");
  Serial.println("========================================");
}

void printStatus() {
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("❌ ATEM: DISCONNECTED");
    return;
  }
  
  ATEMState state = myAtem.getState();
  Serial.println("========================================");
  Serial.println("Current ATEM Status");
  Serial.println("========================================");
  Serial.println("✅ Connection: CONNECTED");
  Serial.print("📺 Program Input: ");
  Serial.print(state.program_input);
  Serial.print(" (");
  Serial.print(getInputName(state.program_input));
  Serial.println(")");
  Serial.print("👁  Preview Input: ");
  Serial.print(state.preview_input);
  Serial.print(" (");
  Serial.print(getInputName(state.preview_input));
  Serial.println(")");
  Serial.println("========================================");
}

void handlePreviewCommand(String command) {
  uint16_t input = parseInputFromCommand(command);
  if (input == 0xFFFF) return; // Parse error already reported
  
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("❌ ATEM not connected!");
    return;
  }
  
  Serial.print("Setting preview to: ");
  Serial.println(getInputName(input));
  myAtem.changePreviewInput(input);
  Serial.println("✅ Preview command sent");
}

void handleProgramCommand(String command) {
  uint16_t input = parseInputFromCommand(command);
  if (input == 0xFFFF) return; // Parse error already reported
  
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("❌ ATEM not connected!");
    return;
  }
  
  Serial.print("Setting program to: ");
  Serial.println(getInputName(input));
  myAtem.changeProgramInput(input);
  Serial.println("✅ Program command sent");
}

void handleCutTransition() {
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("❌ ATEM not connected!");
    return;
  }
  
  Serial.println("Performing CUT transition...");
  myAtem.cut();
  Serial.println("✅ CUT transition sent");
}

void handleAutoTransition() {
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("❌ ATEM not connected!");
    return;
  }
  
  Serial.println("Performing AUTO transition...");
  myAtem.autoTransition();
  Serial.println("✅ AUTO transition sent");
}

uint16_t parseInputFromCommand(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("❌ Missing input parameter");
    return 0xFFFF;
  }
  
  String inputStr = command.substring(spaceIndex + 1);
  inputStr.trim();
  inputStr.toLowerCase();
  
  // Parse input names to ATEM input numbers
  if (inputStr == "1" || inputStr == "cam1") return ATEM_INPUT_CAM1;
  if (inputStr == "2" || inputStr == "cam2") return ATEM_INPUT_CAM2;
  if (inputStr == "3" || inputStr == "cam3") return ATEM_INPUT_CAM3;
  if (inputStr == "4" || inputStr == "cam4") return ATEM_INPUT_CAM4;
  if (inputStr == "black" || inputStr == "0") return ATEM_INPUT_BLACK;
  if (inputStr == "bars") return ATEM_INPUT_BARS;
  if (inputStr == "col1") return ATEM_INPUT_COLOR1;
  if (inputStr == "col2") return ATEM_INPUT_COLOR2;
  if (inputStr == "mp1") return ATEM_INPUT_MP1;
  
  Serial.println("❌ Invalid input. Use: 1-4, black, bars, col1, col2, mp1");
  return 0xFFFF;
}

String getInputName(uint16_t input) {
  switch (input) {
    case ATEM_INPUT_BLACK: return "BLACK";
    case ATEM_INPUT_CAM1: return "CAM1";
    case ATEM_INPUT_CAM2: return "CAM2";
    case ATEM_INPUT_CAM3: return "CAM3";
    case ATEM_INPUT_CAM4: return "CAM4";
    case ATEM_INPUT_BARS: return "BARS";
    case ATEM_INPUT_COLOR1: return "COL1";
    case ATEM_INPUT_COLOR2: return "COL2";
    case ATEM_INPUT_MP1: return "MP1";
    default: return "UNKNOWN";
  }
}
