# Glow Fur for Arduino Pro Mini with MPU6050

My "fork" of the Adafruit Glow Fur adapter for use with 6-dof accelerometer/gyroscope MPU6050.

I adapted the code from Adafruit with additional routines for the Pro Mini, which has a lot more flash and SRAM play.
The original/inspiration: https://learn.adafruit.com/animated-neopixel-gemma-glow-fur-scarf?embeds=allow

## Hardware Notes

** !!! WARNING !!! Do not use onboard power regulator to power LEDs. It will quickly fry !!! **  
(don't use the RAW/VIN pin for voltage input!)

Put regulated 5v to any other VCC pin on the board. For example from a USB Power Bank (any that can provide 1000 mA or more).

Fritzing circuit files soon to follow

## Required Libraries

* https://github.com/FastLED/FastLED
* https://github.com/arkhipenko/TaskScheduler
* https://github.com/jrowberg/i2cdevlib