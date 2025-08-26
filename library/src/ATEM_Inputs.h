#ifndef ATEM_INPUTS_H
#define ATEM_INPUTS_H

#include <stdint.h>  // For uint16_t
#include <stdio.h>   // For snprintf
#include "ATEM_Models.h"  // For model detection and capabilities

/**
 * @file ATEM_Inputs.h
 * @brief Universal ATEM Input Channel Definitions
 * 
 * Supports all ATEM models with dynamic input validation based on detected model
 * These values match the input IDs used in CPvI/CPgI commands across all ATEM switchers
 */

// ===========================================
// CAMERA INPUTS (1-40 depending on model)
// ===========================================
#define ATEM_INPUT_CAM1         1      // Camera 1
#define ATEM_INPUT_CAM2         2      // Camera 2  
#define ATEM_INPUT_CAM3         3      // Camera 3
#define ATEM_INPUT_CAM4         4      // Camera 4
#define ATEM_INPUT_CAM5         5      // Camera 5 (8+ input models)
#define ATEM_INPUT_CAM6         6      // Camera 6 (8+ input models)
#define ATEM_INPUT_CAM7         7      // Camera 7 (8+ input models)
#define ATEM_INPUT_CAM8         8      // Camera 8 (8+ input models)
// Cameras 9-20 for Production Studio 4K
#define ATEM_INPUT_CAM9         9      // Camera 9 (20+ input models)
#define ATEM_INPUT_CAM10        10     // Camera 10 (20+ input models)
#define ATEM_INPUT_CAM11        11     // Camera 11 (20+ input models)
#define ATEM_INPUT_CAM12        12     // Camera 12 (20+ input models)
#define ATEM_INPUT_CAM13        13     // Camera 13 (20+ input models)
#define ATEM_INPUT_CAM14        14     // Camera 14 (20+ input models)
#define ATEM_INPUT_CAM15        15     // Camera 15 (20+ input models)
#define ATEM_INPUT_CAM16        16     // Camera 16 (20+ input models)
#define ATEM_INPUT_CAM17        17     // Camera 17 (20+ input models)
#define ATEM_INPUT_CAM18        18     // Camera 18 (20+ input models)
#define ATEM_INPUT_CAM19        19     // Camera 19 (20+ input models)
#define ATEM_INPUT_CAM20        20     // Camera 20 (20+ input models)
// Cameras 21-40 for Constellation series
#define ATEM_INPUT_CAM21        21     // Camera 21 (40 input models)
#define ATEM_INPUT_CAM22        22     // Camera 22 (40 input models)
#define ATEM_INPUT_CAM23        23     // Camera 23 (40 input models)
#define ATEM_INPUT_CAM24        24     // Camera 24 (40 input models)
#define ATEM_INPUT_CAM25        25     // Camera 25 (40 input models)
#define ATEM_INPUT_CAM26        26     // Camera 26 (40 input models)
#define ATEM_INPUT_CAM27        27     // Camera 27 (40 input models)
#define ATEM_INPUT_CAM28        28     // Camera 28 (40 input models)
#define ATEM_INPUT_CAM29        29     // Camera 29 (40 input models)
#define ATEM_INPUT_CAM30        30     // Camera 30 (40 input models)
#define ATEM_INPUT_CAM31        31     // Camera 31 (40 input models)
#define ATEM_INPUT_CAM32        32     // Camera 32 (40 input models)
#define ATEM_INPUT_CAM33        33     // Camera 33 (40 input models)
#define ATEM_INPUT_CAM34        34     // Camera 34 (40 input models)
#define ATEM_INPUT_CAM35        35     // Camera 35 (40 input models)
#define ATEM_INPUT_CAM36        36     // Camera 36 (40 input models)
#define ATEM_INPUT_CAM37        37     // Camera 37 (40 input models)
#define ATEM_INPUT_CAM38        38     // Camera 38 (40 input models)
#define ATEM_INPUT_CAM39        39     // Camera 39 (40 input models)
#define ATEM_INPUT_CAM40        40     // Camera 40 (40 input models)

// ===========================================
// SPECIAL INPUTS (1000+)
// ===========================================
#define ATEM_INPUT_BLACK        0      // Black/Matte
#define ATEM_INPUT_BARS         1000   // Color Bars
#define ATEM_INPUT_COLOR1       2001   // Color Generator 1
#define ATEM_INPUT_COLOR2       2002   // Color Generator 2

// ===========================================
// MEDIA PLAYERS (3010-3040 + keys)
// ===========================================
#define ATEM_INPUT_MP1          3010   // Media Player 1 (all models with MP)
#define ATEM_INPUT_MP1_KEY      3011   // Media Player 1 Key
#define ATEM_INPUT_MP2          3020   // Media Player 2 (2+ MP models)
#define ATEM_INPUT_MP2_KEY      3021   // Media Player 2 Key
#define ATEM_INPUT_MP3          3030   // Media Player 3 (4 MP models)
#define ATEM_INPUT_MP3_KEY      3031   // Media Player 3 Key
#define ATEM_INPUT_MP4          3040   // Media Player 4 (4 MP models)
#define ATEM_INPUT_MP4_KEY      3041   // Media Player 4 Key

// ===========================================
// SUPERSOURCE & AUX INPUTS (7000+)
// ===========================================
#define ATEM_INPUT_SUPERSOURCE  7001   // SuperSource (models with SS)
#define ATEM_INPUT_SUPERSOURCE2 7002   // SuperSource 2 (4K models)

// ===========================================
// AUX/OUTPUT FEEDS (10000+)
// ===========================================
#define ATEM_INPUT_PROGRAM      10010  // Program Output
#define ATEM_INPUT_PREVIEW      10011  // Preview Output  
#define ATEM_INPUT_MULTIVIEW    10012  // Multiview Output
#define ATEM_INPUT_AUX1         11001  // AUX 1 Output (models with AUX)
#define ATEM_INPUT_AUX2         11002  // AUX 2 Output
#define ATEM_INPUT_AUX3         11003  // AUX 3 Output
#define ATEM_INPUT_AUX4         11004  // AUX 4 Output
#define ATEM_INPUT_AUX5         11005  // AUX 5 Output
#define ATEM_INPUT_AUX6         11006  // AUX 6 Output

// ===========================================
// STREAMING & RECORDING FEEDS (12000+)
// ===========================================
#define ATEM_INPUT_STREAMING    12001  // Streaming Output (models with streaming)
#define ATEM_INPUT_RECORDING    12002  // Recording Output (models with recording)

// ===========================================
// ALIASES FOR CONVENIENCE
// ===========================================
#define ATEM_CAM1              ATEM_INPUT_CAM1
#define ATEM_CAM2              ATEM_INPUT_CAM2
#define ATEM_CAM3              ATEM_INPUT_CAM3
#define ATEM_CAM4              ATEM_INPUT_CAM4
#define ATEM_CAM5              ATEM_INPUT_CAM5
#define ATEM_CAM6              ATEM_INPUT_CAM6
#define ATEM_CAM7              ATEM_INPUT_CAM7
#define ATEM_CAM8              ATEM_INPUT_CAM8
#define ATEM_BLACK             ATEM_INPUT_BLACK
#define ATEM_BARS              ATEM_INPUT_BARS
#define ATEM_COL1              ATEM_INPUT_COLOR1
#define ATEM_COL2              ATEM_INPUT_COLOR2
#define ATEM_MP1               ATEM_INPUT_MP1
#define ATEM_MP2               ATEM_INPUT_MP2
#define ATEM_MP3               ATEM_INPUT_MP3
#define ATEM_MP4               ATEM_INPUT_MP4

// ===========================================
// DYNAMIC INPUT VALIDATION
// ===========================================
/**
 * Check if input is valid for detected ATEM model
 * Uses model capabilities for accurate validation
 */
bool isValidInput(uint16_t input, const ATEMCapabilities* capabilities = nullptr);

/**
 * Get maximum camera input for current model
 */
uint8_t getMaxCameraInput(const ATEMCapabilities* capabilities = nullptr);

/**
 * Get maximum media player for current model  
 */
uint8_t getMaxMediaPlayer(const ATEMCapabilities* capabilities = nullptr);

// ===========================================
// UNIVERSAL INPUT NAME FUNCTIONS
// ===========================================
inline const char* getInputName(uint16_t input) {
    // Camera inputs (1-40)
    if (input >= 1 && input <= 40) {
        static char camName[8];
        snprintf(camName, sizeof(camName), "CAM%d", input);
        return camName;
    }
    
    switch(input) {
        case ATEM_INPUT_BLACK:       return "BLACK";
        case ATEM_INPUT_BARS:        return "BARS";
        case ATEM_INPUT_COLOR1:      return "COL1";
        case ATEM_INPUT_COLOR2:      return "COL2";
        case ATEM_INPUT_MP1:         return "MP1";
        case ATEM_INPUT_MP2:         return "MP2";
        case ATEM_INPUT_MP3:         return "MP3";
        case ATEM_INPUT_MP4:         return "MP4";
        case ATEM_INPUT_SUPERSOURCE: return "SS1";
        case ATEM_INPUT_SUPERSOURCE2:return "SS2";
        case ATEM_INPUT_PROGRAM:     return "PGM";
        case ATEM_INPUT_PREVIEW:     return "PVW";
        case ATEM_INPUT_MULTIVIEW:   return "MVW";
        case ATEM_INPUT_AUX1:        return "AUX1";
        case ATEM_INPUT_AUX2:        return "AUX2";
        case ATEM_INPUT_AUX3:        return "AUX3";
        case ATEM_INPUT_AUX4:        return "AUX4";
        case ATEM_INPUT_AUX5:        return "AUX5";
        case ATEM_INPUT_AUX6:        return "AUX6";
        case ATEM_INPUT_STREAMING:   return "STRM";
        case ATEM_INPUT_RECORDING:   return "REC";
        default:                     return "UNKNOWN";
    }
}

inline const char* getInputDescription(uint16_t input) {
    // Camera inputs (1-40)
    if (input >= 1 && input <= 40) {
        static char camDesc[16];
        snprintf(camDesc, sizeof(camDesc), "Camera %d", input);
        return camDesc;
    }
    
    switch(input) {
        case ATEM_INPUT_BLACK:       return "Black";
        case ATEM_INPUT_BARS:        return "Color Bars";
        case ATEM_INPUT_COLOR1:      return "Color Generator 1";
        case ATEM_INPUT_COLOR2:      return "Color Generator 2";
        case ATEM_INPUT_MP1:         return "Media Player 1";
        case ATEM_INPUT_MP2:         return "Media Player 2";
        case ATEM_INPUT_MP3:         return "Media Player 3";
        case ATEM_INPUT_MP4:         return "Media Player 4";
        case ATEM_INPUT_SUPERSOURCE: return "SuperSource 1";
        case ATEM_INPUT_SUPERSOURCE2:return "SuperSource 2";
        case ATEM_INPUT_PROGRAM:     return "Program Output";
        case ATEM_INPUT_PREVIEW:     return "Preview Output";
        case ATEM_INPUT_MULTIVIEW:   return "Multiview Output";
        case ATEM_INPUT_AUX1:        return "AUX 1 Output";
        case ATEM_INPUT_AUX2:        return "AUX 2 Output";
        case ATEM_INPUT_AUX3:        return "AUX 3 Output";
        case ATEM_INPUT_AUX4:        return "AUX 4 Output";
        case ATEM_INPUT_AUX5:        return "AUX 5 Output";
        case ATEM_INPUT_AUX6:        return "AUX 6 Output";
        case ATEM_INPUT_STREAMING:   return "Streaming Output";
        case ATEM_INPUT_RECORDING:   return "Recording Output";
        default:                     return "Unknown Input";
    }
}

#endif // ATEM_INPUTS_H
