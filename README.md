# OHS Arduino
 OpenHayStack Arduino sketch

This sketch is forked from https://github.com/liucoj/OHS-Arduino which in
turn is based upon OpenHayStack main.c Firmware code for ESP32.
https://github.com/seemoo-lab/openhaystack
I removed the lcd, fixed the advertised address, put the key in a
separate source (key.h) and added a python script to generate key.h from
the base64 advertisement key.


# Setup
- Generate the keys using, e.g., the generate_keys.py script from https://github.com/biemster/FindMy/
- run genkey.py to put the advertisement key in key.h
- Open .ino in Arduino IDE
