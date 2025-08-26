/*
 * ComprehensiveController.ino
 * ATEM ESP32 Library - Comprehensive Example with Serial Interface
 * 
 * This example demonstrates advanced usage of the ATEM ESP32 library with:
 * - WiFi connection management
 * - Real-time ATEM state monitoring with event callbacks
 * - Comprehensive serial command interface for manual control
 * - Preview/Program input switching with validation
 * - CUT and AUTO transition controls
 * - Phase 1 Advanced Switching: Fade to Black, Transition Position, Preview Transition
 * - Detailed status reporting and error handling
 * - Professional debug output and connection management
 * 
 * This example serves multiple purposes:
 * 1. Comprehensive library usage demonstration
 * 2. Ready-to-use ATEM controller with serial interface
 * 3. Test firmware for library regression testing
 * 4. Reference implementation for advanced applications
 * 
 * Hardware Requirements:
 * - ESP32 development board (tested with ESP32-C6)
 * - WiFi network connection
 * - Blackmagic ATEM switcher on same network
 * 
 * Usage:
 * 1. Copy config_sample.h to config.h and update with your settings
 * 2. Upload to ESP32 and open Serial Monitor (115200 baud)
 * 3. Send any character to start, then use commands like:
 *    - preview 2 (set preview to CAM2)
 *    - program bars (set program to Color Bars)
 *    - cut (immediate transition)
 *    - auto (fade/wipe transition)
 *    - ftb (fade to black toggle)
 *    - ftb-rate 25 (set fade rate)
 *    - transition 5000 (set transition position)
 *    - preview-trans on (enable transition preview)
 *    - status (show current state)
 */

// Configure logging level BEFORE including library
#define ATEM_DEFAULT_LOG_LEVEL       ATEM_LOG_DEBUG  // Debug level for packet analysis

#include <ATEM.h>       // ATEM ESP32 Arduino Library
#include <ATEM_Inputs.h> // Input constants and helper functions
#include <WiFi.h>       // ESP32 WiFi library
#include "config.h"     // Copy config_sample.h to config.h and update settings

// Custom ATEM event handler class
class MyATEMHandler : public ATEM {
public:
  void onConnectionStateChanged(ATEMConnectionState state) override {
    Serial.print("[HANDLER] Connection state: ");
    switch (state) {
      case ATEM_DISCONNECTED: Serial.println("DISCONNECTED"); break;
      case ATEM_CONNECTING: Serial.println("CONNECTING"); break;
      case ATEM_CONNECTED: 
        Serial.println("CONNECTED");
        // Print connection info when connected
        printConnectionInfo();
        break;
      case ATEM_ERROR: Serial.println("ERROR"); break;
    }
  }
  
  void onProgramInputChanged(uint16_t input) override {
    Serial.print("[HANDLER] Program input changed to: ");
    Serial.println(input);
  }
  
  void onPreviewInputChanged(uint16_t input) override {
    Serial.print("[HANDLER] Preview input changed to: ");
    Serial.println(input);
  }
  
  void onStateChanged() override {
    Serial.println("[HANDLER] ATEM state changed");
    ATEMState state = getState();
    Serial.print("  Program: ");
    Serial.print(state.program_input);
    Serial.print(", Preview: ");
    Serial.println(state.preview_input);
  }
};

// Single instance of our ATEM handler
MyATEMHandler myAtem;

// UART Command Processing Functions
void processUARTCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove whitespace
    
    if (command.length() == 0) return;
    
    Serial.print("[CMD] Received: ");
    Serial.println(command);
    
    // Parse command
    if (command.equalsIgnoreCase("help") || command.equals("?")) {
      printHelp();
    }
    else if (command.startsWith("preview ") || command.startsWith("pvw ")) {
      handlePreviewCommand(command);
    }
    else if (command.startsWith("program ") || command.startsWith("pgm ")) {
      handleProgramCommand(command);
    }
    else if (command.equalsIgnoreCase("cut")) {
      handleCutCommand();
    }
    else if (command.equalsIgnoreCase("auto")) {
      handleAutoCommand();
    }
    // 🔄 ADVANCED SWITCHING COMMANDS (Phase 1)
    else if (command.equalsIgnoreCase("ftb") || command.equalsIgnoreCase("fadetoBlack")) {
      handleFadeToBlackCommand();
    }
    else if (command.startsWith("ftb-rate ") || command.startsWith("faderate ")) {
      handleFadeToBlackRateCommand(command);
    }
    else if (command.startsWith("transition ") || command.startsWith("trans ")) {
      handleTransitionPositionCommand(command);
    }
    else if (command.startsWith("preview-trans ") || command.startsWith("pvw-trans ")) {
      handlePreviewTransitionCommand(command);
    }
    else if (command.equalsIgnoreCase("status")) {
      printCurrentStatus();
    }
    else {
      Serial.println("[CMD] Unknown command. Type 'help' for available commands.");
    }
  }
}

void printHelp() {
  Serial.println("========================================");
  Serial.println("ATEM ESP32 Controller - UART Commands");
  Serial.println("========================================");
  Serial.println("Available commands:");
  Serial.println("  help          - Show this help");
  Serial.println("  status        - Show current ATEM status");
  Serial.println("");
  Serial.println("🎬 BASIC SWITCHING:");
  Serial.println("  preview <N>   - Set preview input");
  Serial.println("  pvw <N>       - Alias for preview");
  Serial.println("  program <N>   - Set program input");
  Serial.println("  pgm <N>       - Alias for program");
  Serial.println("  cut           - Perform CUT transition (immediate)");
  Serial.println("  auto          - Perform AUTO transition (fade/wipe)");
  Serial.println("");
  Serial.println("🔄 ADVANCED SWITCHING (Phase 1):");
  Serial.println("  ftb           - Fade to black toggle");
  Serial.println("  ftb-rate <N>  - Set fade to black rate (frames)");
  Serial.println("  transition <N> - Set transition position (0-10000)");
  Serial.println("  preview-trans <on|off> - Enable/disable transition preview");
  Serial.println("========================================");
  Serial.println("Input options:");
  Serial.println("  1-4           - Camera inputs (CAM1-CAM4)");
  Serial.println("  black         - Black/Matte");
  Serial.println("  bars          - Color Bars");
  Serial.println("  col1/col2     - Color Generators");
  Serial.println("  mp1           - Media Player 1 (Mini Pro has only 1 MP)");
  Serial.println("========================================");
  Serial.println("Examples:");
  Serial.println("  preview 2     - Set preview to CAM2");
  Serial.println("  program bars  - Set program to Color Bars");
  Serial.println("  cut           - Immediate switch from preview to program");
  Serial.println("  auto          - Fade/wipe transition from preview to program");
  Serial.println("  ftb           - Toggle fade to black");
  Serial.println("  ftb-rate 25   - Set fade rate to 25 frames");
  Serial.println("  transition 5000 - Set transition to 50% position");
  Serial.println("  preview-trans on - Enable transition preview");
  Serial.println("  status        - Show program/preview status");
  Serial.println("========================================");
}

void handlePreviewCommand(String command) {
  // Extract the input number/name
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("[CMD] Error: Missing input. Usage: preview <1-4|black|bars|col1|col2|mp1>");
    return;
  }
  
  String inputStr = command.substring(spaceIndex + 1);
  inputStr.trim();
  inputStr.toLowerCase();
  
  uint16_t inputNum = 0;
  
  // Parse input - numbers or names
  if (inputStr == "1" || inputStr == "cam1") {
    inputNum = ATEM_INPUT_CAM1;
  } else if (inputStr == "2" || inputStr == "cam2") {
    inputNum = ATEM_INPUT_CAM2;
  } else if (inputStr == "3" || inputStr == "cam3") {
    inputNum = ATEM_INPUT_CAM3;
  } else if (inputStr == "4" || inputStr == "cam4") {
    inputNum = ATEM_INPUT_CAM4;
  } else if (inputStr == "0" || inputStr == "black") {
    inputNum = ATEM_INPUT_BLACK;
  } else if (inputStr == "bars") {
    inputNum = ATEM_INPUT_BARS;
  } else if (inputStr == "col1" || inputStr == "color1") {
    inputNum = ATEM_INPUT_COLOR1;
  } else if (inputStr == "col2" || inputStr == "color2") {
    inputNum = ATEM_INPUT_COLOR2;
  } else if (inputStr == "mp1") {
    inputNum = ATEM_INPUT_MP1;
  } else {
    Serial.println("[CMD] Error: Invalid input. Use: 1-4, black, bars, col1, col2, mp1");
    return;
  }
  
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("[CMD] Error: ATEM not connected!");
    return;
  }
  
  Serial.print("[CMD] Setting preview to input ");
  Serial.print(inputNum);
  Serial.print(" (");
  Serial.print(getInputName(inputNum));
  Serial.println(")...");
  
  // Call the ATEM library function
  myAtem.changePreviewInput(inputNum);
  Serial.println("[CMD] ✓ Preview change command sent");
}

void handleProgramCommand(String command) {
  // Extract input number/name from command (works for both "program N" and "pgm N")
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("[CMD] Error: Missing input. Usage: program <1-4|black|bars|col1|col2|mp1>");
    return;
  }
  
  String inputStr = command.substring(spaceIndex + 1);
  inputStr.trim();
  inputStr.toLowerCase();
  
  uint16_t inputNum = 0;
  
  // Parse input - numbers or names
  if (inputStr == "1" || inputStr == "cam1") {
    inputNum = ATEM_INPUT_CAM1;
  } else if (inputStr == "2" || inputStr == "cam2") {
    inputNum = ATEM_INPUT_CAM2;
  } else if (inputStr == "3" || inputStr == "cam3") {
    inputNum = ATEM_INPUT_CAM3;
  } else if (inputStr == "4" || inputStr == "cam4") {
    inputNum = ATEM_INPUT_CAM4;
  } else if (inputStr == "0" || inputStr == "black") {
    inputNum = ATEM_INPUT_BLACK;
  } else if (inputStr == "bars") {
    inputNum = ATEM_INPUT_BARS;
  } else if (inputStr == "col1" || inputStr == "color1") {
    inputNum = ATEM_INPUT_COLOR1;
  } else if (inputStr == "col2" || inputStr == "color2") {
    inputNum = ATEM_INPUT_COLOR2;
  } else if (inputStr == "mp1") {
    inputNum = ATEM_INPUT_MP1;
  } else {
    Serial.println("[CMD] Error: Invalid input. Use: 1-4, black, bars, col1, col2, mp1");
    return;
  }
  
  if (myAtem.getConnectionState() != ATEM_CONNECTED) {
    Serial.println("[CMD] Error: ATEM not connected!");
    return;
  }
  
  // Send command to ATEM
  Serial.print("[CMD] Setting program to input ");
  Serial.print(inputNum);
  Serial.print(" (");
  Serial.print(getInputName(inputNum));
  Serial.println(")...");
  
  myAtem.changeProgramInput(inputNum);
  Serial.println("[CMD] ✓ Program change command sent");
}

void handleCutCommand() {
  Serial.println("[CMD] Performing CUT transition...");
  
  if (!myAtem.isConnected()) {
    Serial.println("[CMD] ❌ Error: ATEM not connected");
    return;
  }
  
  myAtem.cut();
  Serial.println("[CMD] ✓ CUT transition command sent");
  Serial.println("[CMD] Preview instantly switched to Program");
}

void handleAutoCommand() {
  Serial.println("[CMD] Performing AUTO transition...");
  
  if (!myAtem.isConnected()) {
    Serial.println("[CMD] ❌ Error: ATEM not connected");
    return;
  }
  
  myAtem.autoTransition();
  Serial.println("[CMD] ✓ AUTO transition command sent");
  Serial.println("[CMD] Transition effect in progress...");
}

// 🔄 ADVANCED SWITCHING COMMAND HANDLERS (Phase 1)

void handleFadeToBlackCommand() {
  Serial.println("[CMD] Performing Fade to Black toggle...");
  
  if (!myAtem.isConnected()) {
    Serial.println("[CMD] ❌ Error: ATEM not connected");
    return;
  }
  
  myAtem.fadeToBlack(); // Default ME 0
  Serial.println("[CMD] ✓ Fade to Black command sent");
  Serial.println("[CMD] ATEM will toggle fade to black state");
}

void handleFadeToBlackRateCommand(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("[CMD] Error: Missing rate. Usage: ftb-rate <frames>");
    return;
  }
  
  String rateStr = command.substring(spaceIndex + 1);
  rateStr.trim();
  
  int rate = rateStr.toInt();
  if (rate <= 0 || rate > 250) {
    Serial.println("[CMD] Error: Invalid rate. Use 1-250 frames");
    return;
  }
  
  if (!myAtem.isConnected()) {
    Serial.println("[CMD] ❌ Error: ATEM not connected");
    return;
  }
  
  Serial.print("[CMD] Setting fade to black rate to ");
  Serial.print(rate);
  Serial.println(" frames...");
  
  myAtem.setFadeToBlackRate(rate); // Default ME 0
  Serial.println("[CMD] ✓ Fade to black rate command sent");
}

void handleTransitionPositionCommand(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("[CMD] Error: Missing position. Usage: transition <0-10000>");
    return;
  }
  
  String posStr = command.substring(spaceIndex + 1);
  posStr.trim();
  
  int position = posStr.toInt();
  if (position < 0 || position > 10000) {
    Serial.println("[CMD] Error: Invalid position. Use 0-10000 (0=preview, 10000=program)");
    return;
  }
  
  if (!myAtem.isConnected()) {
    Serial.println("[CMD] ❌ Error: ATEM not connected");
    return;
  }
  
  Serial.print("[CMD] Setting transition position to ");
  Serial.print(position);
  Serial.print("/10000 (");
  Serial.print((float)position / 100.0);
  Serial.println("%)...");
  
  myAtem.setTransitionPosition(position); // Default ME 0
  Serial.println("[CMD] ✓ Transition position command sent");
}

void handlePreviewTransitionCommand(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("[CMD] Error: Missing state. Usage: preview-trans <on|off>");
    return;
  }
  
  String stateStr = command.substring(spaceIndex + 1);
  stateStr.trim();
  stateStr.toLowerCase();
  
  bool enable;
  if (stateStr == "on" || stateStr == "1" || stateStr == "true") {
    enable = true;
  } else if (stateStr == "off" || stateStr == "0" || stateStr == "false") {
    enable = false;
  } else {
    Serial.println("[CMD] Error: Invalid state. Use: on, off, 1, 0, true, false");
    return;
  }
  
  if (!myAtem.isConnected()) {
    Serial.println("[CMD] ❌ Error: ATEM not connected");
    return;
  }
  
  Serial.print("[CMD] ");
  Serial.print(enable ? "Enabling" : "Disabling");
  Serial.println(" transition preview...");
  
  myAtem.previewTransition(enable); // Default ME 0
  Serial.print("[CMD] ✓ Transition preview ");
  Serial.print(enable ? "enabled" : "disabled");
  Serial.println(" command sent");
}

void printCurrentStatus() {
  Serial.println("========================================");
  Serial.println("Current ATEM Status");
  Serial.println("========================================");
  Serial.print("✅ Connection: ");
  
  switch (myAtem.getConnectionState()) {
    case ATEM_DISCONNECTED: Serial.println("DISCONNECTED"); break;
    case ATEM_CONNECTING: Serial.println("CONNECTING"); break;
    case ATEM_CONNECTED: {
      Serial.println("CONNECTED ✓");
      uint16_t programInput = myAtem.getProgramInput();
      uint16_t previewInput = myAtem.getPreviewInput();
      
      Serial.print("📺 Program Input: ");
      Serial.print(programInput);
      Serial.print(" (");
      Serial.print(getInputName(programInput));
      Serial.println(")");
      
      Serial.print("📺 Preview Input: ");
      Serial.print(previewInput);
      Serial.print(" (");
      Serial.print(getInputName(previewInput));
      Serial.println(")");
      break;
    }
    case ATEM_ERROR: Serial.println("ERROR"); break;
  }
  Serial.println("========================================");
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000); // Give serial time to initialize
  
  // Initialize built-in LED for debug feedback
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // DEBUG PAUSE: Wait for serial command to continue
  Serial.println("===========================================");
  Serial.println("ATEM ESP32 Controller - DEBUG MODE");
  Serial.println("===========================================");
  Serial.println("*** PAUSED FOR DEBUG ***");
  Serial.println("Send any character via Serial Monitor to continue...");
  Serial.println("This allows you to capture all debug output from the start.");
  Serial.println("===========================================");
  
  // Wait for any input from serial monitor
  while (!Serial.available()) {
    delay(100);
    // Optional: blink built-in LED to show we're waiting
    static bool led_state = false;
    static unsigned long last_blink = 0;
    if (millis() - last_blink > 500) {
      led_state = !led_state;
      digitalWrite(LED_BUILTIN, led_state);
      last_blink = millis();
    }
  }
  
  // Clear the serial input buffer
  while (Serial.available()) {
    Serial.read();
  }
  
  Serial.println("*** CONTINUING PROGRAM ***");
  Serial.println("===========================================");
  
  Serial.println("ATEM ESP32 Controller Starting...");
  Serial.println("Using ATEM ESP32 Arduino Library v" ATEM_ESP32_VERSION);
  Serial.println("================================");
  
  // Initialize WiFi connection
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting");
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 30) {
    delay(500);
    Serial.print(".");
    wifi_attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connection failed!");
    Serial.println("Please check your credentials in config.h");
    return;
  }
  
  Serial.println();
  Serial.print("WiFi connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to ATEM at: ");
  Serial.println(ATEM_IP);
  
  // Create IPAddress object from config string
  IPAddress atemIP;
  atemIP.fromString(ATEM_IP);
  
  // Initialize ATEM connection
  myAtem.enableDebug(true);  // Enable debug for packet analysis
  myAtem.setLogLevel(ATEM_LOG_DEBUG); // Enable debug level logging
  
  if (myAtem.begin(atemIP)) {
    Serial.println("ATEM initialization successful!");
  } else {
    Serial.println("ATEM initialization failed!");
  }
  
  Serial.println("================================");
  Serial.println("ATEM ESP32 Controller Ready!");
  Serial.println("Monitoring handshake, keep-alive, and program state...");
  Serial.println("================================");
  Serial.println();
  Serial.println("🎛️  UART CONTROL READY");
  Serial.println("Type 'help' for available commands");
  Serial.println("Example: 'preview 2' to set preview to CAM2");
  Serial.println("================================");
}

void loop() {
  // Main ATEM processing loop
  myAtem.runLoop();
  
  // Process UART commands for testing
  processUARTCommands();
  
  // Detailed status display every 30 seconds (less frequent)
  static unsigned long last_status = 0;
  if (millis() - last_status > 30000) {
    Serial.println("--- Status Update ---");
    Serial.print("ATEM: ");
    
    switch (myAtem.getConnectionState()) {
      case ATEM_DISCONNECTED: Serial.println("DISCONNECTED"); break;
      case ATEM_CONNECTING: Serial.println("CONNECTING"); break;
      case ATEM_CONNECTED: 
        Serial.print("CONNECTED - Program: ");
        Serial.print(myAtem.getProgramInput());
        Serial.print(", Preview: ");
        Serial.println(myAtem.getPreviewInput());
        break;
      case ATEM_ERROR: Serial.println("ERROR"); break;
    }
    
    last_status = millis();
  }
  
  // Small delay to prevent watchdog issues
  delay(MAIN_LOOP_DELAY);
}
