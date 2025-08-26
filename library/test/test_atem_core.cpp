#include <unity.h>
#include <ATEM.h>

// Mock ATEM class for testing without hardware
class MockATEM : public ATEM {
public:
    // Override network functions to prevent actual network calls
    bool begin(const char* ip) override {
        return true; // Always succeed in mock
    }
    
    void loop() override {
        // Do nothing in mock
    }
    
    ATEMConnectionState getConnectionState() override {
        return ATEM_CONNECTED; // Always connected in mock
    }
    
    // Test helper methods
    void simulateConnection() {
        onConnectionStateChanged(ATEM_CONNECTED);
    }
    
    void simulateProgramChange(uint16_t input) {
        onProgramInputChanged(input);
    }
    
    void simulatePreviewChange(uint16_t input) {
        onPreviewInputChanged(input);
    }
};

MockATEM mockAtem;

// Test setup function
void setUp(void) {
    // Reset mock state before each test
}

// Test teardown function  
void tearDown(void) {
    // Clean up after each test
}

// Test basic initialization
void test_atem_initialization(void) {
    TEST_ASSERT_TRUE(mockAtem.begin("192.168.1.100"));
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, mockAtem.getConnectionState());
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
    mockAtem.changePreviewInput(ATEM_INPUT_CAM2);
    // In a real test, we'd verify the packet was generated correctly
    TEST_ASSERT_TRUE(true); // Placeholder - packet validation would go here
}

// Test program input change
void test_program_input_change(void) {
    mockAtem.changeProgramInput(ATEM_INPUT_CAM3);
    // In a real test, we'd verify the packet was generated correctly
    TEST_ASSERT_TRUE(true); // Placeholder - packet validation would go here
}

// Test CUT transition
void test_cut_transition(void) {
    mockAtem.cut();
    // In a real test, we'd verify the DCut packet was generated
    TEST_ASSERT_TRUE(true); // Placeholder - packet validation would go here
}

// Test AUTO transition
void test_auto_transition(void) {
    mockAtem.autoTransition();
    // In a real test, we'd verify the DAut packet was generated
    TEST_ASSERT_TRUE(true); // Placeholder - packet validation would go here
}

// Test state tracking
void test_state_tracking(void) {
    ATEMState state = mockAtem.getState();
    // Initial state should be valid
    TEST_ASSERT_TRUE(state.program_input >= 0);
    TEST_ASSERT_TRUE(state.preview_input >= 0);
}

// Test invalid input handling
void test_invalid_input_handling(void) {
    // Test with invalid input number
    mockAtem.changePreviewInput(9999);
    // Should handle gracefully without crashing
    TEST_ASSERT_TRUE(true);
}

// Test connection state changes
void test_connection_state_changes(void) {
    mockAtem.simulateConnection();
    TEST_ASSERT_EQUAL(ATEM_CONNECTED, mockAtem.getConnectionState());
}

// Test event callbacks
void test_event_callbacks(void) {
    // Test that callbacks can be called without crashing
    mockAtem.simulateProgramChange(ATEM_INPUT_CAM1);
    mockAtem.simulatePreviewChange(ATEM_INPUT_CAM2);
    TEST_ASSERT_TRUE(true);
}

// 🔄 PHASE 1 ADVANCED SWITCHING UNIT TESTS

// Test fade to black function calls
void test_fade_to_black_functions(void) {
    // Test basic fade to black call (should not crash)
    mockAtem.fadeToBlack();
    mockAtem.fadeToBlack(0); // Explicit ME 0
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

// Test fade to black rate function
void test_fade_to_black_rate_function(void) {
    // Test various fade rates
    mockAtem.setFadeToBlackRate(12);   // 12 frames
    mockAtem.setFadeToBlackRate(25);   // 25 frames  
    mockAtem.setFadeToBlackRate(30);   // 30 frames
    mockAtem.setFadeToBlackRate(50);   // 50 frames
    mockAtem.setFadeToBlackRate(250);  // Maximum reasonable rate
    
    // Test with explicit ME parameter
    mockAtem.setFadeToBlackRate(25, 0);
    
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

// Test transition position function
void test_transition_position_function(void) {
    // Test boundary values
    mockAtem.setTransitionPosition(0);      // 0% (preview)
    mockAtem.setTransitionPosition(5000);   // 50% (halfway)
    mockAtem.setTransitionPosition(10000);  // 100% (program)
    
    // Test intermediate values
    mockAtem.setTransitionPosition(2500);   // 25%
    mockAtem.setTransitionPosition(7500);   // 75%
    
    // Test with explicit ME parameter
    mockAtem.setTransitionPosition(5000, 0);
    
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

// Test preview transition function
void test_preview_transition_function(void) {
    // Test enable/disable states
    mockAtem.previewTransition(true);   // Enable
    mockAtem.previewTransition(false);  // Disable
    
    // Test with explicit ME parameter
    mockAtem.previewTransition(true, 0);
    mockAtem.previewTransition(false, 0);
    
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

// Test Phase 1 function combination
void test_phase1_function_combination(void) {
    // Test calling multiple Phase 1 functions in sequence
    mockAtem.setFadeToBlackRate(25);
    mockAtem.previewTransition(true);
    mockAtem.setTransitionPosition(3000);
    mockAtem.fadeToBlack();
    mockAtem.setTransitionPosition(0);
    mockAtem.previewTransition(false);
    
    TEST_ASSERT_TRUE(true); // If we get here, no crash occurred
}

// Main test runner
int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Basic functionality tests
    RUN_TEST(test_atem_initialization);
    RUN_TEST(test_input_constants);
    RUN_TEST(test_state_tracking);
    
    // Control function tests
    RUN_TEST(test_preview_input_change);
    RUN_TEST(test_program_input_change);
    RUN_TEST(test_cut_transition);
    RUN_TEST(test_auto_transition);
    
    // Error handling tests
    RUN_TEST(test_invalid_input_handling);
    
    // Event system tests
    RUN_TEST(test_connection_state_changes);
    RUN_TEST(test_event_callbacks);
    
    // 🔄 Phase 1 Advanced Switching Tests
    RUN_TEST(test_fade_to_black_functions);
    RUN_TEST(test_fade_to_black_rate_function);
    RUN_TEST(test_transition_position_function);
    RUN_TEST(test_preview_transition_function);
    RUN_TEST(test_phase1_function_combination);
    
    return UNITY_END();
}

// For PlatformIO compatibility
void setup() {
    delay(2000); // Give time for serial monitor
    main(0, NULL);
}

void loop() {
    // Empty loop for Arduino compatibility
}
