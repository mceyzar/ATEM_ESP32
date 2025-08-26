/*
 * config_sample.h
 * Configuration template for ATEM ESP32 library
 * 
 * Copy this file to config.h and edit the values below
 * to match your network and ATEM setup.
 */

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
// Replace with your actual WiFi network credentials
#define WIFI_SSID "Your_WiFi_Network_Name"
#define WIFI_PASSWORD "Your_WiFi_Password"

// ATEM Configuration  
// Replace with your ATEM switcher's IP address
#define ATEM_IP "192.168.1.100"

// Optional: Logging Configuration
// Uncomment to enable detailed debug output
// #define ATEM_DEFAULT_LOG_LEVEL ATEM_LOG_DEBUG

// Optional: Advanced Settings
// #define ATEM_PACKET_TIMEOUT 5000     // Packet timeout in milliseconds
// #define ATEM_RECONNECT_DELAY 3000    // Reconnection delay in milliseconds

#endif // CONFIG_H
