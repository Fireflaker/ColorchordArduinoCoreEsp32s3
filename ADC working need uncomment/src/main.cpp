#include "ccconfig.h"
#include <Arduino.h>
#include <FastLED.h>
#include "driver/timer.h"

extern "C"
{
#include "DFT32.h"
#include "embeddednf.h"
#include "embeddedout.h"
}

// Configuration

#define ADC_PIN 1 // GPIO1 (ADC1_CHANNEL_0)

// LED Configuration
#define LED_PIN 21
#define COLOR_ORDER RGB
#define NUM_LIN_LEDS 300
CRGB leds[NUM_LIN_LEDS]; // fastLed library setup

// Circular buffer for audio samples
#define CIRCBUFSIZE 256

volatile int last_samp_pos; // Circular buffer index (volatile for ISR access)

int16_t sampbuff[CIRCBUFSIZE]; // Audio sample buffer (16-bit signed)volatile

volatile int samples; // Sample counter
// uint16_t buffer_head = 0;
// volatile uint16_t buffer_tail = 0;

int UseNumLinLeds = NUM_LIN_LEDS; // Number of addressable LEDs

// ADC Interrupt Handler needs to be called by internall timer interrupt and given real adc value readout
void ADCCallback(int16_t adcval) // Called on each ADC conversion
{
#ifdef TQFP32
  sampbuff[last_samp_pos] = adcval * Gain; // Apply gain for 32-pin package
#else
  sampbuff[last_samp_pos] = adcval; // Raw ADC for other boards
#endif
  last_samp_pos = ((last_samp_pos + 1) % CIRCBUFSIZE); // Circular buffer
  samples++;                                           // Increment sample counter
}

// below is to fill the void ADCCallback(int16_t adcval) with the real adc value readout
#include "driver/timer.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

// Timer configuration from very old my main.cpp, no idea if needed still
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t *timer = NULL;


void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  // int sample = (adc1_get_raw(ADC1_CHANNEL_0)); // 12-bit to 8-bit signed
  // sample_buffer[buffer_head] = sample ; // Scale to 16-bit
  // buffer_head = (buffer_head + 1) % BUFFER_SIZE;
  // used to be to fill buffer but now use the charils' code to fill instead
  ADCCallback(adc1_get_raw(ADC1_CHANNEL_0)); // Convert to signed? idk. test print
  portEXIT_CRITICAL_ISR(&timerMux);
}

// below is to set up the adc before void IRAM_ATTR onTimer() is called
// No changes made atm from my old main.cpp
void setup_audio()
{
  // Configure ADC
  // https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32s3/api-reference/peripherals/adc.html
  esp_adc_cal_characteristics_t adc_chars;
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12); // Correct attenuation
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 0, &adc_chars);
\
  // Configure timer for 8kHz sampling
  // timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz
  timer = timerBegin(0, 200, true);  // ESP32-S3 has 160MHz CPU, 80MHz APB clock
  timerAttachInterrupt(timer, &onTimer, true);
  // For 8kHz sampling ESP32S3 - need change
  timerAlarmWrite(timer, 1000000 / 8000, true);  // Direct 8kHz calculation
  timerAlarmEnable(timer);
}

// Frame Processing (ColorChord Core)
// no changes made from charles' code
// void NewFrame() // Called every 128 samples (~5.8ms @22kHz)
// {
//   uint8_t led_outs[NUM_LIN_LEDS * 3]; // RGB buffer for LEDs
//   HandleFrameInfo();                  // Process frequency/color data
//   UpdateLinearLEDs();                 // Generate LED patterns
//   SendSPI2812(ledOut, NUM_LIN_LEDS);  // Push to LEDs via SPI
// }

// New push newframe for fastLED
//  Required for C/C++ linkage
uint8_t ledOut[NUM_LIN_LEDS * 3];

extern "C" void NewFrame()
{
  HandleFrameInfo();
  UpdateLinearLEDs();

  // Directly copy to FastLED buffer
  for (int i = 0; i < NUM_LIN_LEDS; i++)
  {
    leds[i].r = ledOut[i * 3 + 0];
    leds[i].g = ledOut[i * 3 + 1];
    leds[i].b = ledOut[i * 3 + 2];
  }
  FastLED.show();
}


// void IRAM_ATTR onTimer() {
//   portENTER_CRITICAL_ISR(&timerMux);

//   // Read ADC and store in buffer
//   int16_t sample = analogRead(ADC_PIN) - 2048;  // Convert to signed
//   sample_buffer[buffer_head] = sample * 16;     // Scale to 16-bit range

//   buffer_head = (buffer_head + 1) % BUFFER_SIZE;

//   portEXIT_CRITICAL_ISR(&timerMux);
// }

// void setup_audio() {
//   // Configure ADC
//   analogReadResolution(12);  // Set ADC to 12-bit resolution
//   analogSetAttenuation(ADC_11db);  // 150mV - 2450mV range
//   pinMode(ADC_PIN, INPUT);

//   // Configure timer for 8kHz sampling
//   timer = timerBegin(0, 80, true);  // 80 MHz / 80 = 1 MHz
//   timerAttachInterrupt(timer, &onTimer, true);
//   timerAlarmWrite(timer, 1000000 / DFREQ, true);  // 125µs period
//   timerAlarmEnable(timer);
// }



// Need help to wdit from charils's main void

int this_samp = 0;
int wf = 0;

void setup()
{

  //Serial.begin(115200);

  // Initialize LEDs
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LIN_LEDS);
  leds[0] = CRGB::White;
  FastLED.show();
  delay(100);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(100);
  FastLED.setBrightness(64);

  // Setup audio input
  setup_audio();

  // Set root note offset
  RootNoteOffset = 3;

  // Initialize ColorChord
  InitColorChord();

  this_samp = 0;
  wf = 0;
}

// Need help to wdit from charils's main loop
void loop()
{
  if (this_samp != last_samp_pos)
  {
    PushSample32(sampbuff[this_samp]); // Can't put in full volume.

    this_samp = (this_samp + 1) % CIRCBUFSIZE;

    wf++;
    if (wf == 128)
    {
      NewFrame(); // Now contains the LED update
      wf = 0;
    }
  }
}

// static unsigned long last_process = 0;

// // Process available samples
// portENTER_CRITICAL(&timerMux);
// uint16_t available = (buffer_head - buffer_tail) % BUFFER_SIZE;
// portEXIT_CRITICAL(&timerMux);

// if (available >= 512)
// { // Process in blocks of 512
//   for (int i = 0; i < 512; i++)
//   {
//     PushSample32(sample_buffer[buffer_tail]);
//     buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
//   }

//   // Update frequency analysis
//   UpdateFreqs();
//   HandleFrameInfo();

//   // Update LEDs
//   UpdateLinearLEDs();

//   for (int i = 0; i < NUM_LIN_LEDS; i++)
//   {
//     leds[i].r = ledOut[i * 3 + 0];
//     leds[i].g = ledOut[i * 3 + 1];
//     leds[i].b = ledOut[i * 3 + 2];
//   }
//   FastLED.show();
// }

// delay(1); // Allow background tasks
