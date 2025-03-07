#include "ccconfig.h"
#include <Arduino.h>
#include <FastLED.h>
#include "driver/timer.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

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
#define COLOR_ORDER RGB
CRGB leds[NUM_LIN_LEDS];

// Timer configuration
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  int16_t sample = (adc1_get_raw(ADC1_CHANNEL_0) >> 4) - 128; // 12-bit to 8-bit signed
  sample_buffer[buffer_head] = sample * 256; // Scale to 16-bit
  buffer_head = (buffer_head + 1) % BUFFER_SIZE;
  portEXIT_CRITICAL_ISR(&timerMux);
}


void setup_audio() {
  // Configure ADC
  // https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32s3/api-reference/peripherals/adc.html
  esp_adc_cal_characteristics_t adc_chars;
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12); // Correct attenuation
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 0, &adc_chars);

  // Configure timer for 8kHz sampling
  //timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz
  timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz
  timerAttachInterrupt(timer, &onTimer, true);
// For 8kHz sampling ESP32S3 - need change
  timerAlarmWrite(timer, 80000000 / 80 / 8000, true);  // 125µs period
  timerAlarmEnable(timer);

}

void loop() {
  static unsigned long last_process = 0;
  static unsigned long last_debug_time = 0;  // Track when we last printed debug info
  static uint32_t loop_count = 0;  // Track loop iterations per second

  // Measure loop speed
  loop_count++;

  // Process available samples
  portENTER_CRITICAL(&timerMux);
  //uint16_t available = (buffer_head - buffer_tail) % BUFFER_SIZE;//Problem: Fails when buffer_head < buffer_tail (negative result wraps)
  uint16_t available = (BUFFER_SIZE + buffer_head - buffer_tail) % BUFFER_SIZE;
  //Add Buffer Protection
  buffer_head %= BUFFER_SIZE;
  buffer_tail %= BUFFER_SIZE;
  portEXIT_CRITICAL(&timerMux);

  if (available >= 512) {  // Process in blocks of 512
    uint16_t safe_available = (available < 512) ? available : 512;
    Serial.println("\nProcessing 512 samples...");
      
      for (int i = 0; i < 512; i++) {
          int16_t sample = sample_buffer[buffer_tail];

          if (sample < -32000 || sample > 32000) {
            Serial.printf("Extreme sample detected: %d\n", sample);
        }
          // Print first 10 samples for debugging
          if (i < 10) {
              Serial.printf("Sample %d: %d\n", i, sample);
          }

          PushSample32(sample);
          buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
      }

      // Update frequency analysis
      UpdateFreqs();
      HandleFrameInfo();

      // Update LEDs
      UpdateLinearLEDs();
      for(int i = 0; i < NUM_LIN_LEDS; i++) {
          leds[i].r = ledOut[i*3+0];
          leds[i].g = ledOut[i*3+1];
          leds[i].b = ledOut[i*3+2];
      }
      FastLED.show();
  }

  delayMicroseconds(5000);  // Allow background tasks
}


void setup() {

  Serial.begin(115600);
  Serial.println("ColorChord ESP32S3 Begin");
  
  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LIN_LEDS);
  FastLED.setBrightness(64);
  
  // Initialize ColorChord
  InitColorChord();
  
  // Setup audio input
  setup_audio();
  
  // Set root note offset
  RootNoteOffset = 3;

  xTaskCreateUniversal(
    [](void* param) { while(1) ((decltype(&loop))param)(); }, 
    "main_loop", 
    16384, 
    (void*)&loop, 
    1, 
    NULL, 
    ARDUINO_RUNNING_CORE
  );
}


//Extras
