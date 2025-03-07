#include "ccconfig.h"

// Global configuration instance definition with defaults
struct CCSettings CCS = {
    .gSETTINGS_KEY = 0xCC01,         // ColorChord config identifier
    .gROOT_NOTE_OFFSET = 0,          // A = 0
    .gDFTIIR = 6,                    // Default IIR coefficient
    .gFUZZ_IIR_BITS = 1,             // Fuzzy bin default
    .gFILTER_BLUR_PASSES = 2,        // Default blur passes
    .gSEMIBITSPERBIN = 3,            // 3 bits per bin
    .gMAX_JUMP_DISTANCE = 4,         // Note tracking threshold
    .gMAX_COMBINE_DISTANCE = 7,      // Note combine distance
    .gAMP_1_IIR_BITS = 4,            // Slow response
    .gAMP_2_IIR_BITS = 2,            // Fast response
    .gMIN_AMP_FOR_NOTE = 80,         // New note threshold
    .gMINIMUM_AMP_FOR_NOTE_TO_DISAPPEAR = 64, // Note disappearance
    .gNOTE_FINAL_AMP = 12,           // LED brightness scaling
    .gNERF_NOTE_PORP = 15,           // Size normalization
    .gUSE_NUM_LIN_LEDS = 16,// Use configured LED count
    .gCOLORCHORD_ACTIVE = 1,         // Enable by default
    .gCOLORCHORD_OUTPUT_DRIVER = 0,  // Default output mode
    .gLED_DRIVER_MODE = 0,           // Default LED mode
    .gINITIAL_AMP = 500              // Initial amplitude baseline
};

