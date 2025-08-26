/*
 * config_sample.h
 * Sample configuration file for ATEM ESP32 Library
 * 
 * INSTRUCTIONS:
 * 1. Copy this file to config.h
 * 2. Update the WiFi credentials and ATEM IP address for your setup
 * 3. Customize other settings as needed
 * 
 * This configuration contains network settings, timing constants, and debug options
 * for the ATEM communication protocol.
 */

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief ATEM ESP32 Library Configuration File
 * 
 * This file contains all configuration constants and settings for the ATEM ESP32 library.
 * It includes network settings, hardware pin definitions, protocol constants, and debug options.
 * 
 * @date August 2024
 * @author GitHub Copilot (based on Sofie ATEM Connection)
 */

// Network Configuration
/**
 * @brief WiFi network credentials
 * Configure these values to match your local network setup
 */
#define WIFI_SSID "Your_WiFi_Network"     ///< WiFi network name (SSID)
#define WIFI_PASSWORD "Your_WiFi_Password" ///< WiFi network password

/**
 * @brief ATEM switcher network configuration
 * Configure the IP address of your ATEM Mini Pro device
 */
#define ATEM_IP "192.168.1.240"     ///< ATEM Mini Pro IP address (example)
#define ATEM_PORT 9910               ///< ATEM protocol UDP port (standard)

// Hardware Pin Definitions (for future expansion)
/**
 * @brief GPIO pin assignments for physical controls
 * These pins can be used for external buttons, LEDs, or other hardware interfaces
 * Currently reserved for future implementation
 */
#define PIN_CUT_BUTTON 2             ///< GPIO pin for CUT button input (with pullup)
#define PIN_AUTO_BUTTON 4            ///< GPIO pin for AUTO button input (with pullup)
#define PIN_PROGRAM_LED 16           ///< GPIO pin for PROGRAM tally LED output
#define PIN_PREVIEW_LED 17           ///< GPIO pin for PREVIEW tally LED output
#define PIN_STATUS_LED 18            ///< GPIO pin for connection status LED output

// Protocol Configuration
/**
 * @brief ATEM protocol timing and buffer settings
 * These values are based on the Sofie ATEM Connection library specification
 */
#define MAX_PACKET_SIZE 1456         ///< Maximum UDP packet size for ATEM protocol
#define HEARTBEAT_INTERVAL 5000      ///< Heartbeat interval in milliseconds (5 seconds)
#define CONNECTION_TIMEOUT 15000     ///< Connection timeout in milliseconds (15 seconds)
#define ACK_TIMEOUT 2000             ///< Acknowledgment timeout in milliseconds (2 seconds)
#define MAX_RETRIES 3                ///< Maximum packet retransmission attempts

// Debug Configuration
/**
 * @brief Debug output settings
 * Enable or disable various debug features for development and troubleshooting
 */
#define DEFAULT_DEBUG_ENABLED true   ///< Enable debug output by default
#define DEBUG_ENABLED true           ///< Legacy name for backward compatibility
#define SERIAL_BAUD_RATE 115200      ///< Serial communication baud rate
#define DEBUG_PACKET_HEX false       ///< Print hex dump of all packets (verbose)
#define DEBUG_HEARTBEAT false        ///< Print heartbeat debug messages
#define DEBUG_PACKET_FLOW true       ///< Print packet send/receive flow

// Buffer and Memory Configuration
/**
 * @brief Memory management settings
 * Configure buffer sizes and memory allocation for optimal performance
 */
#define UDP_BUFFER_SIZE 1500         ///< UDP receive buffer size
#define COMMAND_QUEUE_SIZE 10        ///< Maximum queued commands before overflow
#define MAX_INPUTS 20                ///< Maximum number of video inputs supported

// Timing Configuration
/**
 * @brief Main loop timing settings
 * Configure delays and intervals for optimal performance
 */
#define MAIN_LOOP_DELAY 10           ///< Main loop delay in milliseconds

// Connection Retry Configuration
/**
 * @brief Network reliability settings
 * Configure retry behavior for robust connection handling
 */
#define RECONNECT_DELAY 5000         ///< Delay between reconnection attempts (ms)
#define MAX_RECONNECT_ATTEMPTS 5     ///< Maximum automatic reconnection attempts
#define WIFI_CONNECT_TIMEOUT 10000   ///< WiFi connection timeout (ms)

// ATEM Protocol Constants
/**
 * @brief ATEM protocol-specific constants
 * These values are defined by the ATEM protocol specification
 */
#define ATEM_HELLO_PACKET_SIZE 20    ///< Size of initial handshake packet
#define ATEM_HEADER_SIZE 12          ///< Size of standard packet header
#define ATEM_ACK_PACKET_SIZE 12      ///< Size of acknowledgment packet
#define ATEM_SESSION_ID_OFFSET 2     ///< Byte offset of session ID in packet header

#endif // CONFIG_H
