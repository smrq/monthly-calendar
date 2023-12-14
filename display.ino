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
	pixels.clear();
	pixels.setPixelColor(PIXEL(animationPhase/2), COLOR_INIT);
	pixels.show();

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
	animationPhase = (animationPhase + 1) % 300;
}

void displayRainbow() {
	pixels.clear();

	long hue = 256 * animationPhase;
	for (int i = 0; i < PIXEL_COUNT; ++i) {
		int pixelHue = hue + (i * 2048);
		pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
	}
	pixels.show();
	animationPhase = (animationPhase + 1) % 256;
}

void displayOff() {
	pixels.clear();
	pixels.show();
}

void displayReset() {
	pixels.clear();
	for (int i = 0; i < animationPhase/4; ++i) {
		pixels.setPixelColor(PIXEL(i), COLOR_RESET);
	}
	pixels.show();

	++animationPhase;
	if (animationPhase >= 4*(PIXEL_COUNT+1)) {
		triggerReset = true;
	}
}
