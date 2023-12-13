.PHONY: deps build flash

build: monthly-calendar.ino
	arduino-cli compile --fqbn arduino:avr:nano monthly-calendar.ino

flash: build
	arduino-cli upload -p /dev/cu.usbserial-210 --fqbn arduino:avr:nano:cpu=atmega328old monthly-calendar.ino

deps:
	arduino-cli lib install "Adafruit NeoPixel"
	