#ifndef _CCCONFIG_H
#define _CCCONFIG_H
// Prevent multiple inclusions
#pragma once


#include <stdint.h>       // For standard integer types
#include <Arduino.h>      // ESP32 core headers
#include <string.h>       // For memory functions

// Ensure ADC parameters match ESP32-S3
#define SOC_DAC_SUPPORTED 1  // Enable ESP32-S3-specific DAC features
#define DFREQ 8000.0       // Sample rate
#define BASE_FREQ 55.0f    // A1 note
#define ADC_ATTENUATION ADC_ATTEN_DB_12 //11 does not exsist

// Add ESP32-S3 specific calibration
#define ADC_CALIBRATION_VREF  1100  // 1.1V reference (ESP32-S3 specific)
#define ADC_OVERSAMPLE_SHIFT  2     // 4x oversampling (match ESP-IDF ADC DIGI)

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// Configuration Structure
struct CCSettings
{
    // Device Configuration
    uint16_t gSETTINGS_KEY;           ///< Unique settings identifier
    
    // Audio Processing Parameters
    uint16_t gROOT_NOTE_OFFSET;       ///< Base note offset (0 = A)
    uint16_t gDFTIIR;                 ///< DFT IIR filter coefficient (default: 6)
    uint16_t gFUZZ_IIR_BITS;          ///< Fuzzy bin IIR bits (default: 1)
    uint16_t gFILTER_BLUR_PASSES;     ///< Bin smoothing passes (default: 2)
    
    // Frequency Analysis
    uint16_t gSEMIBITSPERBIN;         ///< Bits per frequency bin (default: 3)
    uint16_t gMAX_JUMP_DISTANCE;      ///< Max note tracking distance (default: 4)
    uint16_t gMAX_COMBINE_DISTANCE;   ///< Note combining threshold (default: 7)
    
    // Amplitude Tracking
    uint16_t gAMP_1_IIR_BITS;         ///< Slow amplitude response (default: 4)
    uint16_t gAMP_2_IIR_BITS;         ///< Fast amplitude response (default: 2)
    uint16_t gMIN_AMP_FOR_NOTE;       ///< New note threshold (default: 80)
    uint16_t gMINIMUM_AMP_FOR_NOTE_TO_DISAPPEAR; ///< Note disappearance threshold (default: 64)
    
    // LED Control
    uint16_t gNOTE_FINAL_AMP;         ///< LED brightness scaling (default: 12)
    uint16_t gNERF_NOTE_PORP;         ///< Size normalization factor (default: 15)
    uint16_t gUSE_NUM_LIN_LEDS;       ///< Actual LED count (default: NUM_LIN_LEDS)
    
    // System Flags
    uint16_t gCOLORCHORD_ACTIVE;      ///< Master enable flag
    uint16_t gCOLORCHORD_OUTPUT_DRIVER; ///< Output mode selector
    uint16_t gLED_DRIVER_MODE;        ///< LED control mode
    uint16_t gINITIAL_AMP;            ///< Initial amplitude baseline
};

// Global configuration instance (declaration)
extern struct CCSettings CCS;

// Hardware Constants ---------------------------------------------------------
#ifndef FIXBPERO
#define FIXBPERO 24  ///< Bins per octave in frequency analysis
#endif

//< Total note range
#ifndef NOTERANGE
#define NOTERANGE ((1 << CCS.gSEMIBITSPERBIN) * FIXBPERO)
#endif

// Configuration Macros -------------------------------------------------------
#define ROOT_NOTE_OFFSET    CCS.gROOT_NOTE_OFFSET
#define DFTIIR              CCS.gDFTIIR
#define FUZZ_IIR_BITS       CCS.gFUZZ_IIR_BITS
#define FILTER_BLUR_PASSES  CCS.gFILTER_BLUR_PASSES
#define SEMIBITSPERBIN      CCS.gSEMIBITSPERBIN
#define MAX_JUMP_DISTANCE   CCS.gMAX_JUMP_DISTANCE
#define MAX_COMBINE_DISTANCE CCS.gMAX_COMBINE_DISTANCE
#define AMP_1_IIR_BITS      CCS.gAMP_1_IIR_BITS
#define AMP_2_IIR_BITS      CCS.gAMP_2_IIR_BITS
#define MIN_AMP_FOR_NOTE    CCS.gMIN_AMP_FOR_NOTE
#define MINIMUM_AMP_FOR_NOTE_TO_DISAPPEAR CCS.gMINIMUM_AMP_FOR_NOTE_TO_DISAPPEAR
#define NOTE_FINAL_AMP      CCS.gNOTE_FINAL_AMP
#define NERF_NOTE_PORP      CCS.gNERF_NOTE_PORP
#define USE_NUM_LIN_LEDS    CCS.gUSE_NUM_LIN_LEDS
#define COLORCHORD_OUTPUT_DRIVER CCS.gCOLORCHORD_OUTPUT_DRIVER
#define COLORCHORD_ACTIVE   CCS.gCOLORCHORD_ACTIVE
#define INITIAL_AMP         CCS.gINITIAL_AMP
#define LED_DRIVER_MODE     CCS.gLED_DRIVER_MODE

// Fixed Configuration --------------------------------------------------------
#define MAXNOTES  12         ///< Maximum simultaneous tracked notes
#define LIN_WRAPAROUND 0     ///< Disable LED wrap-around mode
#define SORT_NOTES 0         ///< Disable note sorting
#define CCCONFIG_ADDRESS 0xAF000 ///< EEPROM configuration address

#endif