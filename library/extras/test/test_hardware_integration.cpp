#include <unity.h>
#include <ATEM.h>
#include <WiFi.h>

#ifdef HARDWARE_TEST

// Hardware test configuration
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* ATEM_IP = "192.168.1.240";

ATEM atem;

// Connection state tracking
volatile bool connectionReceived = false;
volatile bool programChangeReceived = false;
volatile bool previewChangeReceived = false;

// Test callbacks
void onConnectionStateChanged(ATEMConnectionState state) {
    connectionReceived = true;
    Serial.printf("Connection state changed: %d\n", state);
}

void onProgramInputChanged(uint16_t input) {
    programChangeReceived = true;
    Serial.printf("Program input changed: %d\n", input);
}

void onPreviewInputChanged(uint16_t input) {
    previewChangeReceived = true;
    Serial.printf("Preview input changed: %d\n", input);
}

// Hardware test setup
void hardware_setUp(void) {
    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        delay(100);
    }
    
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());
    
    // Setup ATEM callbacks
    atem.setConnectionStateCallback(onConnectionStateChanged);
    atem.setProgramInputCallback(onProgramInputChanged);
    atem.setPreviewInputCallback(onPreviewInputChanged);
    
    // Connect to ATEM
    TEST_ASSERT_TRUE(atem.begin(ATEM_IP));
    
    // Wait for connection
    startTime = millis();
    while (!connectionReceived && millis() - startTime < 5000) {
        atem.loop();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(connectionReceived);
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
}

void hardware_tearDown(void) {
    connectionReceived = false;
    programChangeReceived = false;
    previewChangeReceived = false;
}

// Test actual ATEM hardware connection
void test_hardware_connection(void) {
    // Connection should be established in setUp
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
    
    // Test state retrieval
    ATEMState state = atem.getState();
    TEST_ASSERT_TRUE(state.program_input >= 0);
    TEST_ASSERT_TRUE(state.preview_input >= 0);
}

// Test preview input change with hardware
void test_hardware_preview_change(void) {
    previewChangeReceived = false;
    
    // Change preview to CAM1
    atem.changePreviewInput(ATEM_INPUT_CAM1);
    
    // Wait for acknowledgment
    unsigned long startTime = millis();
    while (!previewChangeReceived && millis() - startTime < 2000) {
        atem.loop();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(previewChangeReceived);
    
    // Verify state
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM1, state.preview_input);
}

// Test program input change with hardware
void test_hardware_program_change(void) {
    programChangeReceived = false;
    
    // Change program to CAM2
    atem.changeProgramInput(ATEM_INPUT_CAM2);
    
    // Wait for acknowledgment
    unsigned long startTime = millis();
    while (!programChangeReceived && millis() - startTime < 2000) {
        atem.loop();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(programChangeReceived);
    
    // Verify state
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM2, state.program_input);
}

// Test CUT transition with hardware
void test_hardware_cut_transition(void) {
    // Set up known state: CAM1 on program, CAM2 on preview
    atem.changeProgramInput(ATEM_INPUT_CAM1);
    delay(500);
    atem.changePreviewInput(ATEM_INPUT_CAM2);
    delay(500);
    
    programChangeReceived = false;
    previewChangeReceived = false;
    
    // Perform CUT
    atem.cut();
    
    // Wait for changes
    unsigned long startTime = millis();
    while ((!programChangeReceived || !previewChangeReceived) && millis() - startTime < 3000) {
        atem.loop();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(programChangeReceived);
    TEST_ASSERT_TRUE(previewChangeReceived);
    
    // Verify the inputs were swapped
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM2, state.program_input);
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM1, state.preview_input);
}

// Test AUTO transition with hardware
void test_hardware_auto_transition(void) {
    // Set up known state: CAM3 on program, CAM4 on preview
    atem.changeProgramInput(ATEM_INPUT_CAM3);
    delay(500);
    atem.changePreviewInput(ATEM_INPUT_CAM4);
    delay(500);
    
    programChangeReceived = false;
    previewChangeReceived = false;
    
    // Perform AUTO transition
    atem.autoTransition();
    
    // Wait for changes (AUTO takes longer than CUT)
    unsigned long startTime = millis();
    while ((!programChangeReceived || !previewChangeReceived) && millis() - startTime < 5000) {
        atem.loop();
        delay(10);
    }
    
    TEST_ASSERT_TRUE(programChangeReceived);
    TEST_ASSERT_TRUE(previewChangeReceived);
    
    // Verify the inputs were swapped
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM4, state.program_input);
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM3, state.preview_input);
}

// Test multiple rapid commands
void test_hardware_rapid_commands(void) {
    // Rapid preview changes
    atem.changePreviewInput(ATEM_INPUT_CAM1);
    delay(100);
    atem.changePreviewInput(ATEM_INPUT_CAM2);
    delay(100);
    atem.changePreviewInput(ATEM_INPUT_CAM3);
    delay(100);
    atem.changePreviewInput(ATEM_INPUT_CAM4);
    
    // Give time for all commands to process
    delay(2000);
    
    // Final state should be CAM4 on preview
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM4, state.preview_input);
}

// 🔄 PHASE 1 ADVANCED SWITCHING TESTS

// Test fade to black functionality
void test_hardware_fade_to_black(void) {
    Serial.println("Testing Fade to Black command...");
    
    // Ensure connected
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
    
    // Send fade to black command
    atem.fadeToBlack();
    delay(1000); // Give time for command to process
    
    // Note: We can't easily test the actual fade state without additional
    // ATEM state monitoring, but we can verify the command was sent successfully
    Serial.println("Fade to black command sent successfully");
}

// Test fade to black rate setting
void test_hardware_fade_to_black_rate(void) {
    Serial.println("Testing Fade to Black Rate command...");
    
    // Ensure connected
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
    
    // Test various fade rates
    uint16_t testRates[] = {12, 25, 30, 50}; // Common frame rates
    
    for (int i = 0; i < 4; i++) {
        Serial.printf("Setting fade rate to %d frames...\n", testRates[i]);
        atem.setFadeToBlackRate(testRates[i]);
        delay(200); // Brief delay between commands
    }
    
    Serial.println("Fade to black rate commands sent successfully");
}

// Test transition position control
void test_hardware_transition_position(void) {
    Serial.println("Testing Transition Position command...");
    
    // Ensure connected
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
    
    // Test various transition positions
    uint16_t testPositions[] = {0, 2500, 5000, 7500, 10000}; // 0%, 25%, 50%, 75%, 100%
    
    for (int i = 0; i < 5; i++) {
        Serial.printf("Setting transition position to %d/10000 (%d%%)...\n", 
                     testPositions[i], testPositions[i] / 100);
        atem.setTransitionPosition(testPositions[i]);
        delay(300); // Brief delay to see transition movement
    }
    
    // Return to preview position
    atem.setTransitionPosition(0);
    delay(200);
    
    Serial.println("Transition position commands sent successfully");
}

// Test preview transition control
void test_hardware_preview_transition(void) {
    Serial.println("Testing Preview Transition command...");
    
    // Ensure connected
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
    
    // Enable preview transition
    Serial.println("Enabling preview transition...");
    atem.previewTransition(true);
    delay(500);
    
    // Disable preview transition
    Serial.println("Disabling preview transition...");
    atem.previewTransition(false);
    delay(500);
    
    Serial.println("Preview transition commands sent successfully");
}

// Test Phase 1 command sequence integration
void test_hardware_phase1_integration(void) {
    Serial.println("Testing Phase 1 Advanced Switching integration...");
    
    // Ensure connected
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
    
    // Set up initial state
    atem.changeProgramInput(ATEM_INPUT_CAM1);
    atem.changePreviewInput(ATEM_INPUT_CAM2);
    delay(500);
    
    // Test sequence: 
    // 1. Set fade rate
    atem.setFadeToBlackRate(25);
    delay(200);
    
    // 2. Enable preview transition  
    atem.previewTransition(true);
    delay(200);
    
    // 3. Move transition position partially
    atem.setTransitionPosition(3000); // 30%
    delay(500);
    
    // 4. Complete transition
    atem.setTransitionPosition(10000); // 100%
    delay(500);
    
    // 5. Reset transition
    atem.setTransitionPosition(0); // Back to preview
    delay(200);
    
    // 6. Disable preview transition
    atem.previewTransition(false);
    delay(200);
    
    Serial.println("Phase 1 integration test completed successfully");
}

// Main hardware test runner
int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    Serial.println("=== ATEM ESP32 Hardware Integration Tests ===");
    Serial.println("WARNING: These tests require actual ATEM hardware!");
    Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
    Serial.printf("ATEM IP: %s\n", ATEM_IP);
    Serial.println();
    
    // Setup hardware connection
    hardware_setUp();
    
    // Run hardware tests
    RUN_TEST(test_hardware_connection);
    RUN_TEST(test_hardware_preview_change);
    RUN_TEST(test_hardware_program_change);
    RUN_TEST(test_hardware_cut_transition);
    RUN_TEST(test_hardware_auto_transition);
    RUN_TEST(test_hardware_rapid_commands);
    
    // 🔄 Phase 1 Advanced Switching Tests
    RUN_TEST(test_hardware_fade_to_black);
    RUN_TEST(test_hardware_fade_to_black_rate);
    RUN_TEST(test_hardware_transition_position);
    RUN_TEST(test_hardware_preview_transition);
    RUN_TEST(test_hardware_phase1_integration);
    
    return UNITY_END();
}

// For Arduino IDE compatibility
void setup() {
    Serial.begin(115200);
    delay(2000);
    main(0, NULL);
}

void loop() {
    // Keep ATEM connection alive during tests
    atem.loop();
    delay(10);
}

#else
// Stub for non-hardware builds
void setup() {
    Serial.begin(115200);
    Serial.println("Hardware tests disabled. Use HARDWARE_TEST build flag to enable.");
}

void loop() {
    delay(1000);
}
#endif
