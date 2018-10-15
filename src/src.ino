#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define BOARD_TYPE 3 // Makerfocus D1 Mini == 0, hiletgo == 1, uno == 3
#define MAX_BRIGHTNESS 200

#if (BOARD_TYPE == 0)
#define PIN 2      // D4
#define TRIG_PIN 1 // D3
#define ECHO_PIN 0 // D2
#elif (BOARD_TYPE == 1)
#define PIN 5      // D1
#define TRIG_PIN 4 // D2
#define ECHO_PIN 2 // D4
#elif (BOARD_TYPE == 3)
#define PIN 5      // D1
#define TRIG_PIN 4 // D2
#define ECHO_PIN 2 // D4
#endif

#define FADE_TIME 100
#define TUNING 6 // Tunes range-finding to cm distance between pixels
#define DELAY 1000
#define ADJACENT_PIXELS 2 // Num of pixels lit on either side of position pixel

const int8_t NUM_PIXELS = 100;
const int8_t POS_DETECTION_GRACE_PERIOD = 100;
char *UP = "UP";
char *DOWN = "DOWN";
char *NOOP = "NOOP";
char *direction = UP;
uint32_t colors[NUM_PIXELS];
uint8_t noPosCountDown = POS_DETECTION_GRACE_PERIOD;

// Actual pixel targeted to light. Moves towards detected position value.
uint8_t pixel = 0;

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

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

void colorWipe(uint32_t color = 0) {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, color);
  }
}

void loop() {
  strf();
}

void strf() {
  float duration, distance;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(2000);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  // distance = microsecondsToInches(duration);

  int8_t position = floor(distance) / TUNING;
  Serial.println(position);

  const bool outOfRange = position <= NUM_PIXELS;

  // if (outOfRange && noPosCountDown > 0) {
  //   noPosCountDown--;
  // }

  // if (outOfRange && noPosCountDown == 0) {
  colorWipe();
  // } else {
  // noPosCountDown = POS_DETECTION_GRACE_PERIOD;
  simUpsideDownPresence(position);
  // }
}

void initColors() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    int rgb = i % 5;
    Serial.println(rgb);

    switch (rgb) {
    case 0:
      colors[i] = strip.Color(0, 255, 0); // green
      break;
    case 1:
      colors[i] = strip.Color(255, 0, 0); // red
      break;
    case 2:
      colors[i] = strip.Color(0, 0, 255); // blue
      break;
    case 3:
      colors[i] = strip.Color(255, 0, 255); // magenta
      break;
    case 4:
      colors[i] = strip.Color(0, 255, 255); // yellow
      break;
    }
  }
}

void simUpsideDownPresence(uint8_t position) {
  colorWipe();

  if (position == pixel || position > NUM_PIXELS) {
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

  strip.setPixelColor(pixel, colors[pixel]);
  for (uint8_t i = 0; i < ADJACENT_PIXELS; i++) {
    strip.setPixelColor(pixel + i, colors[pixel + i]);
    strip.setPixelColor(pixel - i, colors[pixel - i]);
  }

  strip.show();

  delay(DELAY / 100);
}
