int sensorState[2] = {0};

void pollInput() {
	presses[0] = PRESS_NONE;
	presses[1] = PRESS_NONE;
	holds[0] = digitalRead(SENSOR0_PIN);
	holds[1] = digitalRead(SENSOR1_PIN);

	if (holds[0] || holds[1]) {
		sensorState[0] += holds[0];
		sensorState[1] += holds[1];

		if (!pressTriggered) {
			if (sensorState[0] >= PRESS_FRAMES_LONG && sensorState[1] >= PRESS_FRAMES_LONG) {
				presses[0] = PRESS_LONG;
				presses[1] = PRESS_LONG;
				pressTriggered = true;
			}
			else if (sensorState[0] >= PRESS_FRAMES_LONG && sensorState[1] < PRESS_FRAMES_SHORT) {
				presses[0] = PRESS_LONG;
				presses[1] = PRESS_NONE;
				pressTriggered = true;
			}
			else if (sensorState[0] < PRESS_FRAMES_SHORT && sensorState[1] >= PRESS_FRAMES_LONG) {
				presses[0] = PRESS_NONE;
				presses[1] = PRESS_LONG;
				pressTriggered = true;
			}
		}
	} else {
		if (!pressTriggered && (sensorState[0] || sensorState[1])) {
			presses[0] =
				sensorState[0] >= PRESS_FRAMES_LONG ? PRESS_LONG :
				sensorState[0] >= PRESS_FRAMES_SHORT ? PRESS_SHORT :
				PRESS_NONE;
			presses[1] =
				sensorState[1] >= PRESS_FRAMES_LONG ? PRESS_LONG :
				sensorState[1] >= PRESS_FRAMES_SHORT ? PRESS_SHORT :
				PRESS_NONE;
		}
		sensorState[0] = 0;
		sensorState[1] = 0;
		pressTriggered = false;
	}
}
