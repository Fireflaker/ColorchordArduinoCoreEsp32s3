//UNTESTED PROOF OF CONCEPT//

#include "ccconfig.h"
#include <Arduino.h>
#include <FastLED.h>
#include <Adafruit_ZeroI2S.h>


extern "C"
{
#include "DFT32.h"
#include "embeddednf.h"
#include "embeddedout.h"
}

// Configuration
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512  // Reduced for memory constraints

// LED Configuration
#define NUM_LEDS 10      // Circuit Playground has 10 built-in LEDs
#define LED_PIN 8        // CPX NeoPixel pin
#define COLOR_ORDER GRB
CRGB leds[NUM_LIN_LEDS];

// Audio buffers
int16_t sampbuff[BUFFER_SIZE];
volatile int last_samp_pos = 0;
volatile int samples = 0;

// ColorChord interface
uint8_t ledOut[NUM_LIN_LEDS * 3];

#include <Adafruit_ZeroI2S.h>

Adafruit_ZeroI2S i2s;


void setup_audio() {
  if (!i2s.begin(I2S_16_BIT, SAMPLE_RATE)) {
    Serial.println("I2S init failed!");
    while (1);
  }
  i2s.enableRx();  // Enable receive for microphone input
}

extern "C" void NewFrame()
{
  HandleFrameInfo();
  UpdateLinearLEDs();

  for (int i = 0; i < NUM_LIN_LEDS; i++)
  {
    leds[i].r = ledOut[i * 3 + 0];
    leds[i].g = ledOut[i * 3 + 1];
    leds[i].b = ledOut[i * 3 + 2];
  }
  FastLED.show();
}

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(128);

  // Initialize microphone
  setup_audio();

  // Initialize ColorChord
  InitColorChord();
  // was 3
  RootNoteOffset = 0;

  // LED test
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(100);
  leds[0] = CRGB::Black;
  FastLED.show();
  leds[0] = CRGB::Green;
  FastLED.show();
  delay(100);
  leds[0] = CRGB::Black;
  FastLED.show();
  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(100);
  leds[0] = CRGB::Black;
  FastLED.show();

  Serial.printf("Startup complete\n");
}

void loop()
{
  static int wf = 0;
  static unsigned long last_debug = 0;

  while (i2s.rxReady()) {  // Check if data is available
    int32_t left, right;
    i2s.read(&left, &right);  // Read stereo sample

    // Convert to mono (use left channel) and scale to 16-bit
    int16_t sample = (int16_t)(left >> 16);  // Adjust based on your data format

    sampbuff[last_samp_pos] = sample;
    last_samp_pos = (last_samp_pos + 1) % BUFFER_SIZE;
    samples++;

    PushSample32(sample);

    if (++wf >= 512) {  // Process frame every 512 samples
      NewFrame();
      wf = 0;
    }
  }

  // Debug output
  if (millis() - last_debug > 1000) {
    Serial.printf("Samples: %d | Buffer pos: %d\n", samples, last_samp_pos);
    last_debug = millis();
  }
}
