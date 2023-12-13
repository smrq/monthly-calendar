#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define BRIGHTNESS 5

#define DATE_COUNT 366

#define SENSOR1_PIN A1
#define SENSOR2_PIN A4

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

#define DISPLAY_INIT 0
#define DISPLAY_CALENDAR 1
#define DISPLAY_RAINBOW 2
#define DISPLAY_OFF 3

#define STATUS_NONE 0
#define STATUS_SUCCESS 1
#define STATUS_FAILURE 2

#define LONG_PRESS_FRAMES 25

Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
int monthLengths[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int monthStarts[12];
int monthEnds[12];

int statuses[DATE_COUNT];
int date = 0;

int displayState = DISPLAY_INIT;

int sensorState1 = 0;
int sensorState2 = 0;

int animationPhase = 0;

void setup() {
	for (int i = 0; i < 12; ++i) {
		monthStarts[i] = i == 0 ? 0 : (monthStarts[i-1] + monthLengths[i-1]);
		monthEnds[i] = monthStarts[i] + monthLengths[i] - 1;
	}

	for (int i = 0; i < DATE_COUNT; ++i) {
		statuses[i] = EEPROM.read(EEPROM_STATUSES + i);
	}
	date = EEPROM.read(EEPROM_DATE);

	pixels.begin();
	pixels.setBrightness(BRIGHTNESS);

	pinMode(SENSOR1_PIN, INPUT);
	pinMode(SENSOR2_PIN, INPUT);
}

void loop() {
	pollInput();
	updateDisplay();
	delay(10);
}

void pollInput() {
	int pressed1 = digitalRead(SENSOR1_PIN);
	int pressed2 = digitalRead(SENSOR2_PIN);

	if (pressed1 || pressed2) {
		if (pressed1) {
			++sensorState1;
		}
		if (pressed2) {
			++sensorState2;
		}
	} else if (sensorState1 || sensorState2) {
		if (displayState == DISPLAY_CALENDAR) {
			if (sensorState1 > LONG_PRESS_FRAMES && sensorState2 > LONG_PRESS_FRAMES) {
				// Long press 1+2: rainbow mode
				changeDisplayState(DISPLAY_RAINBOW);
			}
			else if (sensorState1 > 0 && sensorState2 > 0) {
				// Short press 1+2: turn off display
				changeDisplayState(DISPLAY_OFF);
			}
			else if (sensorState1 > LONG_PRESS_FRAMES) {
				// Long press 1: failure today
				updateStatus(STATUS_FAILURE);
			}
			else if (sensorState1 > 0) {
				// Short press 1: success today
				updateStatus(STATUS_SUCCESS);
			}
			else if (sensorState2 > 0) {
				// Short press 2: revert
				revertStatus();
			}
		} else {
			changeDisplayState(DISPLAY_CALENDAR);
		}

		sensorState1 = 0;
		sensorState2 = 0;
	}
}

void changeDisplayState(int newState) {
	displayState = newState;
	animationPhase = 0;
}

void updateStatus(int newStatus) {
	int month = getMonthForDate(date);
	if (monthEnds[month] == date) {
		if (statuses[date] == STATUS_NONE) {
			setDateStatus(newStatus);
		} else {
			incrementDate();
		}
	} else {
		setDateStatus(newStatus);
		incrementDate();
	}
}

void revertStatus() {
	int month = getMonthForDate(date);
	if (monthStarts[month] == date && statuses[date] == STATUS_NONE) {
		decrementDate();
	} else if (monthEnds[month] == date && statuses[date] != STATUS_NONE) {
		setDateStatus(STATUS_NONE);
	} else {
		decrementDate();
		setDateStatus(STATUS_NONE);
	}
}

int getMonthForDate(int date) {
	for (int i = 11; i >= 0; --i) {
		if (monthStarts[i] <= date) {
			return i;
		}
	}
	return 0;
}

void setDateStatus(int newStatus) {
	statuses[date] = newStatus;
	// EEPROM.update(EEPROM_STATUSES + date, newStatus);
}

void incrementDate() {
	date = (date + 1) % DATE_COUNT;
	// EEPROM.update(EEPROM_DATE, date);
}

void decrementDate() {
	date = (date - 1 + DATE_COUNT) % DATE_COUNT;
	// EEPROM.update(EEPROM_DATE, date);
}

void updateDisplay() {
	switch (displayState) {
	case DISPLAY_INIT:
		displayInit();
		break;

	case DISPLAY_CALENDAR:
		displayCalendar();
		break;

	case DISPLAY_RAINBOW:
		displayRainbow();
		break;

	case DISPLAY_OFF:
		displayOff();
		break;
	}
}

void displayInit() {
	pixels.clear();
	pixels.setPixelColor(PIXEL(animationPhase/2), pixels.Color(255, 255, 255));
	pixels.show();

	++animationPhase;
	if (animationPhase > 2*PIXEL_COUNT) {
		changeDisplayState(DISPLAY_CALENDAR);
	}
}

void displayCalendar() {
	int month = getMonthForDate(date);
	int monthLength = monthLengths[month];
	int monthStart = monthStarts[month];

	if (date == monthEnds[month]) {
		bool allSuccess = true;
		for (int d = date; d >= monthStart; --d) {
			if (statuses[d] != STATUS_SUCCESS) {
				allSuccess = false;
			}
		}
		if (allSuccess) {
			displayRainbow();
			return;
		}
	}

	pixels.clear();
	for (int i = 0; i < PIXEL_COUNT; ++i) {
		if (i >= monthLength) {
			pixels.setPixelColor(PIXEL(i), COLOR_OOB);
		} else switch (statuses[monthStart + i]) {
			case STATUS_NONE:
				if (monthStart + i == date) {
					pixels.setPixelColor(PIXEL(i), animationPhase < 150 ? COLOR_TODAY1 : COLOR_TODAY2);
				} else {
					pixels.setPixelColor(PIXEL(i), COLOR_NONE);
				}
				break;

			case STATUS_SUCCESS:
				pixels.setPixelColor(PIXEL(i), COLOR_SUCCESS);
				break;

			case STATUS_FAILURE:
				pixels.setPixelColor(PIXEL(i), COLOR_FAILURE);
				break;
		}
	}
	pixels.show();

	++animationPhase;
	if (animationPhase >= 300) {
		animationPhase = 0;
	}
}

void displayRainbow() {
	pixels.clear();

	long hue = 256 * animationPhase;
	for (int i = 0; i < PIXEL_COUNT; ++i) {
		int pixelHue = hue + (i * 2048);
		pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
	}
	pixels.show();

	++animationPhase;
	if (animationPhase >= 256) {
		animationPhase = 0;
	}
}

void displayOff() {
	pixels.clear();
	pixels.show();
}
