#include "ccconfig.h"
#include <Arduino.h>
#include <FastLED.h>
#include "driver/timer.h"

// Configuration

#define ADC_PIN 1  // GPIO1 (ADC1_CHANNEL_0)

// Circular buffer for audio samples
#define BUFFER_SIZE 1024
volatile int16_t sample_buffer[BUFFER_SIZE];
volatile uint16_t buffer_head = 0;
volatile uint16_t buffer_tail = 0;

extern "C" {
  #include "DFT32.h"
  #include "embeddednf.h"
  #include "embeddedout.h"
}



// LED Configuration
#define LED_PIN 48
#define COLOR_ORDER GRB
CRGB leds[NUM_LIN_LEDS];

// Timer configuration
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  
  // Read ADC and store in buffer
  int16_t sample = analogRead(ADC_PIN) - 2048;  // Convert to signed
  sample_buffer[buffer_head] = sample * 16;     // Scale to 16-bit range
  
  buffer_head = (buffer_head + 1) % BUFFER_SIZE;
  
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup_audio() {
  // Configure ADC
  analogReadResolution(12);  // Set ADC to 12-bit resolution
  analogSetAttenuation(ADC_11db);  // 150mV - 2450mV range
  pinMode(ADC_PIN, INPUT);

  // Configure timer for 8kHz sampling
  timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000 / DFREQ, true);  // 125µs period
  timerAlarmEnable(timer);
}

void loop() {
  static unsigned long last_process = 0;
  
  // Process available samples
  portENTER_CRITICAL(&timerMux);
  uint16_t available = (buffer_head - buffer_tail) % BUFFER_SIZE;
  portEXIT_CRITICAL(&timerMux);

  if (available >= 512) {  // Process in blocks of 512
    for (int i = 0; i < 512; i++) {
      PushSample32(sample_buffer[buffer_tail]);
      buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    }

    // Update frequency analysis
    UpdateFreqs();
    HandleFrameInfo();
    
    // Update LEDs
    UpdateLinearLEDs();
    
    for(int i=0; i<NUM_LIN_LEDS; i++) {
      leds[i].r = ledOut[i*3+0];
      leds[i].g = ledOut[i*3+1];
      leds[i].b = ledOut[i*3+2];
    }
    FastLED.show();
  }

  delay(1);  // Allow background tasks
}

void setup() {

  Serial.begin(115200);
  
  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LIN_LEDS);
  FastLED.setBrightness(64);
  
  // Initialize ColorChord
  InitColorChord();
  
  // Setup audio input
  setup_audio();
  
  // Set root note offset
  RootNoteOffset = 3;
}