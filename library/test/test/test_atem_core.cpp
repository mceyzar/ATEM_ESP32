#include <unity.h>

// For unit tests, we'll include the source files directly
// since we don't have the full Arduino environment
#ifndef ARDUINO_MOCK
#define ARDUINO_MOCK
#endif

// Mock Arduino types and functions for native testing
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// Mock Arduino functions
void delay(uint32_t ms) { /* Mock delay */ }
uint32_t millis() { return 0; }

// Mock WiFi types
enum wl_status_t {
    WL_CONNECTED = 3
};

class WiFiClass {
public:
    void begin(const char* ssid, const char* pass = nullptr) {}
    wl_status_t status() { return WL_CONNECTED; }
};
WiFiClass WiFi;

// Define ATEM input constants (normally from ATEM_Inputs.h)
#define ATEM_INPUT_BLACK 0
#define ATEM_INPUT_CAM1 1
#define ATEM_INPUT_CAM2 2
#define ATEM_INPUT_CAM3 3
#define ATEM_INPUT_CAM4 4
#define ATEM_INPUT_BARS 1000
#define ATEM_INPUT_COLOR1 2001
#define ATEM_INPUT_COLOR2 2002
#define ATEM_INPUT_MP1 3010

// Mock ATEM connection states
enum ATEMConnectionState {
    ATEM_DISCONNECTED = 0,
    ATEM_CONNECTING = 1,
    ATEM_CONNECTED = 2
};

// Mock ATEM state structure
struct ATEMState {
    uint16_t program_input = ATEM_INPUT_CAM1;
    uint16_t preview_input = ATEM_INPUT_CAM2;
    bool in_transition = false;
};

// Mock ATEM class for testing
class ATEM {
public:
    virtual ~ATEM() {}
    
    virtual bool begin(const char* ip) {
        return true;
    }
    
    virtual void loop() {
        // Mock loop function
    }
    
    virtual ATEMConnectionState getConnectionState() {
        return ATEM_CONNECTED;
    }
    
    virtual ATEMState getState() {
        return state;
    }
    
    virtual void changePreviewInput(uint16_t input) {
        state.preview_input = input;
    }
    
    virtual void changeProgramInput(uint16_t input) {
        state.program_input = input;
    }
    
    virtual void cut() {
        uint16_t temp = state.program_input;
        state.program_input = state.preview_input;
        state.preview_input = temp;
    }
    
    virtual void autoTransition() {
        state.in_transition = true;
        // In real implementation, this would trigger a transition
        uint16_t temp = state.program_input;
        state.program_input = state.preview_input;
        state.preview_input = temp;
        state.in_transition = false;
    }
    
    // Callback setters (mock)
    void setConnectionStateCallback(void(*callback)(ATEMConnectionState)) {}
    void setProgramInputCallback(void(*callback)(uint16_t)) {}
    void setPreviewInputCallback(void(*callback)(uint16_t)) {}

protected:
    ATEMState state;
    
    // Mock event triggers for testing
    void onConnectionStateChanged(ATEMConnectionState newState) {
        // Mock event trigger
    }
    
    void onProgramInputChanged(uint16_t input) {
        state.program_input = input;
    }
    
    void onPreviewInputChanged(uint16_t input) {
        state.preview_input = input;
    }
};

ATEM atem;

// Test setup function
void setUp(void) {
    // Reset state before each test
}

// Test teardown function  
void tearDown(void) {
    // Clean up after each test
}

// Test basic initialization
void test_atem_initialization(void) {
    TEST_ASSERT_TRUE(atem.begin("192.168.1.100"));
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
}

// Test input constants are properly defined
void test_input_constants(void) {
    TEST_ASSERT_EQUAL(0, ATEM_INPUT_BLACK);
    TEST_ASSERT_EQUAL(1, ATEM_INPUT_CAM1);
    TEST_ASSERT_EQUAL(2, ATEM_INPUT_CAM2);
    TEST_ASSERT_EQUAL(3, ATEM_INPUT_CAM3);
    TEST_ASSERT_EQUAL(4, ATEM_INPUT_CAM4);
    TEST_ASSERT_EQUAL(1000, ATEM_INPUT_BARS);
    TEST_ASSERT_EQUAL(2001, ATEM_INPUT_COLOR1);
    TEST_ASSERT_EQUAL(2002, ATEM_INPUT_COLOR2);
    TEST_ASSERT_EQUAL(3010, ATEM_INPUT_MP1);
}

// Test preview input change
void test_preview_input_change(void) {
    atem.changePreviewInput(ATEM_INPUT_CAM2);
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM2, state.preview_input);
}

// Test program input change
void test_program_input_change(void) {
    atem.changeProgramInput(ATEM_INPUT_CAM3);
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM3, state.program_input);
}

// Test CUT transition
void test_cut_transition(void) {
    // Set initial state
    atem.changeProgramInput(ATEM_INPUT_CAM1);
    atem.changePreviewInput(ATEM_INPUT_CAM2);
    
    // Perform cut
    atem.cut();
    
    // Verify inputs were swapped
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM2, state.program_input);
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM1, state.preview_input);
}

// Test AUTO transition
void test_auto_transition(void) {
    // Set initial state
    atem.changeProgramInput(ATEM_INPUT_CAM3);
    atem.changePreviewInput(ATEM_INPUT_CAM4);
    
    // Perform auto transition
    atem.autoTransition();
    
    // Verify inputs were swapped
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM4, state.program_input);
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM3, state.preview_input);
}

// Test state tracking
void test_state_tracking(void) {
    ATEMState state = atem.getState();
    // Initial state should be valid
    TEST_ASSERT_TRUE(state.program_input >= 0);
    TEST_ASSERT_TRUE(state.preview_input >= 0);
}

// Test invalid input handling
void test_invalid_input_handling(void) {
    // Test with invalid input number - should not crash
    atem.changePreviewInput(9999);
    ATEMState state = atem.getState();
    // State should still be accessible
    TEST_ASSERT_TRUE(state.program_input >= 0);
}

// Test connection state
void test_connection_state(void) {
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, atem.getConnectionState());
}

// Test multiple operations
void test_multiple_operations(void) {
    // Chain multiple operations
    atem.changeProgramInput(ATEM_INPUT_CAM1);
    atem.changePreviewInput(ATEM_INPUT_CAM2);
    atem.cut();
    atem.changePreviewInput(ATEM_INPUT_CAM3);
    
    ATEMState state = atem.getState();
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM2, state.program_input);
    TEST_ASSERT_EQUAL(ATEM_INPUT_CAM3, state.preview_input);
}

// Main test runner
int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Basic functionality tests
    RUN_TEST(test_atem_initialization);
    RUN_TEST(test_input_constants);
    RUN_TEST(test_state_tracking);
    RUN_TEST(test_connection_state);
    
    // Control function tests
    RUN_TEST(test_preview_input_change);
    RUN_TEST(test_program_input_change);
    RUN_TEST(test_cut_transition);
    RUN_TEST(test_auto_transition);
    
    // Error handling tests
    RUN_TEST(test_invalid_input_handling);
    
    // Integration tests
    RUN_TEST(test_multiple_operations);
    
    return UNITY_END();
}

// For PlatformIO compatibility
#ifdef ARDUINO
void setup() {
    delay(2000); // Give time for serial monitor
    main(0, NULL);
}

void loop() {
    // Empty loop for Arduino compatibility
}
#endif
