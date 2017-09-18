#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2      // Actually 04 on Makerfocus D1 Mini
#define TRIG_PIN 1 // Actually TX on Makerfocus D1 Mini
#define ECHO_PIN 0 // Actually D3 on Makerfocus D1 Mini
#define FADE_TIME 1
#define TUNING 8 // Tunes range-finding to cm distance between pixels
#define FPS 50
#define NUM_PIXELS 50

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int reds[NUM_PIXELS];
int greens[NUM_PIXELS];
int blues[NUM_PIXELS];

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

void colorWipe() {
  for (uint8_t pixel = 0; pixel < strip.numPixels(); pixel++) {
    strip.setPixelColor(pixel, 0);
  }
}

void initColors() {
  for (int i = 0; i < strip.numPixels(); i++) {
    reds[i] = random(256);
    greens[i] = random(256);
    blues[i] = random(256);
  }
}

void simUpsideDownTouch(uint8_t pixel) {
  colorWipe();

  int red = reds[pixel];
  int green = greens[pixel];
  int blue = blues[pixel];
  
  for (uint8_t fade = 0; fade < FPS; fade++) {
    int brightness = fade;
    strip.setPixelColor(pixel - 1, red - brightness, green - brightness, blue - brightness);
    strip.setPixelColor(pixel, red - brightness, green - brightness, blue - brightness);
    strip.setPixelColor(pixel + 1, red - brightness, green - brightness, blue - brightness);
    strip.show();
    delay(FADE_TIME);
  }

  for (uint8_t fade = FPS; fade > 0; fade--) {
    int brightness = fade;
    strip.setPixelColor(pixel - 1, red - brightness, green - brightness, blue - brightness);
    strip.setPixelColor(pixel, red - brightness, green - brightness, blue - brightness);
    strip.setPixelColor(pixel + 1, red - brightness, green - brightness, blue - brightness);
    strip.show();
    delay(FADE_TIME);
  }
}
