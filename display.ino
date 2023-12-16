void clearPixels() {
	pixels0.clear();
	pixels1.clear();
	pixels2.clear();
}

void setPixelColor(int pixel, uint32_t color) {
	if (pixel >= 20) {
		pixels2.setPixelColor(30 - pixel, color);
	}
	else if (pixel >= 10) {
		pixels1.setPixelColor(19 - pixel, color);
	}
	else {
		pixels0.setPixelColor(9 - pixel, color);
	}
}

void updatePixels() {
	pixels0.show();
	pixels1.show();
	pixels2.show();
}

void updateBrightness() {
	int value = analogRead(LIGHT_SENSOR_PIN);
	value -= LIGHT_SENSOR_MIN;
	if (value < 0) value = 0;
	value *= PIXEL_BRIGHTNESS_MAX - PIXEL_BRIGHTNESS_MIN;
	value /= LIGHT_SENSOR_MAX - LIGHT_SENSOR_MIN;
	value += PIXEL_BRIGHTNESS_MIN;
	if (value > PIXEL_BRIGHTNESS_MAX) value = PIXEL_BRIGHTNESS_MAX;

	int currentBrightness = pixels0.getBrightness();
	if (abs(value - currentBrightness) > 1) {
		pixels0.setBrightness(value);
		pixels1.setBrightness(value);
		pixels2.setBrightness(value);
	}
}

void updateDisplay() {
	switch (state) {
	case STATE_INIT:
		displayInit();
		break;

	case STATE_CALENDAR:
		displayCalendar();
		break;

	case STATE_OFF:
		displayOff();
		break;

	case STATE_RESET:
		displayReset();
		break;
	}
}

void displayInit() {
	clearPixels();
	setPixelColor(animationPhase/2, COLOR_INIT);
	updatePixels();

	++animationPhase;
	if (animationPhase >= 2*PIXEL_COUNT) {
		changeState(STATE_CALENDAR);
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

	clearPixels();
	for (int i = 0; i < PIXEL_COUNT; ++i) {
		if (i >= monthLength) {
			setPixelColor(i, COLOR_OOB);
		} else switch (statuses[monthStart + i]) {
			case STATUS_NONE:
				if (monthStart + i == date) {
					setPixelColor(i, animationPhase < 150 ? COLOR_TODAY1 : COLOR_TODAY2);
				} else {
					setPixelColor(i, COLOR_NONE);
				}
				break;

			case STATUS_SUCCESS:
				setPixelColor(i, COLOR_SUCCESS);
				break;

			case STATUS_FAILURE:
				setPixelColor(i, COLOR_FAILURE);
				break;
		}
	}
	updatePixels();
	animationPhase = (animationPhase + 1) % 300;
}

void displayRainbow() {
	clearPixels();

	long hue = 256 * animationPhase;
	for (int i = 0; i < PIXEL_COUNT; ++i) {
		int pixelHue = hue + (i * 2048);
		setPixelColor(PIXEL_COUNT-1 - i, pixels0.gamma32(pixels0.ColorHSV(pixelHue)));
	}
	updatePixels();
	animationPhase = (animationPhase + 1) % 256;
}

void displayOff() {
	clearPixels();
	updatePixels();
}

void displayReset() {
	clearPixels();
	for (int i = 0; i < animationPhase/4; ++i) {
		setPixelColor(i, COLOR_RESET);
	}
	updatePixels();

	++animationPhase;
	if (animationPhase >= 4*(PIXEL_COUNT+1)) {
		triggerReset = true;
	}
}
