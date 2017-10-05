#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2      // Actually 04 on Makerfocus D1 Mini
#define TRIG_PIN 1 // Actually TX on Makerfocus D1 Mini
#define ECHO_PIN 0 // Actually D3 on Makerfocus D1 Mini
#define FADE_TIME 100
#define TUNING 5 // Tunes range-finding to cm distance between pixels
#define DELAY 2500
#define NUM_PIXELS 100
#define ADJACENT_PIXELS 16

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

char *UP = "UP";
char *DOWN = "DOWN";
char *NOOP = "NOOP";
char *direction = UP;
int colors[NUM_PIXELS];
int pixel = 0;

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

  int8_t pixel = floor(distance) / TUNING;

  colorWipe(pixel);
  simUpsideDownPresence(pixel);
}

void colorWipe(uint8_t pixel) {
  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
}

void initColors() {
  for (int i = 0; i < strip.numPixels(); i++) {
    int rgb = i % 5;
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
      colors[i] = strip.Color(0, 255, 255);
      break;
    }
  }
}

void simUpsideDownPresence(uint8_t position) {
  if (position == pixel || position > strip.numPixels()) {
    direction = NOOP;
  } else if (position > pixel) {
    direction = UP;
  } else if (position < pixel) {
    direction = DOWN;
  }

  if (direction == UP) {
    pixel++;
  } else if (direction == DOWN) {
    pixel--;
  }

  for (uint8_t i = 0; i < ADJACENT_PIXELS / 2; i++) {
    strip.setPixelColor(pixel + i, colors[pixel + i]);
    strip.setPixelColor(pixel - i, colors[pixel - i]);
  }
  strip.show();

  delayMicroseconds(DELAY);
}
