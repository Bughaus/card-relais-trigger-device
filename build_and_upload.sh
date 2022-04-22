#!/bin/bash

arduino-cli compile --fqbn esp8266:esp8266:d1 TableDevice
arduino-cli upload -p 192.168.2.166 --fqbn esp8266:esp8266:d1 TableDevice
#arduino-cli upload -p /dev/cu.usbserial-142230 --fqbn esp8266:esp8266:d1 TableDevice