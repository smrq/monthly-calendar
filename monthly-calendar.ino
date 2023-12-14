#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define DATE_COUNT 366

#define SENSOR0_PIN A1
#define SENSOR1_PIN A4

#define PIXEL_BRIGHTNESS 5
#define PIXEL_PIN 6
#define PIXEL_COUNT 31
#define PIXEL(n) (PIXEL_COUNT - 1 - (n))

#define EEPROM_STATUSES 0
#define EEPROM_DATE (EEPROM_STATUSES + DATE_COUNT)

#define COLOR_NONE (pixels.Color(0, 0, 0))
#define COLOR_SUCCESS (pixels.Color(31, 150, 42))
#define COLOR_FAILURE (pixels.Color(255, 51, 0))
#define COLOR_OOB (pixels.Color(255, 165, 0))
#define COLOR_TODAY1 (pixels.Color(64, 64, 64))
#define COLOR_TODAY2 (pixels.Color(64, 64, 192))
#define COLOR_RESET (pixels.Color(255, 0, 0))
#define COLOR_INIT (pixels.Color(255, 255, 255))

#define STATE_INIT 0
#define STATE_CALENDAR 1
#define STATE_OFF 2
#define STATE_RESET 3

#define STATUS_NONE 0
#define STATUS_SUCCESS 1
#define STATUS_FAILURE 2

#define PRESS_NONE 0
#define PRESS_SHORT 1
#define PRESS_LONG 2
#define PRESS_FRAMES_SHORT 3
#define PRESS_FRAMES_LONG 30

Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
int monthLengths[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int monthStarts[12];
int monthEnds[12];

int statuses[DATE_COUNT];
int date = 0;

int state = STATE_INIT;

int holds[2] = {0};
int presses[2] = {0};
bool pressTriggered = false;

int animationPhase = 0;
bool triggerReset = false;

void setup() {
	pixels.begin();
	pixels.setBrightness(PIXEL_BRIGHTNESS);

	pinMode(SENSOR0_PIN, INPUT);
	pinMode(SENSOR1_PIN, INPUT);

	for (int i = 0; i < 12; ++i) {
		monthStarts[i] = i == 0 ? 0 : (monthStarts[i-1] + monthLengths[i-1]);
		monthEnds[i] = monthStarts[i] + monthLengths[i] - 1;
	}

	for (int i = 0; i < DATE_COUNT; ++i) {
		statuses[i] = EEPROM.read(EEPROM_STATUSES + i);
	}

	date = EEPROM.read(EEPROM_DATE);
}

void loop() {
	pollInput();
	updateLogic();
	updateDisplay();
	delay(10);
}
