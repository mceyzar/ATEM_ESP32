/*
 * AutomatedSwitching.ino
 * 
 * Demonstrates automated ATEM control with timed input switching
 * 
 * This example shows how to:
 * - Create automated switching sequences
 * - Use timers for scheduled operations
 * - Monitor ATEM state changes
 * - Implement basic show automation
 * 
 * Hardware: ESP32 + ATEM Mini/Pro
 * Author: Mirza Ceyzar
 */

#include <WiFi.h>
#include <ATEM.h>

// Network configuration
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";
const char* atem_ip = "192.168.1.100";

// Timing configuration
const unsigned long SWITCH_INTERVAL = 10000;  // Switch every 10 seconds
const unsigned long CUT_DELAY = 2000;         // Cut 2 seconds after preview change

// Switching sequence
const uint16_t SEQUENCE[] = {
  ATEM_INPUT_CAM1,
  ATEM_INPUT_CAM2,
  ATEM_INPUT_CAM3,
  ATEM_INPUT_CAM4,
  ATEM_INPUT_BARS,
  ATEM_INPUT_COLOR1
};
const int SEQUENCE_LENGTH = sizeof(SEQUENCE) / sizeof(SEQUENCE[0]);

class AutomatedATEM : public ATEM {
public:
  void onConnectionStateChanged(ATEMConnectionState state) override {
    Serial.print("[ATEM] Connection: ");
    switch (state) {
      case ATEM_DISCONNECTED: Serial.println("DISCONNECTED"); break;
      case ATEM_CONNECTING: Serial.println("CONNECTING"); break;
      case ATEM_CONNECTED: 
        Serial.println("CONNECTED - Starting automation");
        startAutomation();
        break;
      case ATEM_ERROR: Serial.println("ERROR"); break;
    }
  }
  
  void onPreviewInputChanged(uint16_t input) override {
    Serial.print("[ATEM] Preview changed to: ");
    Serial.println(getInputName(input));
    
    // Schedule automatic cut
    cutScheduled = true;
    cutTime = millis() + CUT_DELAY;
  }
  
  void onProgramInputChanged(uint16_t input) override {
    Serial.print("[ATEM] Program changed to: ");
    Serial.println(getInputName(input));
  }
  
  void runAutomation() {
    if (getConnectionState() != ATEM_CONNECTED) return;
    
    unsigned long now = millis();
    
    // Handle scheduled cuts
    if (cutScheduled && now >= cutTime) {
      Serial.println("[AUTO] Performing scheduled CUT");
      cut();
      cutScheduled = false;
    }
    
    // Handle timed preview switching
    if (now - lastSwitch >= SWITCH_INTERVAL) {
      switchToNextInput();
      lastSwitch = now;
    }
  }
  
private:
  int currentSequenceIndex = 0;
  unsigned long lastSwitch = 0;
  bool cutScheduled = false;
  unsigned long cutTime = 0;
  
  void startAutomation() {
    Serial.println("[AUTO] Automation started");
    lastSwitch = millis();
    currentSequenceIndex = 0;
  }
  
  void switchToNextInput() {
    uint16_t nextInput = SEQUENCE[currentSequenceIndex];
    
    Serial.print("[AUTO] Switching preview to: ");
    Serial.println(getInputName(nextInput));
    
    changePreviewInput(nextInput);
    
    // Move to next input in sequence
    currentSequenceIndex = (currentSequenceIndex + 1) % SEQUENCE_LENGTH;
  }
  
  String getInputName(uint16_t input) {
    switch (input) {
      case ATEM_INPUT_BLACK: return "BLACK";
      case ATEM_INPUT_CAM1: return "CAM1";
      case ATEM_INPUT_CAM2: return "CAM2";
      case ATEM_INPUT_CAM3: return "CAM3";
      case ATEM_INPUT_CAM4: return "CAM4";
      case ATEM_INPUT_BARS: return "BARS";
      case ATEM_INPUT_COLOR1: return "COLOR1";
      case ATEM_INPUT_COLOR2: return "COLOR2";
      case ATEM_INPUT_MP1: return "MP1";
      default: return "UNKNOWN";
    }
  }
};

AutomatedATEM myAtem;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=================================");
  Serial.println("ATEM ESP32 Automated Controller");
  Serial.println("=================================");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("WiFi connected! IP: ");
  Serial.println(WiFi.localIP());
  
  // Connect to ATEM
  Serial.print("Connecting to ATEM: ");
  Serial.println(atem_ip);
  myAtem.begin(atem_ip);
  
  Serial.println("=================================");
  Serial.println("Automation Configuration:");
  Serial.print("Switch interval: ");
  Serial.print(SWITCH_INTERVAL / 1000);
  Serial.println(" seconds");
  Serial.print("Cut delay: ");
  Serial.print(CUT_DELAY / 1000);
  Serial.println(" seconds");
  Serial.println("=================================");
}

void loop() {
  // Process ATEM communication
  myAtem.loop();
  
  // Run automation logic
  myAtem.runAutomation();
  
  // Handle manual commands
  processSerialCommands();
  
  delay(10);
}

void processSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.equalsIgnoreCase("status")) {
      printStatus();
    }
    else if (command.equalsIgnoreCase("stop")) {
      Serial.println("[CMD] Automation would stop here (not implemented)");
    }
    else if (command.equalsIgnoreCase("help")) {
      Serial.println("Available commands:");
      Serial.println("  status - Show current status");
      Serial.println("  stop   - Stop automation");
      Serial.println("  help   - Show this help");
    }
  }
}

void printStatus() {
  Serial.println("=================================");
  Serial.println("Current Status");
  Serial.println("=================================");
  
  Serial.print("ATEM Connection: ");
  if (myAtem.getConnectionState() == ATEM_CONNECTED) {
    Serial.println("CONNECTED");
    
    ATEMState state = myAtem.getState();
    Serial.print("Program Input: ");
    Serial.println(state.program_input);
    Serial.print("Preview Input: ");
    Serial.println(state.preview_input);
  } else {
    Serial.println("DISCONNECTED");
  }
  
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");
  
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  Serial.println("=================================");
}
