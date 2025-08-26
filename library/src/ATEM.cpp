/*
 * ATEM.cpp
 * Implementation of ATEM communication protocol for ESP32
 * Based on Sofie ATEM Connection library
 * 
 * This file contains the implementation of the ATEM class
 * for communicating with Blackmagic Design ATEM switchers
 * 
 * Version: 1.1.0 - Static analysis improvements and enhanced error handling.
 * Fixed UDP initialization tracking, improved packet format handling,
 * added comprehensive error checking for network operations, and enhanced
 * documentation for all debugging features.
 */

#include "ATEM.h"

/**
 * @brief Constructor - Initialize ATEM object with default values
 * Sets up connection state, initial session ID, initializes counters and state
 * Version info will be printed when begin() is called after Serial is ready
 */

ATEM::ATEM() {
  _connection_state        = ATEM_DISCONNECTED;
  _session_id              = 0x53AB;  // Initial session ID for HELLO - ATEM will assign the real one
  _local_packet_id         = 768;     // Start with 768 to match HELLO packet ID that ATEM expects
  _remote_packet_id        = 0;
  _last_heartbeat          = 0;
  _last_received           = 0;
  _connection_start_time   = 0;
  _state_dirty             = false;
  _log_level               = (ATEMLogLevel)ATEM_DEFAULT_LOG_LEVEL;  // Initialize with default log level
  _udp_initialized         = false;   // Initialize UDP status flag
  
  // Initialize packet retransmission storage
  _packet_buffer_index = 0;
  for (int i = 0; i < MAX_RETRANSMIT_PACKETS; i++) {
    _sent_packets[i].in_use     = false;
    _sent_packets[i].packet_id  = 0;
    _sent_packets[i].length     = 0;
    _sent_packets[i].timestamp  = 0;
  }
  
  // Initialize state
  _state.program_input        = 0;
  _state.preview_input        = 0;
  _state.in_transition        = false;
  _state.transition_position  = 0;
  
  // Version info will be printed in begin() after Serial is ready
}

/**
 * @brief Destructor - Clean up resources and close connections
 * Automatically disconnects from ATEM if currently connected
 */
ATEM::~ATEM() {
  disconnect();
}

/**
 * @brief Initialize ATEM connection to specified IP address
 * @param ip IPAddress of the ATEM switcher to connect to
 * @return true if initialization and connection successful, false otherwise
 * 
 * This function:
 * 1. Prints version information to Serial
 * 2. Sets up UDP socket on LOCAL_PORT
 * 3. Tests basic network connectivity to ATEM
 * 4. Attempts connection handshake
 */

/**
 * @brief Initialize ATEM connection to specified IP address with network diagnostics
 * @param ip IPAddress of the ATEM switcher to connect to
 * @return true if initialization and connection successful, false otherwise
 * 
 * Enhanced initialization process includes:
 * 1. Print version information to Serial for debugging
 * 2. Set up UDP socket on LOCAL_PORT with error checking
 * 3. Perform comprehensive network connectivity tests:
 *    - TCP connectivity test to verify basic network reachability
 *    - UDP socket binding verification with port confirmation
 *    - UDP send capability test with response validation
 * 4. Attempt ATEM protocol handshake with enhanced error reporting
 * 
 * Network diagnostic features help identify:
 * - Network routing/firewall issues (TCP test)
 * - UDP socket problems (binding test)
 * - Basic UDP functionality (send test)
 * - ATEM protocol-specific issues (handshake analysis)
 */
bool ATEM::begin(IPAddress ip) {
  _switcher_ip = ip;
  
  // Print version info now that Serial is ready
  printVersionInfo();
  
  debugPrint("Initializing ATEM connection...");
  
  // Initialize UDP
  if (!_udp.begin(LOCAL_PORT)) {
    debugPrint("Failed to initialize UDP");
    _udp_initialized = false;
    return false;
  }
  
  _udp_initialized = true;  // Mark UDP as successfully initialized
  debugPrint("UDP initialized successfully");
  
  // Test basic network connectivity to ATEM
  debugPrint("Testing network connectivity to ATEM...");
  
  // Test basic ping connectivity
  logPrintf(ATEM_LOG_INFO, "Testing ping to ATEM at %s", _switcher_ip.toString().c_str());
  
  // Test if we can reach the ATEM at all
  WiFiClient testClient;
  if (testClient.connect(_switcher_ip, ATEM_PORT)) {
    debugPrint("TCP connection to ATEM:9910 successful");
    testClient.stop();
  } else {
    debugPrint("WARNING: Cannot TCP connect to ATEM:9910");
    debugPrint("This might be normal - ATEM may only accept UDP");
  }
  
  // Test UDP socket binding
  logPrintf(ATEM_LOG_INFO, "UDP socket bound to local port: %d", LOCAL_PORT);
  logPrintf(ATEM_LOG_INFO, "Will send to ATEM IP: %s port: %d", 
           _switcher_ip.toString().c_str(), ATEM_PORT);
  
  // Test basic UDP send (ping-like test)
  debugPrint("Testing UDP send capability...");
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  uint8_t test_packet[] = {0x00, 0x04, 0x00, 0x00}; // Minimal 4-byte test
  int sent = _udp.write(test_packet, 4);
  bool send_result = _udp.endPacket();
  logPrintf(ATEM_LOG_DEBUG, "Test UDP send result: wrote %d bytes, endPacket: %s", 
           sent, send_result ? "SUCCESS" : "FAILED");
  
  return connect();
}

/**
 * @brief Establish connection handshake with ATEM switcher
 * @return true if handshake successful, false if timeout or error
 * 
 * Sends HELLO packet with 20-byte format:
 * - Bytes 0-3: Flags and length (0x10, 0x14, 0x00, 0x00)
 * - Bytes 4-5: Session ID
 * - Bytes 6-11: Packet IDs and padding
 * 
 * Waits up to CONNECTION_TIMEOUT ms for response with 0x02 flag
 */
/**
 * @brief Establish connection handshake with ATEM switcher (Enhanced Debugging Version)
 * @return true if handshake successful, false if timeout or error
 * 
 * Enhanced HELLO packet transmission with comprehensive error checking:
 * - Validates UDP socket initialization before sending
 * - Sends 20-byte HELLO packet with corrected format matching working implementations
 * - Monitors packet transmission success (bytes written, endPacket result)
 * - Provides periodic status updates during connection wait period
 * - Logs all received packets with detailed analysis during handshake
 * 
 * HELLO packet structure (20 bytes):
 * - Byte 0: 0x10 (Hello flag)
 * - Byte 1: 0x14 (Length = 20 bytes)
 * - Bytes 2-3: Session ID (big-endian)
 * - Bytes 4-5: 0x00 (padding)
 * - Bytes 6-7: Acked packet ID (0x00 for initial)
 * - Bytes 8-9: Local packet ID (0x003A matches working example)
 * - Bytes 10-19: Padding (0x00)
 * 
 * Waits up to CONNECTION_TIMEOUT ms for HELLO_RESPONSE (0x02 flag)
 * Enhanced debugging shows exactly what packets are received and why they
 * are or aren't recognized as valid HELLO_RESPONSE packets.
 */
bool ATEM::connect() {
  debugPrint("Attempting to connect to ATEM...");
  _connection_state = ATEM_CONNECTING;
  _connection_start_time = millis();
  
  logPrintf(ATEM_LOG_INFO, "Using initial session ID for HELLO: 0x%04X", _session_id);
  
  // Send HELLO packet - EXACT format from working JavaScript analysis
  // Working packet: 101453ab00000000003a00000100000000000000
  uint8_t hello_packet[20] = {
    0x10, 0x14, 0x53, 0xab, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  
  // The packet ID for this HELLO is embedded as 0x003a (58) in the packet
  // But ATEM will later request retransmission using a different ID scheme
  uint16_t hello_storage_id = 768; // ID that ATEM will request for retransmit
  
  logPrintf(ATEM_LOG_DEBUG, "HELLO packet uses embedded ID 0x3a, stored as ID: %d", hello_storage_id);
  
  debugPrint("Sending HELLO packet...");
  debugPrintHex(hello_packet, 20);
  
  logPrintf(ATEM_LOG_DEBUG, "Sending to: %s:%d from local port: %d", 
           _switcher_ip.toString().c_str(), ATEM_PORT, LOCAL_PORT);
  
  // Update version to reflect increased packet storage and new logging system
  logInfo("*** ATEM PROTOCOL v1.10.1 ***");
  logInfo("CRITICAL FIX: Increased packet storage to 100 packets");
  logInfo("Analysis showed ATEM requests retransmit FROM packet ID 1");
  logInfo("Previous 10-packet buffer was too small, causing retransmit failures");
  logInfo("Now storing last 100 packets for proper retransmission");
  
  _connection_state = ATEM_CONNECTING;
  _connection_start_time = millis();
  
  // HELLO packet should NOT be stored for retransmission per Sofie library
  // Only data packets with AckRequest flag are stored in _inFlight array
  
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  size_t bytes_written = _udp.write(hello_packet, 20);
  bool send_success = _udp.endPacket();
  
  // Log in Sofie format for comparison
  printSofieFormat("SEND", hello_packet, 20);
  
  if (bytes_written != 20 || !send_success) {
    logPrintf(ATEM_LOG_ERROR, "HELLO packet send failed - wrote %d/20 bytes, endPacket: %s", 
             bytes_written, send_success ? "OK" : "FAILED");
    _connection_state = ATEM_ERROR;
    return false;
  }
  
  logPrintf(ATEM_LOG_INFO, "HELLO packet sent successfully (%d bytes) - NOT stored for retransmission", 
           bytes_written);
  
  // Set our local packet ID counter to start from 1 for data packets
  _local_packet_id = 1;  // First data packet will be ID 1
  
  // Set our local packet ID counter to start after the HELLO
  _local_packet_id = 1;  // Next packets will be 1, 2, 3, etc.
  
  debugPrint("HELLO packet sent, waiting for response...");
  debugPrint("Expected HELLO_RESPONSE packet - any packet will be logged");
  
  // Add a counter to show we're actively checking for packets
  int check_count = 0;
  
  // Wait for response
  unsigned long start_time = millis();
  while (millis() - start_time < CONNECTION_TIMEOUT) {
    processIncomingPacket();
    
    if (_connection_state == ATEM_CONNECTED) {
      debugPrint("Successfully connected to ATEM!");
      onConnectionStateChanged(_connection_state);
      return true;
    }
    
    // Show periodic status during wait
    check_count++;
    if (check_count % 100 == 0) { // Every ~1 second (100 * 10ms)
      logPrintf(ATEM_LOG_DEBUG, "Still waiting for response... (%ds) - checking for packets every 10ms",
               (millis() - start_time) / 1000);
    }
    
    delay(10);
  }
  
  debugPrint("Connection timeout");
  logError("=== CONNECTION TIMEOUT ANALYSIS ===");
  logPrintf(ATEM_LOG_ERROR, "Waited %d ms for HELLO response", CONNECTION_TIMEOUT);
  logPrintf(ATEM_LOG_ERROR, "Sent HELLO to: %s:%d", _switcher_ip.toString().c_str(), ATEM_PORT);
  logPrintf(ATEM_LOG_ERROR, "Listening on local port: %d", LOCAL_PORT);
  logError("No packets were received during this time");
  logError("Possible issues:");
  logError("1. ATEM device is not responding");
  logError("2. Network routing/firewall issues");
  logError("3. ATEM is on different network segment");
  logError("4. ATEM port 9910 is not accessible");
  logError("========================================");
  _connection_state = ATEM_ERROR;
  onConnectionStateChanged(_connection_state);
  return false;
}

/**
 * @brief Disconnect from ATEM switcher and cleanup resources
 * Sets connection state to DISCONNECTED and stops UDP communication
 * Triggers onConnectionStateChanged callback if currently connected
 */
void ATEM::disconnect() {
  if (_connection_state == ATEM_CONNECTED) {
    debugPrint("Disconnecting from ATEM...");
    _connection_state = ATEM_DISCONNECTED;
    onConnectionStateChanged(_connection_state);
  }
  _udp.stop();
  _udp_initialized = false;  // Reset UDP initialization status
}

/**
 * @brief Check if currently connected to ATEM switcher
 * @return true if connection state is ATEM_CONNECTED, false otherwise
 */
bool ATEM::isConnected() {
  return _connection_state == ATEM_CONNECTED;
}

/**
 * @brief Get current connection state
 * @return ATEMConnectionState enum value
 * Possible values: ATEM_DISCONNECTED, ATEM_CONNECTING, ATEM_CONNECTED, ATEM_ERROR
 */
ATEMConnectionState ATEM::getConnectionState() {
  return _connection_state;
}

/**
 * @brief Main processing loop - MUST be called frequently in main loop
 * 
 * This function handles:
 * 1. Processing incoming UDP packets from ATEM
 * 2. Sending periodic heartbeat packets (every 500ms - matching Sofie library)
 * 3. Detecting connection timeouts and handling retransmission requests
 * 4. Triggering state change events when internal state is modified
 * 
 * Call this at least every 10ms for proper operation
 */

void ATEM::runLoop() {
  // Process incoming packets
  processIncomingPacket();
  
  unsigned long current_time = millis();
  
  // Send heartbeat every 500ms when connected (matching Sofie library exactly)
  if (_connection_state == ATEM_CONNECTED && 
      current_time - _last_heartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    _last_heartbeat = current_time;
  }
  
  // Check for connection timeout with detailed logging
  if (_connection_state == ATEM_CONNECTED && 
      current_time - _last_received > CONNECTION_TIMEOUT) {
    logPrintf(ATEM_LOG_ERROR, "[T+%dms] CONNECTION TIMEOUT DETECTED! Last packet received at T+%dms, timeout threshold: %dms, gap: %dms",
             current_time, _last_received, CONNECTION_TIMEOUT, current_time - _last_received);
    
    debugPrint("Connection timeout - no packets received");
    _connection_state = ATEM_ERROR;
    onConnectionStateChanged(_connection_state);
  }
  
  // Notify if state changed
  if (_state_dirty) {
    onStateChanged();
    _state_dirty = false;
  }
}

/**
 * @brief Process incoming UDP packets from ATEM switcher
 * 
 * Checks for available UDP data, reads into buffer with bounds checking,
 * validates minimum packet size, updates last received timestamp,
 * prints debug information if enabled, and calls parsePacket() for processing
 */
void ATEM::processIncomingPacket() {
  int packet_size = _udp.parsePacket();
  if (packet_size <= 0) {
    return;
  }
  
  // Enhanced packet logging with timestamps
  unsigned long current_time = millis();
  logPrintf(ATEM_LOG_VERBOSE, "*** PACKET RECEIVED! Size: %d bytes from %s:%d to local port: %d at T+%dms (%s) ***",
           packet_size, _udp.remoteIP().toString().c_str(), _udp.remotePort(), LOCAL_PORT, current_time,
           (_last_received > 0) ? ("gap: " + String(current_time - _last_received) + "ms").c_str() : "first packet");
  
  // Log that we received ANY packet during connection
  if (_connection_state == ATEM_CONNECTING) {
    logDebug("This packet was received during connection attempt!");
  }
  
  uint8_t buffer[MAX_PACKET_SIZE];
  int length = _udp.read(buffer, sizeof(buffer));
  
  // Log in Sofie format for comparison
  printSofieFormat("RECV", buffer, length);
  
  logPrintf(ATEM_LOG_DEBUG, "Successfully read %d bytes from UDP socket", length);
  
  if (length < HEADER_SIZE) {
    logPrintf(ATEM_LOG_ERROR, "Packet too short (%d bytes, need at least %d)", length, HEADER_SIZE);
    return;
  }
  
  _last_received = millis();
  
  // Always show packet content during debugging
  logPrintf(ATEM_LOG_VERBOSE, "Packet content (first %d bytes): ", 
           (length > 32) ? 32 : length);
  if (_log_level >= ATEM_LOG_VERBOSE) {
    Serial.print("[ATEM VERBOSE] ");
    for (int i = 0; i < ((length > 32) ? 32 : length); i++) {
      if (buffer[i] < 16) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    if (length > 32) Serial.print("...");
    Serial.println();
  }
  
  parsePacket(buffer, length);
}

/**
 * @brief Parse received ATEM packet and handle protocol logic
 * @param buffer Pointer to received packet data
 * @param length Length of received packet
 * @return true if packet processed successfully, false on error
 * 
 * Packet structure (12-byte header):
 * - Byte 0: Flags (0x01=Hello, 0x02=Hello response, 0x80=Reliable, 0x10=ACK)
 * - Bytes 1-2: Packet length (big-endian)
 * - Byte 3: Unknown/padding
 * - Bytes 4-5: Session ID (big-endian)
 * - Bytes 6-7: Acknowledged packet ID (big-endian)
 * - Bytes 8-9: Remote packet ID (big-endian)
 * - Bytes 10-11: Unknown/padding
 * 
 * Handles connection establishment, ACK requirements, and command processing
 */
bool ATEM::parsePacket(uint8_t* buffer, int length) {
  // Validate minimum packet size
  if (length < HEADER_SIZE) {
    debugPrint("Packet too short for header");
    return false;
  }
  
  // Parse header using EXACT Sofie ATEM Connection format
  uint16_t flagsAndLength = (buffer[0] << 8) | buffer[1];
  uint8_t flags = (buffer[0] >> 3);  // Flags are in high 5 bits of first byte
  uint16_t packet_length = flagsAndLength & 0x07FF;  // Length is low 11 bits
  uint16_t session_id = (buffer[2] << 8) | buffer[3];
  uint16_t acked_packet_id = (buffer[6] << 8) | buffer[7];  // bytes 6-7
  uint16_t remote_packet_id = (buffer[10] << 8) | buffer[11]; // CORRECTED: bytes 10-11 contain the ATEM's packet ID
  
  // Enhanced packet analysis (VERBOSE level only)
  if (_log_level >= ATEM_LOG_VERBOSE) {
    Serial.println("[ATEM VERBOSE] === PACKET ANALYSIS (SOFIE FORMAT) ===");
    Serial.print("[ATEM VERBOSE] Raw first bytes: ");
    for (int i = 0; i < 12 && i < length; i++) {
      if (buffer[i] < 16) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    Serial.print("[ATEM VERBOSE] Flags: 0x");
    if (flags < 16) Serial.print("0");
    Serial.print(flags, HEX);
    Serial.print(" (");
    if (flags & 0x01) Serial.print("AckRequest ");
    if (flags & 0x02) Serial.print("NewSessionId ");
    if (flags & 0x04) Serial.print("IsRetransmit ");
    if (flags & 0x08) Serial.print("RetransmitRequest ");
    if (flags & 0x10) Serial.print("AckReply ");
    if (flags == 0x00) Serial.print("NONE");
    Serial.println(")");
    Serial.printf("[ATEM VERBOSE] Length: %d (actual: %d)\n", packet_length, length);
    Serial.printf("[ATEM VERBOSE] Session ID: 0x%04X\n", session_id);
    Serial.printf("[ATEM VERBOSE] Acked Packet ID: %d\n", acked_packet_id);
    Serial.printf("[ATEM VERBOSE] Remote Packet ID: %d\n", remote_packet_id);
    Serial.println("[ATEM VERBOSE] =======================================");
  }
  
  // Validate packet length
  if (packet_length != length) {
    logPrintf(ATEM_LOG_WARN, "Packet length mismatch: header says %d, actually received %d", packet_length, length);
  }
  
  // Handle connection response - look for NewSessionId flag (0x02)
  if (_connection_state == ATEM_CONNECTING) {
    logDebug("*** ANALYZING PACKET DURING HELLO HANDSHAKE ***");
    
    if (flags & 0x02) { // NewSessionId flag from Sofie implementation
      logInfo("*** HELLO_RESPONSE WITH NewSessionId FLAG DETECTED! ***");
      
      // Use the session ID from the response header (exactly like Sofie does)
      _session_id = session_id;
      logPrintf(ATEM_LOG_INFO, "ATEM assigned session ID: 0x%04X", _session_id);
      
      _connection_state = ATEM_CONNECTED;
      _last_heartbeat = millis();  // Initialize heartbeat timer when connection established
      
      // Send ACK for the hello response (like Sofie does)
      if (remote_packet_id > 0) {
        logPrintf(ATEM_LOG_DEBUG, "Sending ACK for packet ID: %d", remote_packet_id);
        sendAck(remote_packet_id);
        _remote_packet_id = remote_packet_id;  // Update our tracking
      }
      
      // CRITICAL: Start packet storage immediately after connection established
      logInfo("*** INITIALIZING PACKET STORAGE FOR RETRANSMISSION ***");
      logInfo("All outgoing packets will now be stored for potential retransmission");
      
      return true;
    } else {
      logPrintf(ATEM_LOG_DEBUG, "*** NOT A NewSessionId RESPONSE - flags: 0x%02X ***", flags);
    }
  }
  
  // Handle session ID changes (ATEM may reassign during operation)
  if (_connection_state == ATEM_CONNECTED && session_id != _session_id) {
    logPrintf(ATEM_LOG_INFO, "Session ID changed from 0x%04X to 0x%04X - updating", _session_id, session_id);
    _session_id = session_id;
  }
  
  // Update packet tracking
  if (remote_packet_id > _remote_packet_id) {
    _remote_packet_id = remote_packet_id;
  }
  
  // Handle RetransmitRequest packets specifically
  if (flags & 0x08) { // RetransmitRequest flag
    // Extract the fromPacketId from bytes 6-7 (like Sofie library does)
    uint16_t from_packet_id = (buffer[6] << 8) | buffer[7];
    
    logPrintf(ATEM_LOG_INFO, "[T+%dms] ATEM requesting retransmit FROM packet ID: %d (sequence %d, like Sofie: from this packet onwards)",
             millis(), from_packet_id, remote_packet_id);
    
    // Handle the retransmit request by resending from the requested packet onwards
    logDebug("IMPLEMENTING RETRANSMISSION FROM PACKET ONWARDS...");
    handleRetransmitRequest(from_packet_id, remote_packet_id);
    return true; // Don't process further
  }
  
  // Send ACK for ALL packets with data (like Sofie does), not just AckRequest packets
  // This is critical - ATEM expects ACK for every data packet to continue sending
  if (length > HEADER_SIZE || (flags & 0x01)) {  // Has data OR explicit AckRequest
    logPrintf(ATEM_LOG_DEBUG, "[T+%dms] Sending ACK for packet ID: %d (packet has %d bytes of data)",
             millis(), remote_packet_id, length - HEADER_SIZE);
    sendAck(remote_packet_id);
  }
  
  // Process commands if present
  if (length > HEADER_SIZE) {
    processInitialPayload(buffer + HEADER_SIZE, length - HEADER_SIZE);
  }
  
  return true;
}

/**
 * @brief Process command data payload from ATEM packet
 * @param data Pointer to command data (after 12-byte packet header)
 * @param length Length of command data
 * 
 * ATEM commands have structure:
 * - Bytes 0-1: Command length (big-endian, includes this 8-byte header)
 * - Bytes 2-3: Unknown/reserved
 * - Bytes 4-7: 4-character command name (e.g., "PrgI", "PrvI")
 * - Bytes 8+: Command-specific data
 * 
 * Processes multiple commands that may be packed in single packet
 * Currently handles: PrgI (Program Input), PrvI (Preview Input)
 */
void ATEM::processInitialPayload(uint8_t* data, int length) {
  int offset = 0;
  
  while (offset + 8 <= length) {
    // Bounds check before reading command length
    if (offset + 2 > length) {
      debugPrint("Insufficient data for command length");
      break;
    }
    
    uint16_t cmd_length = (data[offset] << 8) | data[offset + 1];
    
    // Validate command length
    if (cmd_length < 8) {
      debugPrint("Command length too short");
      break;
    }
    
    if (offset + cmd_length > length) {
      debugPrint("Command extends beyond packet");
      break;
    }
    
    // Bounds check before reading command name
    if (offset + 8 > length) {
      debugPrint("Insufficient data for command name");
      break;
    }
    
    // Extract command name
    char cmd_name[5];
    memcpy(cmd_name, data + offset + 4, 4);
    cmd_name[4] = '\0';
    
    logPrintf(ATEM_LOG_VERBOSE, "Command: %s (%d bytes)", cmd_name, cmd_length);
    
    // Process specific commands
    if (strcmp(cmd_name, "PrgI") == 0) {
      processProgramInput(data + offset + 8, cmd_length - 8);
    }
    else if (strcmp(cmd_name, "PrvI") == 0) {
      processPreviewInput(data + offset + 8, cmd_length - 8);
    }
    
    offset += cmd_length;
  }
}

/**
 * @brief Process Program Input (PrgI) command from ATEM
 * @param data Pointer to PrgI command data (after 8-byte command header)
 * @param length Length of command data
 * 
 * PrgI command data structure:
 * - Bytes 0-1: ME (Mix Effects) index
 * - Bytes 2-3: Input source ID (big-endian)
 * 
 * Updates internal program input state, sets dirty flag, and triggers
 * onProgramInputChanged() callback if value actually changed
 */
void ATEM::processProgramInput(uint8_t* data, int length) {
  if (length < 4) {
    debugPrint("PrgI command data too short");
    return;
  }
  
  uint16_t input = (data[2] << 8) | data[3];
  
    if (input != _state.program_input) {
    _state.program_input = input;
    _state_dirty = true;
    
    logPrintf(ATEM_LOG_INFO, "Program input changed to: %d", input);
    
    onProgramInputChanged(input);
  }
}

/**
 * @brief Process Preview Input (PrvI) command from ATEM
 * @param data Pointer to PrvI command data (after 8-byte command header)
 * @param length Length of command data
 * 
 * PrvI command data structure:
 * - Bytes 0-1: ME (Mix Effects) index  
 * - Bytes 2-3: Input source ID (big-endian)
 * 
 * Updates internal preview input state, sets dirty flag, and triggers
 * onPreviewInputChanged() callback if value actually changed
 */
void ATEM::processPreviewInput(uint8_t* data, int length) {
  if (length < 4) {
    debugPrint("PrvI command data too short");
    return;
  }
  
  uint16_t input = (data[2] << 8) | data[3];
  
  if (input != _state.preview_input) {
    _state.preview_input = input;
    _state_dirty = true;
    
    logPrintf(ATEM_LOG_INFO, "Preview input changed to: %d", input);
    
    onPreviewInputChanged(input);
  }
}

/**
 * @brief Send heartbeat packet to maintain connection with ATEM
 * 
 * Heartbeat packet structure (12 bytes):
 * - Byte 0: 0x80 (Reliable packet flag)
 * - Bytes 1-2: 0x0C, 0x00 (Length = 12 bytes)
 * - Byte 3: 0x00 (Unknown/padding)
 * - Bytes 4-5: Session ID (big-endian)
 * - Bytes 6-7: Last received packet ID from ATEM (big-endian)
 * - Bytes 8-9: Our local packet ID (big-endian)
 * - Bytes 10-11: 0x00 (Unknown/padding)
 * 
 * Must be sent every HEARTBEAT_INTERVAL ms to prevent timeout
 * Increments local packet ID after sending
 */
void ATEM::sendHeartbeat() {
  // Build heartbeat packet like Sofie ATEM Connection library
  uint16_t opcode = (FLAG_ACK_REQUEST << 11);  // AckRequest flag for heartbeat
  uint16_t length = 12;  // Always 12 bytes for heartbeat
  
  uint8_t packet[HEADER_SIZE] = {0};
  
  // Opcode and length in first 2 bytes (big-endian)
  packet[0] = (opcode | length) >> 8;
  packet[1] = (opcode | length) & 0xFF;
  
  // Session ID in bytes 2-3 (big-endian)
  packet[2] = (_session_id >> 8) & 0xFF;
  packet[3] = _session_id & 0xFF;
  
  // Local packet ID in bytes 10-11 (big-endian)
  packet[10] = (_local_packet_id >> 8) & 0xFF;
  packet[11] = _local_packet_id & 0xFF;
  
  // Store packet for potential retransmission
  storePacketForRetransmission(_local_packet_id, packet, HEADER_SIZE);
  
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  _udp.write(packet, HEADER_SIZE);
  _udp.endPacket();
  
  // Log in Sofie format for comparison
  printSofieFormat("SEND", packet, HEADER_SIZE);
  
  _local_packet_id++;
  
  logPrintf(ATEM_LOG_DEBUG, "[T+%dms] Heartbeat sent", millis());
}

/**
 * @brief Send acknowledgment packet for received reliable data
 * @param packet_id ID of the packet being acknowledged
 * 
 * ACK packet structure (12 bytes):
 * - Byte 0: 0x10 (ACK flag)
 * - Bytes 1-2: 0x0C, 0x00 (Length = 12 bytes)
 * - Byte 3: 0x00 (Unknown/padding)
 * - Bytes 4-5: Session ID (big-endian)
 * - Bytes 6-7: Packet ID being acknowledged (big-endian)
 * - Bytes 8-9: 0x00 (Local packet ID not used for ACK)
 * - Bytes 10-11: 0x00 (Unknown/padding)
 * 
 * Required for reliable packet delivery in ATEM protocol
 */
void ATEM::sendAck(uint16_t packet_id) {
  // Build ACK packet exactly like Sofie ATEM Connection library
  // Format: 800c [session_id] 0000 [acked_packet_id] 0000
  uint8_t packet[HEADER_SIZE] = {0};
  
  // ACK flag (0x10) shifted to high 5 bits, with 12-byte length
  packet[0] = 0x80;  // High 5 bits = 0x10 (AckReply flag), length = 0x0C (12 bytes)
  packet[1] = 0x0C;
  
  // Session ID in bytes 2-3 (big-endian)
  packet[2] = (_session_id >> 8) & 0xFF;
  packet[3] = _session_id & 0xFF;
  
  // CRITICAL FIX: Acked packet ID goes in bytes 4-5 (big-endian), not 6-7!
  packet[4] = (packet_id >> 8) & 0xFF;
  packet[5] = packet_id & 0xFF;
  
  // Bytes 6-7 remain zero for ACK packets  
  packet[6] = 0x00;
  packet[7] = 0x00;
  
  // Bytes 8-11 remain zero for ACK packets
  
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  _udp.write(packet, HEADER_SIZE);
  _udp.endPacket();
  
  // Log in Sofie format for comparison
  printSofieFormat("SEND", packet, HEADER_SIZE);
  
  logPrintf(ATEM_LOG_VERBOSE, "[T+%dms] ACK sent for packet %d with session ID 0x%04X - HEX: ",
           millis(), packet_id, _session_id);
  if (_log_level >= ATEM_LOG_VERBOSE) {
    for (int i = 0; i < HEADER_SIZE; i++) {
      if (packet[i] < 16) Serial.print("0");
      Serial.print(packet[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

// State access functions

/**
 * @brief Get complete current ATEM state
 * @return ATEMState struct containing program/preview inputs and transition status
 * Returns a copy of the internal state structure
 */
ATEMState ATEM::getState() {
  return _state;
}

/**
 * @brief Get current program input number
 * @return uint16_t program input ID (1=CAM1, 2=CAM2, 0=BLACK, etc.)
 */
uint16_t ATEM::getProgramInput() {
  return _state.program_input;
}

/**
 * @brief Get current preview input number
 * @return uint16_t preview input ID (1=CAM1, 2=CAM2, 0=BLACK, etc.)
 */
uint16_t ATEM::getPreviewInput() {
  return _state.preview_input;
}

// Control function stubs (to be implemented in future versions)

// ===========================================
// CONTROL FUNCTIONS - PHASE 2 IMPLEMENTATION  
// ===========================================

// ✅ BASIC SWITCHING (1/4 COMPLETE)

/**
 * @brief Change program input on ATEM switcher ✅ WORKING!
 * @param input Input ID to switch to program (1=CAM1, 2=CAM2, etc.)
 * 
 * Sends CPgI command to change program input
 * Uses same packet structure as CPvI but with "CPgI" command name
 */
void ATEM::changeProgramInput(uint16_t input) {
  if (_connection_state != ATEM_CONNECTED) {
    logPrintf(ATEM_LOG_WARN, "Cannot change program input: ATEM not connected");
    return;
  }
  
  // ATEM CPgI (Change Program Input) command structure:
  // Header: 12 bytes  
  // Command: 4 bytes length + 4 bytes "CPgI" + 4 bytes data = 12 bytes
  // Total: 24 bytes (12 header + 12 command section)
  
  const int command_length = 24;
  
  uint8_t packet[command_length] = {0};
  
  // Build packet header (identical to CPvI)
  uint16_t opcode = (FLAG_ACK_REQUEST << 11);  // AckRequest flag
  uint16_t length = command_length;
  
  // Opcode and length in first 2 bytes (big-endian)
  packet[0] = (opcode | length) >> 8;
  packet[1] = (opcode | length) & 0xFF;
  
  // Session ID in bytes 2-3 (big-endian)
  packet[2] = (_session_id >> 8) & 0xFF;
  packet[3] = _session_id & 0xFF;
  
  // Local packet ID in bytes 10-11 (big-endian)
  packet[10] = (_local_packet_id >> 8) & 0xFF;
  packet[11] = _local_packet_id & 0xFF;
  
  // Command data starts at byte 12
  // Command length (12 bytes total for command)
  packet[12] = 0x00;
  packet[13] = 0x0C;  // 12 bytes total for command
  packet[14] = 0x00;
  packet[15] = 0x00;
  
  // Command name "CPgI" (Change Program Input) - ONLY DIFFERENCE FROM CPvI!
  packet[16] = 'C';
  packet[17] = 'P';
  packet[18] = 'g';  // 'g' for program vs 'v' for preview
  packet[19] = 'I';
  
  // Input number (4 bytes, big-endian) - Same as CPvI format
  packet[20] = 0x00;
  packet[21] = 0x00;
  packet[22] = (input >> 8) & 0xFF;  // Upper byte of input value
  packet[23] = input & 0xFF;         // Lower byte of input value
  
  // Store packet for potential retransmission
  storePacketForRetransmission(_local_packet_id, packet, command_length);
  
  // Send the packet
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  _udp.write(packet, command_length);
  bool success = _udp.endPacket();
  
  // Log the command
  printSofieFormat("SEND", packet, command_length);
  
  // Force debug output (temporary)
  Serial.print("ℹ️  ESP32 CPgI packet: ");
  for (int i = 0; i < command_length; i++) {
    if (packet[i] < 0x10) Serial.print("0");
    Serial.print(packet[i], HEX);
  }
  Serial.println();
  
  if (success) {
    logPrintf(ATEM_LOG_INFO, "Sent CPgI command: program input %d", input);
    _local_packet_id++;
  } else {
    logPrintf(ATEM_LOG_ERROR, "Failed to send CPgI command");
  }
}

/**
 * @brief Change preview input on ATEM switcher ✅ WORKING!
 * 
 * Sends CPvI command to change preview input
 * @param input Input ID to switch to preview (1=CAM1, 2=CAM2, etc.)
 */
void ATEM::changePreviewInput(uint16_t input) {
  if (_connection_state != ATEM_CONNECTED) {
    logPrintf(ATEM_LOG_WARN, "Cannot change preview input: ATEM not connected");
    return;
  }
  
  // ATEM CPvI (Change Preview Input) command structure:
  // Header: 12 bytes
  // Command: 4 bytes length + 4 bytes "CPvI" + 4 bytes data = 12 bytes
  // Total: 24 bytes (12 header + 12 command section)
  
  const int command_length = 24;
  
  uint8_t packet[command_length] = {0};
  
  // Build packet header (similar to heartbeat but with data)
  uint16_t opcode = (FLAG_ACK_REQUEST << 11);  // AckRequest flag
  uint16_t length = command_length;
  
  // Opcode and length in first 2 bytes (big-endian)
  packet[0] = (opcode | length) >> 8;
  packet[1] = (opcode | length) & 0xFF;
  
  // Session ID in bytes 2-3 (big-endian)
  packet[2] = (_session_id >> 8) & 0xFF;
  packet[3] = _session_id & 0xFF;
  
  // Local packet ID in bytes 10-11 (big-endian)
  packet[10] = (_local_packet_id >> 8) & 0xFF;
  packet[11] = _local_packet_id & 0xFF;
  
  // Command data starts at byte 12
  // Command length (4 bytes for "CPvI" + 8 bytes data = 12 bytes total) - FIXED!
  packet[12] = 0x00;
  packet[13] = 0x0C;  // 12 bytes total for command (was 0x08 = 8 bytes)
  packet[14] = 0x00;
  packet[15] = 0x00;
  
  // Command name "CPvI" (Change Preview Input)
  packet[16] = 'C';
  packet[17] = 'P';
  packet[18] = 'v';
  packet[19] = 'I';
  
  // Input number (4 bytes, big-endian) - Sofie format uses 4 bytes, not 2!
  // No conversion needed - use input as-is (1-4) like Sofie does
  packet[20] = 0x00;
  packet[21] = 0x00;
  packet[22] = (input >> 8) & 0xFF;  // Upper byte of input value
  packet[23] = input & 0xFF;         // Lower byte of input value
  
  // Store packet for potential retransmission
  storePacketForRetransmission(_local_packet_id, packet, command_length);
  
  // Send the packet
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  _udp.write(packet, command_length);
  bool success = _udp.endPacket();
  
  // Log the command
  printSofieFormat("SEND", packet, command_length);
  
  // Force debug output regardless of log level (temporary)
  Serial.print("ℹ️  ESP32 CPvI packet: ");
  for (int i = 0; i < command_length; i++) {
    if (packet[i] < 0x10) Serial.print("0");
    Serial.print(packet[i], HEX);
  }
  Serial.println();
  
  if (success) {
    logPrintf(ATEM_LOG_INFO, "Sent CPvI command: preview input %d", input);
    _local_packet_id++;
  } else {
    logPrintf(ATEM_LOG_ERROR, "Failed to send CPvI command");
  }
}

/**
 * @brief Perform CUT transition (immediate switch) ✅ IMPLEMENTED!
 * 
 * Sends DCut command to perform immediate transition from preview to program
 * This instantly cuts from the current preview input to program output
 */
void ATEM::cut() {
  if (_connection_state != ATEM_CONNECTED) {
    logPrintf(ATEM_LOG_WARN, "Cannot perform CUT: ATEM not connected");
    return;
  }
  
  // ATEM DCut (Direct Cut) command structure:
  // Header: 12 bytes
  // Command: 4 bytes length + 4 bytes "DCut" + 4 bytes data = 12 bytes
  // Total: 24 bytes (12 header + 12 command section)
  
  const int command_length = 24;
  
  uint8_t packet[command_length] = {0};
  
  // Build packet header (same pattern as CPvI/CPgI)
  uint16_t opcode = (FLAG_ACK_REQUEST << 11);  // AckRequest flag
  uint16_t length = command_length;
  
  // Opcode and length in first 2 bytes (big-endian)
  packet[0] = (opcode | length) >> 8;
  packet[1] = (opcode | length) & 0xFF;
  
  // Session ID in bytes 2-3 (big-endian)
  packet[2] = (_session_id >> 8) & 0xFF;
  packet[3] = _session_id & 0xFF;
  
  // Local packet ID in bytes 10-11 (big-endian)
  packet[10] = (_local_packet_id >> 8) & 0xFF;
  packet[11] = _local_packet_id & 0xFF;
  
  // Command data starts at byte 12
  // Command length (12 bytes total for command)
  packet[12] = 0x00;
  packet[13] = 0x0C;  // 12 bytes total for command
  packet[14] = 0x00;
  packet[15] = 0x00;
  
  // Command name "DCut" (Direct Cut)
  packet[16] = 'D';
  packet[17] = 'C';
  packet[18] = 'u';
  packet[19] = 't';
  
  // ME index (Mix Effect index) - 4 bytes, big-endian
  // For ATEM Mini Pro, ME index is 0 (first/only Mix Effect)
  packet[20] = 0x00;
  packet[21] = 0x00;
  packet[22] = 0x00;
  packet[23] = 0x00;  // ME 0 (first Mix Effect)
  
  // Store packet for potential retransmission
  storePacketForRetransmission(_local_packet_id, packet, command_length);
  
  // Send the packet
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  _udp.write(packet, command_length);
  bool success = _udp.endPacket();
  
  // Log the command
  printSofieFormat("SEND", packet, command_length);
  
  if (success) {
    logPrintf(ATEM_LOG_INFO, "Sent DCut command: performed CUT transition");
    _local_packet_id++;
  } else {
    logPrintf(ATEM_LOG_ERROR, "Failed to send DCut command");
  }
}

/**
 * @brief Perform AUTO transition (fade/wipe effect) ✅ IMPLEMENTED!
 * 
 * Sends DAut command to perform automated transition from preview to program
 * Uses the currently configured transition type (fade, wipe, etc.) and duration
 */
void ATEM::autoTransition() {
  if (_connection_state != ATEM_CONNECTED) {
    logPrintf(ATEM_LOG_WARN, "Cannot perform AUTO transition: ATEM not connected");
    return;
  }
  
  // ATEM DAut (Direct Auto) command structure:
  // Header: 12 bytes
  // Command: 4 bytes length + 4 bytes "DAut" + 4 bytes data = 12 bytes
  // Total: 24 bytes (12 header + 12 command section)
  
  const int command_length = 24;
  
  uint8_t packet[command_length] = {0};
  
  // Build packet header (same pattern as DCut)
  uint16_t opcode = (FLAG_ACK_REQUEST << 11);  // AckRequest flag
  uint16_t length = command_length;
  
  // Opcode and length in first 2 bytes (big-endian)
  packet[0] = (opcode | length) >> 8;
  packet[1] = (opcode | length) & 0xFF;
  
  // Session ID in bytes 2-3 (big-endian)
  packet[2] = (_session_id >> 8) & 0xFF;
  packet[3] = _session_id & 0xFF;
  
  // Local packet ID in bytes 10-11 (big-endian)
  packet[10] = (_local_packet_id >> 8) & 0xFF;
  packet[11] = _local_packet_id & 0xFF;
  
  // Command data starts at byte 12
  // Command length (12 bytes total for command)
  packet[12] = 0x00;
  packet[13] = 0x0C;  // 12 bytes total for command
  packet[14] = 0x00;
  packet[15] = 0x00;
  
  // Command name "DAut" (Direct Auto)
  packet[16] = 'D';
  packet[17] = 'A';
  packet[18] = 'u';
  packet[19] = 't';
  
  // ME index (Mix Effect index) - 4 bytes, big-endian
  // For ATEM Mini Pro, ME index is 0 (first/only Mix Effect)
  packet[20] = 0x00;
  packet[21] = 0x00;
  packet[22] = 0x00;
  packet[23] = 0x00;  // ME 0 (first Mix Effect)
  
  // Store packet for potential retransmission
  storePacketForRetransmission(_local_packet_id, packet, command_length);
  
  // Send the packet
  _udp.beginPacket(_switcher_ip, ATEM_PORT);
  _udp.write(packet, command_length);
  bool success = _udp.endPacket();
  
  // Log the command
  printSofieFormat("SEND", packet, command_length);
  
  if (success) {
    logPrintf(ATEM_LOG_INFO, "Sent DAut command: performed AUTO transition");
    _local_packet_id++;
  } else {
    logPrintf(ATEM_LOG_ERROR, "Failed to send DAut command");
  }
}

// 🔄 ADVANCED SWITCHING

/**
 * @brief Fade to black or fade from black
 * TODO: Send FtbS command
 */
void ATEM::fadeToBlack(uint8_t me) {
  logPrintf(ATEM_LOG_INFO, "fadeToBlack(me=%d) - TODO: implement FtbS command", me);
  debugPrint("fadeToBlack() - not implemented yet");
}

/**
 * @brief Set fade to black rate
 * TODO: Send FtbP command  
 */
void ATEM::setFadeToBlackRate(uint16_t rate, uint8_t me) {
  logPrintf(ATEM_LOG_INFO, "setFadeToBlackRate(rate=%d, me=%d) - TODO: implement FtbP command", rate, me);
  debugPrint("setFadeToBlackRate() - not implemented yet");
}

/**
 * @brief Set transition position manually
 * TODO: Send CTPs command
 */
void ATEM::setTransitionPosition(uint16_t position, uint8_t me) {
  logPrintf(ATEM_LOG_INFO, "setTransitionPosition(pos=%d, me=%d) - TODO: implement CTPs command", position, me);
  debugPrint("setTransitionPosition() - not implemented yet");
}

/**
 * @brief Enable/disable transition preview
 * TODO: Send CTPr command
 */
void ATEM::previewTransition(bool on, uint8_t me) {
  logPrintf(ATEM_LOG_INFO, "previewTransition(on=%s, me=%d) - TODO: implement CTPr command", on ? "true" : "false", me);
  debugPrint("previewTransition() - not implemented yet");
}

// 🔄 AUX & DOWNSTREAM KEYS

/**
 * @brief Set AUX output source
 * TODO: Send CAuS command
 */
void ATEM::setAuxSource(uint16_t source, uint8_t bus) {
  logPrintf(ATEM_LOG_INFO, "setAuxSource(source=%d, bus=%d) - TODO: implement CAuS command", source, bus);
  debugPrint("setAuxSource() - not implemented yet");
}

/**
 * @brief Set downstream key on air state
 * TODO: Send CDsL command
 */
void ATEM::setDownstreamKeyOnAir(bool onAir, uint8_t key) {
  logPrintf(ATEM_LOG_INFO, "setDownstreamKeyOnAir(onAir=%s, key=%d) - TODO: implement CDsL command", onAir ? "true" : "false", key);
  debugPrint("setDownstreamKeyOnAir() - not implemented yet");
}

/**
 * @brief Auto transition downstream key
 * TODO: Send DDsA command
 */
void ATEM::autoDownstreamKey(uint8_t key, bool isTowardsOnAir) {
  logPrintf(ATEM_LOG_INFO, "autoDownstreamKey(key=%d, towards=%s) - TODO: implement DDsA command", key, isTowardsOnAir ? "onAir" : "offAir");
  debugPrint("autoDownstreamKey() - not implemented yet");
}

// 🔄 UPSTREAM KEYS

/**
 * @brief Set upstream keyer on air state
 * TODO: Send CKOn command
 */
void ATEM::setUpstreamKeyerOnAir(bool onAir, uint8_t me, uint8_t keyer) {
  logPrintf(ATEM_LOG_INFO, "setUpstreamKeyerOnAir(onAir=%s, me=%d, keyer=%d) - TODO: implement CKOn command", onAir ? "true" : "false", me, keyer);
  debugPrint("setUpstreamKeyerOnAir() - not implemented yet");
}

/**
 * @brief Set upstream keyer cut source
 * TODO: Send CKeC command
 */
void ATEM::setUpstreamKeyerCutSource(uint16_t cutSource, uint8_t me, uint8_t keyer) {
  logPrintf(ATEM_LOG_INFO, "setUpstreamKeyerCutSource(cutSource=%d, me=%d, keyer=%d) - TODO: implement CKeC command", cutSource, me, keyer);
  debugPrint("setUpstreamKeyerCutSource() - not implemented yet");
}

/**
 * @brief Set upstream keyer fill source
 * TODO: Send CKeF command
 */
void ATEM::setUpstreamKeyerFillSource(uint16_t fillSource, uint8_t me, uint8_t keyer) {
  logPrintf(ATEM_LOG_INFO, "setUpstreamKeyerFillSource(fillSource=%d, me=%d, keyer=%d) - TODO: implement CKeF command", fillSource, me, keyer);
  debugPrint("setUpstreamKeyerFillSource() - not implemented yet");
}

// 🔄 MEDIA & SETTINGS

/**
 * @brief Set color generator color
 * TODO: Send CClV command
 */
void ATEM::setColorGeneratorColour(float hue, float saturation, float lightness, uint8_t index) {
  logPrintf(ATEM_LOG_INFO, "setColorGeneratorColour(h=%.1f, s=%.2f, l=%.2f, idx=%d) - TODO: implement CClV command", hue, saturation, lightness, index);
  debugPrint("setColorGeneratorColour() - not implemented yet");
}

/**
 * @brief Set media player source
 * TODO: Send MPCS command
 */
void ATEM::setMediaPlayerSource(uint8_t sourceType, uint8_t sourceIndex, uint8_t player) {
  logPrintf(ATEM_LOG_INFO, "setMediaPlayerSource(type=%d, idx=%d, player=%d) - TODO: implement MPCS command", sourceType, sourceIndex, player);
  debugPrint("setMediaPlayerSource() - not implemented yet");
}

/**
 * @brief Set multiviewer window source
 * TODO: Send CMvI command
 */
void ATEM::setMultiViewerWindowSource(uint16_t source, uint8_t mv, uint8_t window) {
  logPrintf(ATEM_LOG_INFO, "setMultiViewerWindowSource(source=%d, mv=%d, window=%d) - TODO: implement CMvI command", source, mv, window);
  debugPrint("setMultiViewerWindowSource() - not implemented yet");
}

// 🔄 AUDIO (BASIC)

/**
 * @brief Set classic audio mixer input gain
 * TODO: Send CAIP command
 */
void ATEM::setClassicAudioMixerInputGain(uint8_t input, float gain) {
  logPrintf(ATEM_LOG_INFO, "setClassicAudioMixerInputGain(input=%d, gain=%.1f) - TODO: implement CAIP command", input, gain);
  debugPrint("setClassicAudioMixerInputGain() - not implemented yet");
}

/**
 * @brief Set classic audio mixer master gain
 * TODO: Send CAMP command
 */
void ATEM::setClassicAudioMixerMasterGain(float gain) {
  logPrintf(ATEM_LOG_INFO, "setClassicAudioMixerMasterGain(gain=%.1f) - TODO: implement CAMP command", gain);
  debugPrint("setClassicAudioMixerMasterGain() - not implemented yet");
}

// Event callbacks (default implementations - override in derived class)

/**
 * @brief Default connection state change handler
 * @param state New ATEMConnectionState value
 * Prints state change to Serial. Override this method in your derived class
 * to implement custom connection state handling (LEDs, displays, etc.)
 */
void ATEM::onConnectionStateChanged(ATEMConnectionState state) {
  Serial.print("Connection state changed: ");
  switch (state) {
    case ATEM_DISCONNECTED: Serial.println("DISCONNECTED"); break;
    case ATEM_CONNECTING: Serial.println("CONNECTING"); break;
    case ATEM_CONNECTED: Serial.println("CONNECTED"); break;
    case ATEM_ERROR: Serial.println("ERROR"); break;
  }
}

/**
 * @brief Default program input change handler
 * @param input New program input ID
 * Override this method in your derived class to implement custom handling
 * (update displays, trigger tally lights, log changes, etc.)
 */
void ATEM::onProgramInputChanged(uint16_t input) {
  // Override in your implementation
}

/**
 * @brief Default preview input change handler
 * @param input New preview input ID
 * Override this method in your derived class to implement custom handling
 * (update displays, trigger tally lights, log changes, etc.)
 */
void ATEM::onPreviewInputChanged(uint16_t input) {
  // Override in your implementation
}

/**
 * @brief Default general state change handler
 * Called after any state variable is updated (program, preview, transition)
 * Override this method in your derived class for general state change handling
 * (update displays, save state, trigger actions, etc.)
 */
void ATEM::onStateChanged() {
  // Override in your implementation
}

// Debug and utility functions

/**
 * @brief Print debug message with [ATEM] prefix
 * @param message Debug message string to print
 * Only prints if debug output is enabled via enableDebug(true)
 * Adds consistent "[ATEM] " prefix to all debug messages
 */
void ATEM::debugPrint(const char* message) {
  logDebug(message);  // Convert to new logging system
}

/**
 * @brief Print hexadecimal dump of data for debugging
 * @param data Pointer to data buffer to dump
 * @param length Number of bytes to dump
 * 
 * Prints hex bytes with bounds checking to prevent buffer overflows
 * Format: "HEX: 01 23 45 67 89 AB CD EF ..." with 16 bytes per line
 * Only prints if debug output is enabled
 */
void ATEM::debugPrintHex(uint8_t* data, int length) {
  if (_log_level < ATEM_LOG_VERBOSE) return;  // Only show at VERBOSE level
  
  // Bounds checking to prevent buffer overflow
  int safe_length = (length > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : length;
  
  Serial.print("[ATEM VERBOSE] HEX: ");
  for (int i = 0; i < safe_length; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
    Serial.print(" ");
    if ((i + 1) % 16 == 0) Serial.println();
  }
  Serial.println();
}

/**
 * @brief Print detailed connection information to Serial
 * Displays formatted connection status including:
 * - Current connection state
 * - ATEM switcher IP address
 * - Session ID (hex format)
 * - Local and remote packet ID counters
 * - Current program and preview input states
 * Useful for debugging connection and protocol issues
 */
void ATEM::printConnectionInfo() {
  Serial.println("=== ATEM Connection Info ===");
  Serial.print("State: ");
  switch (_connection_state) {
    case ATEM_DISCONNECTED: Serial.println("DISCONNECTED"); break;
    case ATEM_CONNECTING: Serial.println("CONNECTING"); break;
    case ATEM_CONNECTED: Serial.println("CONNECTED"); break;
    case ATEM_ERROR: Serial.println("ERROR"); break;
  }
  Serial.print("Switcher IP: ");
  Serial.println(_switcher_ip);
  Serial.print("Session ID: 0x");
  Serial.println(_session_id, HEX);
  Serial.print("Local Packet ID: ");
  Serial.println(_local_packet_id);
  Serial.print("Remote Packet ID: ");
  Serial.println(_remote_packet_id);
  Serial.print("Program Input: ");
  Serial.println(_state.program_input);
  Serial.print("Preview Input: ");
  Serial.println(_state.preview_input);
  Serial.println("==========================");
}

/**
 * @brief Print library version information to Serial
 * Displays formatted version banner including:
 * - Library version number
 * - Build date and time
 * - Source library attribution
 * Called automatically during begin() after Serial is ready
 */
void ATEM::printVersionInfo() {
  Serial.println("================================");
  Serial.print("ATEM ESP32 Library ");
  Serial.println(ATEM_ESP32_VERSION);
  Serial.print("Build: ");
  Serial.println(ATEM_ESP32_BUILD_DATE);
  Serial.println("Based on Sofie ATEM Connection");
  Serial.println("*** VERSION VALIDATION ***");
  Serial.print("Expected: v1.7.0 (HELLO packet storage + retransmit)");
  if (strcmp(ATEM_ESP32_VERSION, "v1.7.0") == 0) {
    Serial.println(" ✓ CORRECT VERSION");
    Serial.println("*** Configuration: HELLO packet ID 768 stored for retransmission ***");
    Serial.println("*** Based on ESP32 analysis showing ATEM requests packet ID 768 ***");
  } else {
    Serial.println(" ✗ VERSION MISMATCH!");
    Serial.println("*** WARNING: Code may not have been saved properly! ***");
  }
  Serial.println("================================");
}

/**
 * @brief Store outgoing packet for potential retransmission
 * @param packet_id ID of the packet being sent
 * @param data Pointer to packet data
 * @param length Length of packet data
 * 
 * Stores packet in circular buffer for retransmission on request.
 * Uses circular buffer to maintain recent packets while avoiding memory issues.
 * Old packets are automatically overwritten when buffer is full.
 */
void ATEM::storePacketForRetransmission(uint16_t packet_id, uint8_t* data, int length) {
  if (length > MAX_PACKET_SIZE) {
    Serial.print("[ATEM] WARNING: Packet too large for storage: ");
    Serial.print(length);
    Serial.print(" > ");
    Serial.println(MAX_PACKET_SIZE);
    return;
  }
  
  // Use circular buffer
  StoredPacket* slot = &_sent_packets[_packet_buffer_index];
  
  slot->packet_id = packet_id;
  slot->length = length;
  slot->timestamp = millis();
  slot->in_use = true;
  memcpy(slot->data, data, length);
  
  logPrintf(ATEM_LOG_DEBUG, "Stored packet ID %d in slot %d (%d bytes)",
           packet_id, _packet_buffer_index, length);
  
  // Move to next slot (circular)
  _packet_buffer_index = (_packet_buffer_index + 1) % MAX_RETRANSMIT_PACKETS;
}

/**
 * @brief Handle retransmit request from ATEM
 * @param from_packet_id Starting packet ID to retransmit from
 * 
 * Based on Sofie atem-connection analysis: ATEM requests retransmit FROM a packet ID,
 * meaning resend that packet AND ALL SUBSEQUENT packets (like TCP retransmission).
 * This is different from requesting a single specific packet.
 */
void ATEM::handleRetransmitRequest(uint16_t from_packet_id, uint16_t sequence_to_ack) {
  Serial.print("[T+");
  Serial.print(millis());
  Serial.print("ms] Retransmitting FROM packet ");
  Serial.print(from_packet_id);
  Serial.println(" onwards (like Sofie library)");
  
  // Find the starting packet and retransmit from there onwards
  bool found_start = false;
  int retransmit_count = 0;
  
  // Search through our stored packets for the starting packet
  for (int i = 0; i < MAX_RETRANSMIT_PACKETS; i++) {
    StoredPacket* slot = &_sent_packets[i];
    
    if (!slot->in_use) continue;
    
    // Once we find the starting packet, retransmit it and all subsequent ones
    if (slot->packet_id == from_packet_id) {
      found_start = true;
    }
    
    if (found_start) {
      Serial.print("[T+");
      Serial.print(millis());
      Serial.print("ms] Retransmitting packet ID ");
      Serial.print(slot->packet_id);
      Serial.print(" from slot ");
      Serial.print(i);
      Serial.print(" (");
      Serial.print(slot->length);
      Serial.println(" bytes)");
      
      _udp.beginPacket(_switcher_ip, ATEM_PORT);
      _udp.write(slot->data, slot->length);
      _udp.endPacket();
      
      // Log in Sofie format for comparison
      printSofieFormat("SEND", slot->data, slot->length);
      
      retransmit_count++;
    }
  }
  
  if (retransmit_count > 0) {
    Serial.print("[T+");
    Serial.print(millis());
    Serial.print("ms] Retransmission complete - sent ");
    Serial.print(retransmit_count);
    Serial.print(" packets from ID ");
    Serial.print(from_packet_id);
    Serial.println(" onwards");
    
    // CRITICAL: Send ACK response after retransmission (like Sofie library does)
    Serial.print("[T+");
    Serial.print(millis());
    Serial.print("ms] Sending ACK response for sequence ");
    Serial.println(sequence_to_ack);
    sendAck(sequence_to_ack);
  } else {
    // Starting packet not found in storage - this is critical
    Serial.print("[T+");
    Serial.print(millis());
    Serial.print("ms] *** CRITICAL: RETRANSMIT FROM ");
    Serial.print(from_packet_id);
    Serial.println(" FAILED - starting packet not found! ***");
    
    Serial.println("[ATEM] Packet storage status:");
    for (int i = 0; i < MAX_RETRANSMIT_PACKETS; i++) {
      StoredPacket* slot = &_sent_packets[i];
      if (slot->in_use) {
        Serial.print("  Slot ");
        Serial.print(i);
        Serial.print(": Packet ID ");
        Serial.print(slot->packet_id);
        Serial.print(" (");
        Serial.print(slot->length);
        Serial.print(" bytes, age ");
        Serial.print(millis() - slot->timestamp);
        Serial.println("ms)");
      }
    }
    
    // CRITICAL FIX: Send ACK even when we can't retransmit to prevent retransmission storm
    Serial.print("[T+");
    Serial.print(millis());
    Serial.print("ms] Sending ACK for sequence ");
    Serial.print(sequence_to_ack);
    Serial.println(" (packet too old but acknowledging request)");
    sendAck(sequence_to_ack);
  }
}

/**
 * @brief Print packet in Sofie-compatible format for comparison
 * @param prefix Either "SEND" or "RECV" 
 * @param data Pointer to packet data
 * @param length Packet length
 */
void ATEM::printSofieFormat(const char* prefix, uint8_t* data, int length) {
  // Only print if log level allows DEBUG or higher
  if (_log_level >= ATEM_LOG_DEBUG) {
    Serial.print("ℹ️  Info: ");
    Serial.print(prefix);
    Serial.print(" ");
    
    // Print continuous hex string like Sofie does
    for (int i = 0; i < length && i < 64; i++) {  // Limit to prevent overflow
      if (data[i] < 0x10) Serial.print("0");
      Serial.print(data[i], HEX);
    }
    Serial.println();
  }
}

/**
 * @brief Logging System Implementation
 * Provides configurable verbosity levels for ESP32 output control
 */

void ATEM::logError(const char* message) {
  if (_log_level >= ATEM_LOG_ERROR) {
    Serial.print("[ATEM ERROR] ");
    Serial.println(message);
  }
}

void ATEM::logWarn(const char* message) {
  if (_log_level >= ATEM_LOG_WARN) {
    Serial.print("[ATEM WARN] ");
    Serial.println(message);
  }
}

void ATEM::logInfo(const char* message) {
  if (_log_level >= ATEM_LOG_INFO) {
    Serial.print("[ATEM] ");
    Serial.println(message);
  }
}

void ATEM::logDebug(const char* message) {
  if (_log_level >= ATEM_LOG_DEBUG) {
    Serial.print("[ATEM DEBUG] ");
    Serial.println(message);
  }
}

void ATEM::logVerbose(const char* message) {
  if (_log_level >= ATEM_LOG_VERBOSE) {
    Serial.print("[ATEM VERBOSE] ");
    Serial.println(message);
  }
}

void ATEM::logPrintf(ATEMLogLevel level, const char* format, ...) {
  if (_log_level >= level) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Add appropriate prefix based on level
    switch (level) {
      case ATEM_LOG_ERROR:   Serial.print("[ATEM ERROR] "); break;
      case ATEM_LOG_WARN:    Serial.print("[ATEM WARN] "); break;
      case ATEM_LOG_INFO:    Serial.print("[ATEM] "); break;
      case ATEM_LOG_DEBUG:   Serial.print("[ATEM DEBUG] "); break;
      case ATEM_LOG_VERBOSE: Serial.print("[ATEM VERBOSE] "); break;
      default: Serial.print("[ATEM] "); break;
    }
    Serial.println(buffer);
  }
}
