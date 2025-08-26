/*
 * ATEM.h
 * ESP32 Arduino Library for Blackmagic Design ATEM Video Switchers
 * 
 * Copyright (c) 2025 Mirza Ceyzar <mirza.ceyzar@gmail.com>
 * Licensed under the Apache License 2.0
 * 
 * Based on protocol specifications from the Sofie ATEM Connection library:
 * https://github.com/Sofie-Automation/sofie-atem-connection
 * Copyright (c) 2023 Norsk rikskringkasting AS (MIT License)
 * 
 * This is an independent C++/Arduino implementation for ESP32 platforms.
 * See THIRD_PARTY_LICENSES.md for complete attribution details.
 */

#ifndef ATEM_H
#define ATEM_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include "ATEM_Inputs.h"

// Version Information
#define ATEM_ESP32_VERSION "v2.0.0"
#define ATEM_ESP32_BUILD_DATE __DATE__ " " __TIME__

// ATEM Protocol Constants (based on Sofie ATEM Connection library)
#define ATEM_PORT                    9910
#define LOCAL_PORT                   9910
#define CONNECTION_TIMEOUT           5000      // 5 seconds connection timeout
#define CONNECTION_RETRY_INTERVAL    1000      // 1 second reconnection check
#define RETRANSMIT_INTERVAL          10        // 10ms retransmit check
#define HEARTBEAT_INTERVAL           500       // 500ms heartbeat (matching Sofie library exactly)
#define MAX_PACKET_SIZE              1500
#define HEADER_SIZE                  12
// Retransmission settings - based on Sofie analysis showing retransmit FROM packet 1
// ATEM can request retransmission of very old packets, need to store many more
#define MAX_RETRANSMIT_PACKETS       100       // Store last 100 packets instead of 10

// Logging Levels - configurable in user code before including ATEM.h
enum ATEMLogLevel {
  ATEM_LOG_NONE                  = 0,        // No logging
  ATEM_LOG_ERROR                 = 1,        // Errors only
  ATEM_LOG_WARN                  = 2,        // Warnings and errors
  ATEM_LOG_INFO                  = 3,        // Info, warnings, and errors
  ATEM_LOG_DEBUG                 = 4,        // Debug info and above
  ATEM_LOG_VERBOSE               = 5         // All detailed protocol info
};

// Default log level - can be overridden in .ino file
#ifndef ATEM_DEFAULT_LOG_LEVEL
#define ATEM_DEFAULT_LOG_LEVEL ATEM_LOG_INFO
#endif

// ATEM Protocol Flags (exact match to Sofie ATEM Connection library)
#define FLAG_ACK_REQUEST             0x01
#define FLAG_NEW_SESSION_ID          0x02
#define FLAG_IS_RETRANSMIT           0x04
#define FLAG_RETRANSMIT_REQUEST      0x08
#define FLAG_ACK_REPLY               0x10

// ATEM Connection States
enum ATEMConnectionState {
  ATEM_DISCONNECTED,
  ATEM_CONNECTING,
  ATEM_CONNECTED,
  ATEM_ERROR
};

// ATEM Input Sources (based on Sofie library)
#define ATEM_INPUT_BLACK             0
#define ATEM_INPUT_CAM1              1
#define ATEM_INPUT_CAM2              2
#define ATEM_INPUT_CAM3              3
#define ATEM_INPUT_CAM4              4
#define ATEM_INPUT_CAM5              5
#define ATEM_INPUT_CAM6              6
#define ATEM_INPUT_CAM7              7
#define ATEM_INPUT_CAM8              8
#define ATEM_INPUT_COLORBARS         1000
#define ATEM_INPUT_COLOR1            2001
#define ATEM_INPUT_COLOR2            2002
#define ATEM_INPUT_MEDIA1            3010
#define ATEM_INPUT_MEDIA2            3020

// ATEM Packet Structure
struct ATEMPacketHeader {
  uint8_t flags;
  uint8_t length_high;
  uint8_t length_low;
  uint8_t unknown;
  uint16_t session_id;
  uint16_t acked_packet_id;
  uint16_t packet_id;
};

// ATEM Command Structure
struct ATEMCommand {
  uint16_t length;
  uint16_t unknown;
  char name[4];
  uint8_t data[1480]; // Max command data size
};

// ATEM State Structure (simplified for initial implementation)
struct ATEMState {
  uint16_t program_input;
  uint16_t preview_input;
  bool in_transition;
  uint8_t transition_position;
};

class ATEM {
public:
  // Constructor and Destructor
  /**
   * @brief Constructor - Initializes ATEM object with default values
   * Sets up initial connection state, session ID, and internal variables
   */
  ATEM();
  
  /**
   * @brief Destructor - Cleans up resources and closes connections
   * Automatically disconnects from ATEM if connected
   */
  ~ATEM();
  
  // Connection Management
  /**
   * @brief Initialize ATEM connection to specified IP address
   * @param ip IPAddress of the ATEM switcher
   * @return true if initialization and connection successful, false otherwise
   * Prints version info, sets up UDP, tests connectivity, and attempts handshake
   */
  bool begin(IPAddress ip);
  
  /**
   * @brief Establish connection handshake with ATEM switcher
   * @return true if handshake successful, false if timeout or error
   * Sends HELLO packet and waits for response to establish session
   */
  bool connect();
  
  /**
   * @brief Disconnect from ATEM switcher and cleanup resources
   * Sends disconnect notification and stops UDP communication
   */
  void disconnect();
  
  /**
   * @brief Check if currently connected to ATEM switcher
   * @return true if connected, false otherwise
   */
  bool isConnected();
  
  /**
   * @brief Get current connection state
   * @return ATEMConnectionState enum value (DISCONNECTED, CONNECTING, CONNECTED, ERROR)
   */
  ATEMConnectionState getConnectionState();
  
  // Main Loop - call this frequently
  /**
   * @brief Main processing loop - MUST be called frequently in main loop
   * Processes incoming packets, sends heartbeats, handles timeouts, and triggers events
   * Call this at least every 10ms for proper operation
   */
  void runLoop();
  
  // State Access
  /**
   * @brief Get complete current ATEM state
   * @return ATEMState struct containing program/preview inputs and transition status
   */
  ATEMState getState();
  
  /**
   * @brief Get current program input number
   * @return uint16_t program input ID (e.g., 1=CAM1, 2=CAM2, etc.)
   */
  uint16_t getProgramInput();
  
  /**
   * @brief Get current preview input number  
   * @return uint16_t preview input ID (e.g., 1=CAM1, 2=CAM2, etc.)
   */
  uint16_t getPreviewInput();
  
  
  // ===========================================
  // CONTROL FUNCTIONS - PHASE 2 IMPLEMENTATION
  // ===========================================
  
  // ✅ BASIC SWITCHING (3/4 COMPLETE - MAJOR PROGRESS!)
  /**
   * @brief Change preview input on ATEM switcher ✅ WORKING!
   * @param input Input ID to switch to preview (1=CAM1, 2=CAM2, etc.) 
   */
  void changePreviewInput(uint16_t input);
  
  /**
   * @brief Change program input on ATEM switcher ✅ WORKING!
   * @param input Input ID to switch to program (1=CAM1, 2=CAM2, etc.)
   */
  void changeProgramInput(uint16_t input);
  
  /**
   * @brief Perform CUT transition (immediate switch) ✅ IMPLEMENTED!
   * Immediately cuts from preview to program using DCut command
   */
  void cut();
  
  /**
   * @brief Perform AUTO transition (fade/wipe) ✅ IMPLEMENTED!
   * Performs transition effect from preview to program using DAut command
   */
  void autoTransition();
  
  // 🔄 ADVANCED SWITCHING
  /**
   * @brief Fade to black or fade from black
   * @param me Mix effect index (default 0)
   * TODO: Send FtbS command
   */
  void fadeToBlack(uint8_t me = 0);
  
  /**
   * @brief Set fade to black rate
   * @param rate Rate in frames
   * @param me Mix effect index (default 0)
   * TODO: Send FtbP command  
   */
  void setFadeToBlackRate(uint16_t rate, uint8_t me = 0);
  
  /**
   * @brief Set transition position manually
   * @param position Position 0-10000 (0=preview, 10000=program)
   * @param me Mix effect index (default 0)
   * TODO: Send CTPs command
   */
  void setTransitionPosition(uint16_t position, uint8_t me = 0);
  
  /**
   * @brief Enable/disable transition preview
   * @param on Enable preview transition
   * @param me Mix effect index (default 0)
   * TODO: Send CTPr command
   */
  void previewTransition(bool on, uint8_t me = 0);
  
  // 🔄 AUX & DOWNSTREAM KEYS
  /**
   * @brief Set AUX output source
   * @param source Input source ID
   * @param bus AUX bus index (default 0)
   * TODO: Send CAuS command
   */
  void setAuxSource(uint16_t source, uint8_t bus = 0);
  
  /**
   * @brief Set downstream key on air state
   * @param onAir Enable/disable downstream key
   * @param key Downstream key index (default 0)
   * TODO: Send CDsL command
   */
  void setDownstreamKeyOnAir(bool onAir, uint8_t key = 0);
  
  /**
   * @brief Auto transition downstream key
   * @param key Downstream key index (default 0)
   * @param isTowardsOnAir Direction of auto transition
   * TODO: Send DDsA command
   */
  void autoDownstreamKey(uint8_t key = 0, bool isTowardsOnAir = true);
  
  // 🔄 UPSTREAM KEYS
  /**
   * @brief Set upstream keyer on air state
   * @param onAir Enable/disable upstream keyer
   * @param me Mix effect index (default 0)
   * @param keyer Keyer index (default 0)
   * TODO: Send CKOn command
   */
  void setUpstreamKeyerOnAir(bool onAir, uint8_t me = 0, uint8_t keyer = 0);
  
  /**
   * @brief Set upstream keyer cut source
   * @param cutSource Input source for cut
   * @param me Mix effect index (default 0)  
   * @param keyer Keyer index (default 0)
   * TODO: Send CKeC command
   */
  void setUpstreamKeyerCutSource(uint16_t cutSource, uint8_t me = 0, uint8_t keyer = 0);
  
  /**
   * @brief Set upstream keyer fill source
   * @param fillSource Input source for fill
   * @param me Mix effect index (default 0)
   * @param keyer Keyer index (default 0)
   * TODO: Send CKeF command
   */
  void setUpstreamKeyerFillSource(uint16_t fillSource, uint8_t me = 0, uint8_t keyer = 0);
  
  // 🔄 MEDIA & SETTINGS
  /**
   * @brief Set color generator color
   * @param hue Hue value (0.0-359.9)
   * @param saturation Saturation (0.0-1.0)
   * @param lightness Lightness (0.0-1.0)
   * @param index Color generator index (default 0)
   * TODO: Send CClV command
   */
  void setColorGeneratorColour(float hue, float saturation, float lightness, uint8_t index = 0);
  
  /**
   * @brief Set media player source
   * @param sourceType Source type (1=still, 2=clip)
   * @param sourceIndex Source index
   * @param player Media player index (default 0)
   * TODO: Send MPCS command
   */
  void setMediaPlayerSource(uint8_t sourceType, uint8_t sourceIndex, uint8_t player = 0);
  
  /**
   * @brief Set multiviewer window source
   * @param source Input source ID
   * @param mv Multiviewer index (default 0)
   * @param window Window index (default 0)
   * TODO: Send CMvI command
   */
  void setMultiViewerWindowSource(uint16_t source, uint8_t mv = 0, uint8_t window = 0);
  
  // 🔄 AUDIO (BASIC)
  /**
   * @brief Set classic audio mixer input gain
   * @param input Input index
   * @param gain Gain level (-60.0 to 6.0 dB)
   * TODO: Send CAIP command
   */
  void setClassicAudioMixerInputGain(uint8_t input, float gain);
  
  /**
   * @brief Set classic audio mixer master gain
   * @param gain Master gain level (-60.0 to 6.0 dB)
   * TODO: Send CAMP command
   */
  void setClassicAudioMixerMasterGain(float gain);
  
  // Event Callbacks - override these for custom behavior
  /**
   * @brief Callback triggered when connection state changes
   * @param state New ATEMConnectionState value
   * Override this method to handle connection events in your application
   */
  virtual void onConnectionStateChanged(ATEMConnectionState state);
  
  /**
   * @brief Callback triggered when program input changes
   * @param input New program input ID
   * Override this method to handle program input changes in your application
   */
  virtual void onProgramInputChanged(uint16_t input);
  
  /**
   * @brief Callback triggered when preview input changes
   * @param input New preview input ID
   * Override this method to handle preview input changes in your application
   */
  virtual void onPreviewInputChanged(uint16_t input);
  
  /**
   * @brief Callback triggered when any ATEM state changes
   * Called after any state variable is updated (program, preview, transition)
   * Override this method for general state change handling
   */
  virtual void onStateChanged();
  
  // Debug functions
  /**
   * @brief Enable or disable debug output to Serial
   * @param enable true to enable debug output, false to disable
   * When enabled, prints detailed packet information and protocol events
   * @deprecated Use setLogLevel() instead for fine-grained control
   */
  void enableDebug(bool enable) { _log_level = enable ? ATEM_LOG_DEBUG : ATEM_LOG_INFO; }
  
  /**
   * @brief Set logging verbosity level
   * @param level ATEMLogLevel (NONE=0, ERROR=1, WARN=2, INFO=3, DEBUG=4, VERBOSE=5)
   * Controls how much information is printed to Serial console
   */
  void setLogLevel(ATEMLogLevel level) { _log_level = level; }
  
  /**
   * @brief Get current logging level
   * @return Current ATEMLogLevel setting
   */
  ATEMLogLevel getLogLevel() { return _log_level; }
  
  /**
   * @brief Print detailed connection information to Serial
   * Displays current state, IP, session ID, packet counters, and input states
   * Useful for debugging connection issues
   */
  void printConnectionInfo();
  
  /**
   * @brief Print library version information to Serial
   * Displays version number, build date, and library basis
   * Called automatically during begin() after Serial is ready
   */
  void printVersionInfo();
  
  /**
   * @brief Get version string
   * @return const char* pointer to version string (e.g., "v1.0.4")
   */
  const char* getVersion() { return ATEM_ESP32_VERSION; }
  
  /**
   * @brief Print packet in Sofie-compatible format for comparison
   * @param prefix Either "SEND" or "RECV" 
   * @param data Pointer to packet data
   * @param length Packet length
   */
  void printSofieFormat(const char* prefix, uint8_t* data, int length);

private:
  // Network
  WiFiUDP _udp;                    // UDP socket for ATEM communication
  IPAddress _switcher_ip;          // IP address of ATEM switcher
  bool _udp_initialized;           // Track UDP socket initialization status
  
  // Connection State
  ATEMConnectionState _connection_state;  // Current connection status
  uint16_t _session_id;                   // Session ID for this connection
  uint16_t _local_packet_id;              // Counter for outgoing packets
  uint16_t _remote_packet_id;             // Last received packet ID from ATEM
  unsigned long _last_heartbeat;          // Timestamp of last heartbeat sent
  unsigned long _last_received;           // Timestamp of last packet received
  unsigned long _connection_start_time;   // When connection attempt started
  
  // ATEM State
  ATEMState _state;                // Current ATEM switcher state
  bool _state_dirty;               // Flag indicating state has changed
  
  // Packet Retransmission Storage
  struct StoredPacket {
    uint16_t packet_id;            // Packet ID for this stored packet
    uint8_t data[MAX_PACKET_SIZE]; // Packet data
    int length;                    // Actual length of packet data
    unsigned long timestamp;       // When packet was sent
    bool in_use;                   // Whether this slot is occupied
  };
  StoredPacket _sent_packets[MAX_RETRANSMIT_PACKETS]; // Circular buffer for sent packets
  int _packet_buffer_index;        // Current index in circular buffer
  
  // Logging
  ATEMLogLevel _log_level;         // Current logging verbosity level
  
  // Protocol Functions
  /**
   * @brief Process incoming UDP packets from ATEM
   * Reads available UDP data, validates packet size, and calls parsePacket()
   * Updates _last_received timestamp and prints debug info if enabled
   */
  void processIncomingPacket();
  
  /**
   * @brief Parse received ATEM packet and handle protocol logic
   * @param buffer Pointer to packet data
   * @param length Length of packet data
   * @return true if packet processed successfully, false on error
   * Handles connection responses, ACKs, and command processing
   */
  bool parsePacket(uint8_t* buffer, int length);
  
  /**
   * @brief Process individual ATEM command (unused - placeholder)
   * @param cmd Pointer to ATEMCommand structure
   * TODO: Future implementation for structured command processing
   */
  void processCommand(ATEMCommand* cmd);
  
  /**
   * @brief Send packet to ATEM switcher (unused - placeholder)
   * @param flags Packet flags byte
   * @param data Optional data payload
   * @param data_length Length of data payload
   * TODO: Future implementation for general packet sending
   */
  void sendPacket(uint8_t flags, uint8_t* data = nullptr, int data_length = 0);
  
  /**
   * @brief Send heartbeat packet to maintain connection
   * Sends reliable packet with current session and packet IDs
   * Must be called regularly to prevent connection timeout
   */
  void sendHeartbeat();
  
  /**
   * @brief Send acknowledgment packet for received data
   * @param packet_id ID of packet being acknowledged
   * Required for reliable packet delivery in ATEM protocol
   */
  void sendAck(uint16_t packet_id);
  
  /**
   * @brief Store outgoing packet for potential retransmission
   * @param packet_id ID of the packet being sent
   * @param data Pointer to packet data
   * @param length Length of packet data
   * Stores packet in circular buffer for retransmission on request
   */
  void storePacketForRetransmission(uint16_t packet_id, uint8_t* data, int length);
  
  /**
   * @brief Handle retransmit request from ATEM
   * @param requested_packet_id ID of packet requested for retransmission
   * Searches stored packets and resends if found, or sends current sync if not
   */
  void handleRetransmitRequest(uint16_t requested_packet_id, uint16_t sequence_to_ack);
  
  // Command Processing
  /**
   * @brief Process command data payload from ATEM packet
   * @param data Pointer to command data (after packet header)
   * @param length Length of command data
   * Parses multiple commands in single packet and dispatches to specific handlers
   */
  void processInitialPayload(uint8_t* data, int length);
  
  /**
   * @brief Process Program Input (PrgI) command from ATEM
   * @param data Pointer to command-specific data
   * @param length Length of command data
   * Updates internal program input state and triggers events
   */
  void processProgramInput(uint8_t* data, int length);
  
  /**
   * @brief Process Preview Input (PrvI) command from ATEM  
   * @param data Pointer to command-specific data
   * @param length Length of command data
   * Updates internal preview input state and triggers events
   */
  void processPreviewInput(uint8_t* data, int length);
  
  // Utility Functions
  /**
   * @brief Extract packet length from packet header (unused - placeholder)
   * @param buffer Pointer to packet data
   * @return uint16_t packet length value
   * TODO: Future implementation for packet validation
   */
  uint16_t getPacketLength(uint8_t* buffer);
  
  /**
   * @brief Print debug message with [ATEM] prefix
   * @param message Debug message string
   * Only prints if debug is enabled via enableDebug(true)
   * @deprecated Use logError(), logWarn(), etc. instead
   */
  void debugPrint(const char* message);
  
  /**
   * @brief Print hexadecimal dump of data for debugging
   * @param data Pointer to data to dump
   * @param length Number of bytes to dump
   * Prints hex bytes with bounds checking, 16 bytes per line
   */
  void debugPrintHex(uint8_t* data, int length);
  
  // Logging Functions
  /**
   * @brief Print error message if log level allows
   * @param message Error message string
   */
  void logError(const char* message);
  
  /**
   * @brief Print warning message if log level allows
   * @param message Warning message string
   */
  void logWarn(const char* message);
  
  /**
   * @brief Print info message if log level allows
   * @param message Info message string
   */
  void logInfo(const char* message);
  
  /**
   * @brief Print debug message if log level allows
   * @param message Debug message string
   */
  void logDebug(const char* message);
  
  /**
   * @brief Print verbose message if log level allows
   * @param message Verbose message string
   */
  void logVerbose(const char* message);
  
  /**
   * @brief Print formatted log message with timestamp
   * @param level Log level for this message
   * @param format Printf-style format string
   * @param ... Additional arguments for format string
   */
  void logPrintf(ATEMLogLevel level, const char* format, ...);
};

#endif // ATEM_H
