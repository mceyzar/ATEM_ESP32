#ifndef ATEM_MODELS_H
#define ATEM_MODELS_H

#include <stdint.h>

/**
 * @file ATEM_Models.h
 * @brief ATEM Model Detection and Capabilities
 * 
 * Comprehensive support for all ATEM switcher models with their specific input configurations
 * Based on official Blackmagic Design specifications and Sofie ATEM Connection library
 */

// ===========================================
// ATEM MODEL IDENTIFICATION
// ===========================================
enum ATEMModel {
    ATEM_MODEL_UNKNOWN = 0,
    
    // Mini Series
    ATEM_MINI = 1,
    ATEM_MINI_PRO = 2,
    ATEM_MINI_PRO_ISO = 3,
    ATEM_MINI_EXTREME = 4,
    ATEM_MINI_EXTREME_ISO = 5,
    
    // Television Studio Series
    ATEM_TVS_HD = 10,
    ATEM_TVS_HD8 = 11,
    ATEM_TVS_HD8_ISO = 12,
    ATEM_TVS_4K8 = 13,
    
    // Production Studio Series
    ATEM_PRODUCTION_STUDIO_4K = 20,
    
    // Constellation Series
    ATEM_CONSTELLATION_HD = 30,
    ATEM_CONSTELLATION_4K = 31,
    ATEM_CONSTELLATION_8K = 32,
    
    // SDI Series
    ATEM_SDI = 40,
    ATEM_SDI_PRO_ISO = 41,
    ATEM_SDI_EXTREME_ISO = 42
};

// ===========================================
// MODEL CAPABILITIES STRUCTURE
// ===========================================
struct ATEMCapabilities {
    ATEMModel model;
    const char* name;
    uint8_t camera_inputs;      // Number of camera inputs (1-N)
    uint8_t media_players;      // Number of media players (0-4)
    uint8_t color_generators;   // Number of color generators (0-2)
    bool has_bars;             // Color bars available
    bool has_black;            // Black/matte available
    bool has_multiview;        // Multiview output
    bool has_program_out;      // Program output as input
    bool has_preview_out;      // Preview output as input
    bool has_aux_outputs;      // AUX outputs
    uint8_t supersource_boxes; // SuperSource boxes (0-4)
    bool has_streaming;        // Built-in streaming
    bool has_recording;        // Built-in recording
    uint16_t max_input_id;     // Highest input ID for validation
};

// ===========================================
// ATEM MODEL CAPABILITIES DATABASE
// ===========================================
static const ATEMCapabilities ATEM_CAPABILITIES[] = {
    // Mini Series
    {
        .model = ATEM_MINI,
        .name = "ATEM Mini",
        .camera_inputs = 4,
        .media_players = 1,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = false,
        .has_program_out = false,
        .has_preview_out = false,
        .has_aux_outputs = false,
        .supersource_boxes = 0,
        .has_streaming = true,
        .has_recording = false,
        .max_input_id = 3020
    },
    {
        .model = ATEM_MINI_PRO,
        .name = "ATEM Mini Pro",
        .camera_inputs = 4,
        .media_players = 1,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = false,
        .supersource_boxes = 0,
        .has_streaming = true,
        .has_recording = true,
        .max_input_id = 10012
    },
    {
        .model = ATEM_MINI_PRO_ISO,
        .name = "ATEM Mini Pro ISO",
        .camera_inputs = 4,
        .media_players = 1,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = false,
        .supersource_boxes = 0,
        .has_streaming = true,
        .has_recording = true,
        .max_input_id = 10012
    },
    {
        .model = ATEM_MINI_EXTREME,
        .name = "ATEM Mini Extreme",
        .camera_inputs = 8,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = true,
        .has_recording = true,
        .max_input_id = 11001
    },
    {
        .model = ATEM_MINI_EXTREME_ISO,
        .name = "ATEM Mini Extreme ISO",
        .camera_inputs = 8,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = true,
        .has_recording = true,
        .max_input_id = 11001
    },
    
    // Television Studio Series
    {
        .model = ATEM_TVS_HD,
        .name = "ATEM Television Studio HD",
        .camera_inputs = 4,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 1,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    {
        .model = ATEM_TVS_HD8,
        .name = "ATEM Television Studio HD8",
        .camera_inputs = 8,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    {
        .model = ATEM_TVS_HD8_ISO,
        .name = "ATEM Television Studio HD8 ISO",
        .camera_inputs = 8,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = true,
        .max_input_id = 11001
    },
    {
        .model = ATEM_TVS_4K8,
        .name = "ATEM Television Studio 4K8",
        .camera_inputs = 8,
        .media_players = 4,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    
    // Production Studio Series
    {
        .model = ATEM_PRODUCTION_STUDIO_4K,
        .name = "ATEM Production Studio 4K",
        .camera_inputs = 20,
        .media_players = 4,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    
    // Constellation Series (Flagship models)
    {
        .model = ATEM_CONSTELLATION_HD,
        .name = "ATEM Constellation HD",
        .camera_inputs = 40,
        .media_players = 4,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    {
        .model = ATEM_CONSTELLATION_4K,
        .name = "ATEM Constellation 4K",
        .camera_inputs = 40,
        .media_players = 4,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    {
        .model = ATEM_CONSTELLATION_8K,
        .name = "ATEM Constellation 8K",
        .camera_inputs = 40,
        .media_players = 4,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 11001
    },
    
    // SDI Series
    {
        .model = ATEM_SDI,
        .name = "ATEM SDI",
        .camera_inputs = 8,
        .media_players = 1,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = false,
        .has_program_out = false,
        .has_preview_out = false,
        .has_aux_outputs = false,
        .supersource_boxes = 0,
        .has_streaming = false,
        .has_recording = false,
        .max_input_id = 3020
    },
    {
        .model = ATEM_SDI_PRO_ISO,
        .name = "ATEM SDI Pro ISO",
        .camera_inputs = 8,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 1,
        .has_streaming = true,
        .has_recording = true,
        .max_input_id = 11001
    },
    {
        .model = ATEM_SDI_EXTREME_ISO,
        .name = "ATEM SDI Extreme ISO",
        .camera_inputs = 8,
        .media_players = 2,
        .color_generators = 2,
        .has_bars = true,
        .has_black = true,
        .has_multiview = true,
        .has_program_out = true,
        .has_preview_out = true,
        .has_aux_outputs = true,
        .supersource_boxes = 4,
        .has_streaming = true,
        .has_recording = true,
        .max_input_id = 11001
    }
};

// ===========================================
// HELPER FUNCTIONS
// ===========================================

/**
 * Get ATEM model capabilities by model enum
 */
inline const ATEMCapabilities* getATEMCapabilities(ATEMModel model) {
    for (size_t i = 0; i < sizeof(ATEM_CAPABILITIES) / sizeof(ATEM_CAPABILITIES[0]); i++) {
        if (ATEM_CAPABILITIES[i].model == model) {
            return &ATEM_CAPABILITIES[i];
        }
    }
    return nullptr;
}

/**
 * Detect ATEM model from product name string (from discovery)
 */
inline ATEMModel detectATEMModel(const char* productName) {
    if (!productName) return ATEM_MODEL_UNKNOWN;
    
    // Convert to lowercase for comparison
    String name = String(productName);
    name.toLowerCase();
    
    // Mini Series Detection
    if (name.indexOf("mini extreme iso") >= 0) return ATEM_MINI_EXTREME_ISO;
    if (name.indexOf("mini extreme") >= 0) return ATEM_MINI_EXTREME;
    if (name.indexOf("mini pro iso") >= 0) return ATEM_MINI_PRO_ISO;
    if (name.indexOf("mini pro") >= 0) return ATEM_MINI_PRO;
    if (name.indexOf("mini") >= 0) return ATEM_MINI;
    
    // Television Studio Series
    if (name.indexOf("television studio hd8 iso") >= 0) return ATEM_TVS_HD8_ISO;
    if (name.indexOf("television studio hd8") >= 0) return ATEM_TVS_HD8;
    if (name.indexOf("television studio 4k8") >= 0) return ATEM_TVS_4K8;
    if (name.indexOf("television studio hd") >= 0) return ATEM_TVS_HD;
    
    // Production Studio Series
    if (name.indexOf("production studio 4k") >= 0) return ATEM_PRODUCTION_STUDIO_4K;
    
    // Constellation Series
    if (name.indexOf("constellation 8k") >= 0) return ATEM_CONSTELLATION_8K;
    if (name.indexOf("constellation 4k") >= 0) return ATEM_CONSTELLATION_4K;
    if (name.indexOf("constellation hd") >= 0) return ATEM_CONSTELLATION_HD;
    
    // SDI Series
    if (name.indexOf("sdi extreme iso") >= 0) return ATEM_SDI_EXTREME_ISO;
    if (name.indexOf("sdi pro iso") >= 0) return ATEM_SDI_PRO_ISO;
    if (name.indexOf("sdi") >= 0) return ATEM_SDI;
    
    return ATEM_MODEL_UNKNOWN;
}

/**
 * Check if an input is valid for the current ATEM model
 */
inline bool isValidInputForModel(uint16_t input, const ATEMCapabilities* caps) {
    if (!caps) return false;
    
    // Check camera inputs (1 to N)
    if (input >= 1 && input <= caps->camera_inputs) return true;
    
    // Check black
    if (input == 0 && caps->has_black) return true;
    
    // Check color bars
    if (input == 1000 && caps->has_bars) return true;
    
    // Check color generators
    if (input == 2001 && caps->color_generators >= 1) return true;
    if (input == 2002 && caps->color_generators >= 2) return true;
    
    // Check media players
    if (input == 3010 && caps->media_players >= 1) return true;
    if (input == 3020 && caps->media_players >= 2) return true;
    if (input == 3030 && caps->media_players >= 3) return true;
    if (input == 3040 && caps->media_players >= 4) return true;
    
    // Check program/preview outputs
    if (input == 10010 && caps->has_program_out) return true;
    if (input == 10011 && caps->has_preview_out) return true;
    
    // Check multiview
    if (input == 10012 && caps->has_multiview) return true;
    
    // Check if within max range
    return input <= caps->max_input_id;
}

#endif // ATEM_MODELS_H
