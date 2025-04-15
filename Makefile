enable-port:
	@sudo chmod a+rw /dev/ttyUSB0

download-cli:
	@curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

install:
	@arduino-cli core update-index --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json

attach:
	@arduino-cli board attach -p /dev/ttyUSB0 -b esp8266:esp8266:nodemcuv2 ClosedDoors.ino --format json --log

build:
	@arduino-cli compile

upload:
	@arduino-cli upload
