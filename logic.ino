void updateLogic() {
	switch (state) {
		case STATE_INIT:
		case STATE_OFF:
			if (presses[0] || presses[1]) {
				changeState(STATE_CALENDAR);
			}
			break;

		case STATE_CALENDAR:
			if (presses[0] == PRESS_LONG && presses[1] == PRESS_LONG) {
				changeState(STATE_RESET);
			}
			else if (presses[0] && presses[1]) {
				changeState(STATE_OFF);
			}
			else if (presses[0] == PRESS_LONG) {
				updateStatus(STATUS_FAILURE);
			}
			else if (presses[0]) {
				updateStatus(STATUS_SUCCESS);
			}
			else if (presses[1]) {
				revertStatus();
			}

			break;

		case STATE_RESET:
			if (!holds[0] || !holds[1]) {
				changeState(STATE_CALENDAR);
				break;
			}

			if (triggerReset) {
				triggerReset = false;
				reset();
				changeState(STATE_CALENDAR);
			}
			break;
	}
}

void changeState(int newState) {
	state = newState;
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
	EEPROM.update(EEPROM_STATUSES + date, newStatus);
}

void incrementDate() {
	date = (date + 1) % DATE_COUNT;
	EEPROM.update(EEPROM_DATE, date);
}

void decrementDate() {
	date = (date - 1 + DATE_COUNT) % DATE_COUNT;
	EEPROM.update(EEPROM_DATE, date);
}

void reset() {
	for (int i = 0; i < DATE_COUNT; ++i) {
		statuses[i] = STATUS_NONE;
		EEPROM.update(EEPROM_STATUSES + i, STATUS_NONE);
	}

	date = 0;
	EEPROM.update(EEPROM_DATE, 0);
}
