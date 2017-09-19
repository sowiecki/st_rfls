#include <Adafruit_NeoPixel.h>
#include <algorithm>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2      // Actually 04 on Makerfocus D1 Mini
#define TRIG_PIN 1 // Actually TX on Makerfocus D1 Mini
#define ECHO_PIN 0 // Actually D3 on Makerfocus D1 Mini
#define FADE_TIME 1
#define TUNING 9 // Tunes range-finding to cm distance between pixels
#define FPS 50
#define NUM_PIXELS 50
#define TRAIL_LENGTH 10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int colors[NUM_PIXELS];
int trailingPixels[TRAIL_LENGTH];

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

#if defined(__AVR_ATtiny85__)
  if (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif

  initColors();

  strip.begin();
  strip.show();
}

void loop() {
  float duration, distance;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) * 0.0344;

  simUpsideDownTouch(floor(distance) / TUNING);
}

void colorWipe(uint8_t pixel) {
   std::rotate(trailingPixels, trailingPixels + TRAIL_LENGTH - 1, trailingPixels + TRAIL_LENGTH);
   trailingPixels[0] = pixel;
   
  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    int *leaveLit = std::find(std::begin(trailingPixels), std::end(trailingPixels), pixel);
    
//    if (leaveLit == std::end(trailingPixels)) {
    strip.setPixelColor(i, 0);
//    }
  }
}

void initColors() {
  for (int i = 0; i < strip.numPixels(); i++) {
    int rgb = random(5);
    switch (rgb) {
      case 0:
        colors[i] = strip.Color(255, 0, 0);
        break;
      case 1:
        colors[i] = strip.Color(0, 255, 0);
        break;
      case 2:
        colors[i] = strip.Color(0, 0, 255);
        break;
      case 3:
        colors[i] = strip.Color(255, 255, 0);
        break;
      case 4:
        colors[i] = strip.Color(255, 0, 255);
        break;
    }
  }
}

void simUpsideDownTouch(uint8_t pixel) {
  colorWipe(pixel);

  for (uint8_t fade = 0; fade < FPS; fade++) {
    int brightness = fade;
    strip.setPixelColor(pixel, colors[pixel]);
    strip.show();
    delay(FADE_TIME);
  }

  for (uint8_t fade = FPS; fade > 0; fade--) {
    int brightness = fade;
    strip.setPixelColor(pixel, colors[pixel]);
    strip.show();
    delay(FADE_TIME);
  }
}
