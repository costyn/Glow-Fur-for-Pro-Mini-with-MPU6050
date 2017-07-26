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

## Available Routines 

FastLED color palettes (moving patterns - pattern changes direction according to MPU pitch angle):
* Rainbow 
* Rainbow Stripe
* Ocean
* Heat
* Party
* Cloud
* Forest

Other routines:
* Fire2012 - changes motion speed in reference to MPU pitch; split in 2 to flow "down" both sides of the LED strip from the middle.
* Cylon - Larson scanner/cylon; changes colors in reference to MPU yaw
* Cylon Multi - 4 "scanners" moving simultaniously - not interactive yet
* Fade Glitter - sparkles/glitter that fades out slowly. More MPU movement = faster/more glitter
* Disco Glitter - sparkles/glitter that winks out immediately. More MPU movement = faster/more glitter
* Pulse2 - streamers of light slide down from middle to the sides
* Pulse Static - A pulse appears on random spots on the strip, fading in and out quickly
* Racers - 4 leds pixels race the strip, randomly speeding up and slowing down, overtaking each other
* Wave - Color filled strip changes brightness in reference to acceleration, color in reference to yaw
* Shake It - "shake out" colored pixels and streamers from ends of the strip by moving the MPU vigorously
* Strobe2 - Flashes all leds in a particular color above a certain acceleration threshhold, color in reference to yaw
* Black - no pattern, no reactions. Standby state